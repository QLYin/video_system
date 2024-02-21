#include "audioplayer.h"
#include "audiohelper.h"
#include "qthread.h"
#include "qvariant.h"
#include "qdebug.h"

AudioPlayer::AudioPlayer(bool audioThread, QObject *parent) : QObject(parent)
{
    volume = 100;
    muted = false;
    audioLevel = false;
    this->audioThread = audioThread;

    audioInput = NULL;
    deviceInput = NULL;
    audioOutput = NULL;
    deviceOutput = NULL;

    //注册数据类型
    qRegisterMetaType<const char *>("const char *");

    //移到线程执行防止界面卡主导致声音卡主
    if (audioThread) {
        QThread *thread = new QThread;
        this->moveToThread(thread);
        thread->start();
    }
}

AudioPlayer::~AudioPlayer()
{
    this->closeAudioInput();
    this->closeAudioOutput();
    if (audioThread) {
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)
        this->thread()->terminate();
#else
        //QThread::wait: Thread tried to wait on itself
        this->thread()->quit();
        this->thread()->wait();
#endif
    }
}

QAudioInputx *AudioPlayer::getAudioInput()
{
    return this->audioInput;
}

QAudioOutputx *AudioPlayer::getAudioOutput()
{
    return this->audioOutput;
}

bool AudioPlayer::getIsOk() const
{
    bool isOk = false;
#ifdef multimedia
    if (audioOutput) {
        isOk = (audioOutput->state() != QAudio::StoppedState);
    }
#endif
    return isOk;
}

int AudioPlayer::getVolume() const
{
    qreal value = 1.0;
#ifdef multimedia5
    if (audioOutput) {
        value = audioOutput->volume();
#if (QT_VERSION >= QT_VERSION_CHECK(5,8,0))
        value = QAudio::convertVolume(value, QAudio::LinearVolumeScale, QAudio::LogarithmicVolumeScale);
#endif
    }
#endif
    return value * 100;
}

void AudioPlayer::setVolume(int volume)
{
    //Qt5.8版本以下在静音状态下不能设置音量大小卧槽
    this->volume = volume;
    emit receiveVolume(volume);
#ifdef multimedia5
    if (audioOutput) {
        qreal value = (qreal)volume / 100;
#if (QT_VERSION >= QT_VERSION_CHECK(5,8,0))
        value = QAudio::convertVolume(value, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
#endif
        audioOutput->setVolume(value);
    }
#endif
}

bool AudioPlayer::getMuted() const
{
    return this->muted;
    //return (getVolume() == 0);
}

void AudioPlayer::setMuted(bool muted)
{
    //Qt6中的QAudioSink音频类用挂起和复位失效(需要用静音标志位来控制是否写入数据实现静音)
    this->muted = muted;
    emit receiveMuted(muted);
    //发现用标志位这个万能办法最完美无需继续
    return;

#ifdef multimedia
    //本身没有提供静音函数接口(采用挂起和复位来实现静音切换)
    if (muted) {
        audioOutput->suspend();
    } else {
        audioOutput->resume();
    }
#endif
}

bool AudioPlayer::getAudioLevel() const
{
    return this->audioLevel;
}

void AudioPlayer::setAudioLevel(bool audioLevel)
{
    this->audioLevel = audioLevel;
}

void AudioPlayer::readyRead()
{
#ifdef multimedia
    //将音频输入设备数据发给音频输出设备播放
    if (deviceInput && deviceOutput) {
        QByteArray data = deviceInput->readAll();
        emit receiveInputData(data);
        playAudioData(data.constData(), data.length());
    }
#endif
}

void AudioPlayer::openAudioInput(int sampleRate, int channelCount, int sampleSize)
{
    this->openAudioInput("", sampleRate, channelCount, sampleSize);
}

void AudioPlayer::openAudioInput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize)
{
#ifdef multimedia
    //先关闭设备
    this->closeAudioInput();
    //初始化音频格式
    QAudioFormat format;
    AudioHelper::initAudioFormat(format, sampleRate, channelCount, sampleSize);

    //找到指定设备并实例化音频类(为空则采用默认设备)
    if (deviceName.isEmpty()) {
        audioInput = new QAudioInputx(format, this);
    } else {
        QAudioDevice device = AudioHelper::getAudioDevice(deviceName, true);
        audioInput = new QAudioInputx(device, format, this);
    }

    //启动音频服务
    deviceInput = audioInput->start();
    //将音频输入设备的数据读取出来
    if (deviceInput) {
        connect(deviceInput, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }
#endif
}

void AudioPlayer::closeAudioInput()
{
#ifdef multimedia
    if (audioInput) {
        audioInput->deleteLater();
        audioInput = NULL;
    }
#endif
}

void AudioPlayer::openAudioOutput(int sampleRate, int channelCount, int sampleSize)
{
    this->openAudioOutput("", sampleRate, channelCount, sampleSize);
}

void AudioPlayer::openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize)
{
#ifdef multimedia
    //先关闭设备
    this->closeAudioOutput();
    //初始化音频格式
    QAudioFormat format;
    AudioHelper::initAudioFormat(format, sampleRate, channelCount, sampleSize);

    //找到指定设备并实例化音频类(为空则采用默认设备)
    if (deviceName.isEmpty()) {
        audioOutput = new QAudioOutputx(format, this);
    } else {
        QAudioDevice device = AudioHelper::getAudioDevice(deviceName, false);
        audioOutput = new QAudioOutputx(device, format, this);
    }

    //启动音频服务
    deviceOutput = audioOutput->start();
#endif
}

void AudioPlayer::openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize, int volume)
{
    this->openAudioOutput(deviceName, sampleRate, channelCount, sampleSize);
    if (volume > 0) {
        this->setVolume(volume);
    }
}

void AudioPlayer::closeAudioOutput()
{
#ifdef multimedia
    muted = false;
    if (audioOutput) {
        audioOutput->deleteLater();
        audioOutput = NULL;
    }
#endif
}

void AudioPlayer::playAudioData(const char *data, qint64 len)
{
#ifdef multimedia
    if (deviceOutput) {
        emit receiveOutputData(data, len);
        //限定静音状态下不写入数据减轻压力
        if (!muted) {
            deviceOutput->write(data, len);
        }

        //获取音频数据振幅
        if (audioLevel) {
            qreal leftLevel, rightLevel;
            AudioHelper::getAudioLevel(audioOutput->format(), data, len, leftLevel, rightLevel);
            emit receiveLevel(leftLevel, rightLevel);
        }
    }
#endif
}
