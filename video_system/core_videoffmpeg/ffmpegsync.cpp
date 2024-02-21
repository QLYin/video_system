#include "ffmpegsync.h"
#include "ffmpeghelper.h"
#include "ffmpegthread.h"

FFmpegSync::FFmpegSync(quint8 type, QObject *parent) : QThread(parent)
{
    this->stopped = false;
    this->type = type;
    this->thread = (FFmpegThread *)parent;
}

FFmpegSync::~FFmpegSync()
{

}

void FFmpegSync::run()
{
    if (!thread) {
        return;
    }

    this->reset();
    while (!stopped) {
        //暂停状态或者切换进度中或者队列中没有帧则不处理
        if (!thread->isPause && !thread->changePosition && packets.count() > 0) {
            mutex.lock();
            AVPacket *packet = packets.first();
            mutex.unlock();

            //h264的裸流文件同步有问题因为获取不到pts和dts(暂时用最蠢的延时办法解决)
            if (thread->formatName == "h264" || thread->formatName == "hevc") {
                int sleepTime = (1000 / thread->frameRate) - 5;
                sleepTime = (double)sleepTime / thread->speed;
                msleep(sleepTime);
            } else {
                //计算当前帧显示时间(外部时钟同步)
                ptsTime = FFmpegHelper::getPtsTime(thread->formatCtx, packet);
                if (!this->checkPtsTime()) {
                    msleep(1);
                    continue;
                }

                //显示当前的播放进度
                this->checkShowTime();
            }

            //如果解码线程停止了则不用处理
            if (!thread->stopped) {
                //0-表示音频 1-表示视频
                if (type == 0) {
                    thread->decodeAudio1(packet);
                } else if (type == 1) {
                    thread->decodeVideo1(packet);
                }
            }

            //释放资源并移除
            mutex.lock();
            FFmpegHelper::freePacket(packet);
            packets.removeFirst();
            mutex.unlock();
        }

        if (thread->caching != 102400000) {
            msleep(1);
        }

    }

    this->reset();
    this->clear();
    stopped = false;
}

void FFmpegSync::stop()
{
    if (this->isRunning()) {
        stopped = true;
        this->wait();
    }
}

void FFmpegSync::clear()
{
    mutex.lock();
    //释放还没有来得及处理的剩余的帧
    foreach (AVPacket *packet, packets) {
        FFmpegHelper::freePacket(packet);
    }
    packets.clear();
    mutex.unlock();
}

void FFmpegSync::reset()
{
    //复位音频外部时钟
    showTime = 0;
    bufferTime = 0;
    offsetTime = -1;
    startTime = av_gettime();
}

void FFmpegSync::append(AVPacket *packet)
{
    mutex.lock();
    packets << packet;
    mutex.unlock();
}

int FFmpegSync::getPacketCount()
{
    return this->packets.count();
}

bool FFmpegSync::checkPtsTime()
{
    //下面这几个时间值是参考的别人的
    bool ok = false;
    if (ptsTime > 0) {
        if (ptsTime > offsetTime + 100000) {
            bufferTime = ptsTime - offsetTime + 100000;
        }

        int offset = (type == 0 ? 1000 : 5000);
        //做梦都想不到倍速播放就这里控制个系数就行
        offsetTime = (av_gettime() - startTime) * thread->speed + bufferTime;
        if ((offsetTime <= ptsTime && ptsTime - offsetTime <= offset) || (offsetTime > ptsTime)) {
            ok = true;
        }
    } else {
        ok = true;
    }

    return ok;
}

void FFmpegSync::checkShowTime()
{
    //必须是文件(本地文件或网络文件)才有播放进度
    if (!thread->getIsFile()) {
        return;
    }

    //过滤重复发送播放时间
    bool showPosition = false;
    bool existVideo = (thread->videoIndex >= 0);
    if (type == 0) {
        //音频同步线程不能存在视频
        if (!existVideo) {
            showPosition = true;
        }
    } else if (type == 1) {
        //视频同步线程必须存在视频
        if (existVideo) {
            showPosition = true;
        }
    }

    //需要显示时间的时候发送对应进度(限定差值大于200毫秒/没必要频繁发送)
    if (showPosition && (ptsTime - showTime > 200000)) {
        showTime = ptsTime;
        thread->position = ptsTime / 1000;
        emit receivePosition(thread->position);
    }
}
