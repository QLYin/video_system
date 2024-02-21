#include "videohelper.h"
#include "videoutil.h"
#include "urlhelper.h"
#include "widgethelper.h"

QString VideoHelper::getVersion()
{
    return VideoUtil::getVersion();
}

QString VideoHelper::getVersion(const VideoCore &videoCore)
{
    return VideoUtil::getVersion(videoCore);
}

VideoCore VideoHelper::getVideoCore(int type)
{
    return VideoUtil::getVideoCore(type);
}

void VideoHelper::renameFile(const QString &fileName)
{
    VideoUtil::renameFile(fileName);
}

void VideoHelper::resetCursor()
{
    VideoUtil::resetCursor();
}

void VideoHelper::rotateImage(int rotate, QImage &image)
{
    if (rotate > 0) {
        QTransform matrix;
        matrix.rotate(rotate);
        image = image.transformed(matrix, Qt::SmoothTransformation);
    }
}

void VideoHelper::rotateSize(int rotate, int &width, int &height)
{
    WidgetHelper::rotateSize(rotate, width, height);
}

int VideoHelper::getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax)
{
    return (((oldValue - oldMin) * (newMax - newMin)) / (oldMax - oldMin)) + newMin;
}

bool VideoHelper::checkUrl(const QString &url, int timeout)
{
    return WidgetHelper::checkUrl(url, timeout);
}

bool VideoHelper::checkUrl(VideoThread *videoThread, const VideoType &videoType, const QString &videoUrl, int timeout)
{
    //因为每个视频采集子类都会执行这个方法/所以通用的处理放在这里

    //udp://开头的表示必须用udp协议
    if (videoUrl.startsWith("udp://")) {
        videoThread->setTransport("udp");
    }

    //本地设备采集不支持硬解码
    if (videoType == VideoType_Camera || videoType == VideoType_Desktop) {
        videoThread->setHardware("none");
    }

    //如果指定窗口采集则不能设置分辨率
    if (videoUrl.contains("title=")) {
        videoThread->setBufferSize("0x0");
    }

    QString error;
    if (videoUrl.isEmpty()) {
        error = "地址为空";
    } else if (videoType == VideoType_Rtsp) {
        if (!checkUrl(videoUrl, timeout)) {
            error = "网络地址不可达";
        }
    } else if (videoType == VideoType_FileLocal) {
        if (!QFile(videoUrl).exists()) {
            error = "文件不存在";
        }
    } else if (videoType == VideoType_Other) {

    }

    if (error.isEmpty()) {
        return true;
    } else {
        videoThread->debug("地址有误", "原因: " + error);
        return false;
    }
}

QString VideoHelper::getRightUrl(const VideoType &videoType, const QString &videoUrl)
{
    QString url = videoUrl;
    if (videoType == VideoType_Rtsp) {
        //视频流中的特殊字符比如rtsp中的用户密码部分需要转义
        int index = videoUrl.lastIndexOf("@");
        if (index > 0) {
            QString userName, userPwd;
            UrlHelper::getUserInfo(url, userName, userPwd);
            QString otherInfo = videoUrl.mid(index + 1, videoUrl.length() - index);
            userName = userName.toUtf8().toPercentEncoding();
            userPwd = userPwd.toUtf8().toPercentEncoding();

            //不能用字符串拼接(又会把转义字符转回去)
            //url = QString("rtsp://%1:%2@%3").arg(userName).arg(userPwd).arg(otherInfo);
            url.clear();
            url.append("rtsp://");
            url.append(userName);
            url.append(":");
            url.append(userPwd);
            url.append("@");
            url.append(otherInfo);
        }
    } else if (videoType == VideoType_Http) {
        //视频流中带空格要转换
        if (url.contains(" ")) {
            url = QUrl(url).toEncoded();
        }
    }

    return url;
}

QUrl VideoHelper::getUrl(const VideoType &videoType, const QString &videoUrl)
{
    QUrl url;
    if (videoType == VideoType_FileLocal) {
        url = QUrl::fromLocalFile(videoUrl);
    } else {
        url = QUrl(VideoHelper::getRightUrl(videoType, videoUrl));
    }
    return url;
}

VideoType VideoHelper::getVideoType(const QString &videoUrl)
{
    //http开头的文件地址这里没法判断(需要打开后获取时长判断)
    VideoType videoType;
    if (videoUrl.startsWith("rtsp://")) {
        videoType = VideoType_Rtsp;
    } else if (videoUrl.startsWith("rtmp://")) {
        videoType = VideoType_Rtmp;
    } else if (videoUrl.startsWith("http://") || videoUrl.startsWith("https://")) {
        videoType = VideoType_Http;
    } else if (QFile(videoUrl).exists() && !videoUrl.startsWith("/dev/")) {
        //优先判断是否是本地文件(避免有些文件是desktop等关键字开头导致判断为其他)
        //在linux系统万物皆文件所以设备文件 /dev/video0 这种会认为文件存在
        videoType = VideoType_FileLocal;
    } else if (videoUrl.startsWith("video=") || videoUrl.contains("/dev/") || videoUrl.startsWith("dshow") || videoUrl.startsWith("avdevice:dshow") || videoUrl.startsWith("avdevice://dshow")) {
        videoType = VideoType_Camera;
    } else if (videoUrl.startsWith("desktop") || videoUrl.contains(":0") || videoUrl.startsWith("title=") || videoUrl.startsWith("screen") || videoUrl.startsWith("avdevice:gdigrab") || videoUrl.startsWith("avdevice://gdigrab")) {
        videoType = VideoType_Desktop;
    } else {
        videoType = VideoType_Other;
    }

    return videoType;
}

bool VideoHelper::getOnlyAudio(const QString &videoUrl, const QString &formatName)
{
    //如果还有其他可能的格式自行增加即可
    QStringList suffixs;
    suffixs << "mp3" << "wav" << "aac" << "wma" << "mid" << "ogg";

    bool onlyAudio = false;
    QString suffix = videoUrl.split(".").last();
    onlyAudio = suffixs.contains(suffix);

    //解析出来的文件格式也需要判断下
    if (!formatName.isEmpty()) {
        onlyAudio = suffixs.contains(formatName);
    }

    return onlyAudio;
}

void VideoHelper::getCameraPara(const VideoCore &videoCore, QString &videoUrl, QString &bufferSize, int &frameRate)
{
    //支持不同格式输入(根据不同内核自动设置)
    //video=USB2.0 PC CAMERA|1920x1080|30
    //dshow://:dshow-vdev=USB2.0 PC CAMERA|1920x1080|30

    QString url = videoUrl;
    url.replace("video=", "");
    url.replace(QString("%1://:%1-vdev=").arg(Device_Flag), "");
    url.replace(QString("avdevice://%1:").arg(Device_Flag), "");
    url.replace(QString("avdevice:%1:").arg(Device_Flag), "");
    url.replace(QString("%1://").arg(Device_Flag), "");
    AbstractVideoThread::checkDeviceUrl(url, videoUrl, bufferSize, frameRate);
    url = url.split("|").first();

    //ffmpeg规定windows上前面要加上video=开头
#ifdef Q_OS_WIN
    url = QString("video=%1").arg(url);
#endif

    //不同内核对应地址不一样
    if (videoCore == VideoCore_FFmpeg) {
    } else if (videoCore == VideoCore_Vlc) {
        url = QString("%1://:%1-vdev=%2").arg(Device_Flag).arg(videoUrl);
    } else if (videoCore == VideoCore_Mdk) {
        url = QString("avdevice://%1:%2").arg(Device_Flag).arg(url);
    } else if (videoCore == VideoCore_Qtav) {
        url = QString("avdevice:%1:%2").arg(Device_Flag).arg(url);
    } else if (videoCore == VideoCore_EasyPlayer) {
        url = QString("%1://%2").arg(Device_Flag).arg(url);
    }

    videoUrl = url;
}

//格式要求: url|bufferSize|frameRate|offsetX|offsetY|screenIndex|encodeScale
void VideoHelper::getDesktopPara(const VideoCore &videoCore, QString &videoUrl, QString &bufferSize, int &frameRate, int &offsetX, int &offsetY, QString &encodeScale)
{
    //ffmpeg -f gdigrab -s 800x600 -r 30 -i desktop out.mp4
    //ffmpeg -f x11grab -s 800x600 -r 30 -i :0.0+0,0 out.mp4
    //ffmpeg -f avfoundation -s 800x600 -r 30 -i 0:0 out.mp4

    //常规写法 desktop|800x600|25
    //窗口写法 title=MainWindow|100x200|25

    QString url = videoUrl;
    url.replace("desktop=", "");
    url.replace("screen://", "");
    url.replace(QString("avdevice://%1:").arg(Desktop_Flag), "");
    url.replace(QString("avdevice:%1:").arg(Desktop_Flag), "");
    url.replace(QString("%1://").arg(Desktop_Flag), "");
    AbstractVideoThread::checkDeviceUrl(url, videoUrl, bufferSize, frameRate, offsetX, offsetY, encodeScale);
    url = url.split("|").first();

    //不同内核对应地址不一样
    if (videoCore == VideoCore_FFmpeg) {
    } else if (videoCore == VideoCore_Vlc) {
        url = QString("screen://");
    } else if (videoCore == VideoCore_Mdk) {
        url = QString("avdevice://%1:%2").arg(Desktop_Flag).arg(url);
    } else if (videoCore == VideoCore_Qtav) {
        url = QString("avdevice:%1:%2").arg(Desktop_Flag).arg(url);
    } else if (videoCore == VideoCore_EasyPlayer) {
        url = QString("%1://%2").arg(Desktop_Flag).arg(url);
    }

    videoUrl = url;
}

//格式要求: url|transport|encodeType|videoFormat|encodeVideoFps|encodeVideoRatio|encodeVideoScale
void VideoHelper::getNormalPara(QString &videoUrl, QString &transport, EncodeType &encodeType, VideoFormat &videoFormat, int &encodeVideoFps, float &encodeVideoRatio, QString &encodeVideoScale)
{
    QString url = videoUrl;
    QStringList list = url.split("|");
    videoUrl = list.at(0);
    int count = list.count();

    //通信协议
    if (count >= 2) {
        QString value = list.at(1);
        if (!value.isEmpty()) {
            transport = value;
        }
    }

    //编码策略
    if (count >= 3) {
        encodeType = (EncodeType)list.at(2).toInt();
    }

    //编码视频格式
    if (count >= 4) {
        videoFormat = (VideoFormat)list.at(3).toInt();
    }

    //编码视频帧率
    if (count >= 5) {
        encodeVideoFps = list.at(4).toInt();
    }

    //视频压缩比率
    if (count >= 6) {
        encodeVideoRatio = list.at(5).toFloat();
    }

    //视频缩放比例
    if (count >= 7) {
        QString value = list.at(6);
        if (!value.isEmpty()) {
            encodeVideoScale = value;
        }
    }
}

VideoType VideoHelper::initPara(WidgetPara &widgetPara, VideoPara &videoPara)
{
    //获取视频类型
    QString url = videoPara.videoUrl;
    VideoType videoType = VideoHelper::getVideoType(url);

    //视频地址不能为空
    if (url.isEmpty()) {
        return videoType;
    }

    //标签和图形绘制模式默认值
    widgetPara.osdDrawMode = DrawMode_Cover;
    widgetPara.graphDrawMode = DrawMode_Cover;

    //只有音频则只能绘制模式(需要绘制专辑封面)
    if (VideoHelper::getOnlyAudio(url)) {
        widgetPara.videoMode = VideoMode_Painter;
    }

    //下面的纠正仅仅是目前已经实现的(如果后面有增加对应处理需要调整)
    VideoCore videoCore = videoPara.videoCore;
    if (videoCore == VideoCore_QMedia) {
        //qmedia内核下的GPU模式对应要用rgbwidget窗体
        if (widgetPara.videoMode == VideoMode_Opengl) {
            videoPara.hardware = "rgb";
            widgetPara.osdDrawMode = DrawMode_Source;
            widgetPara.graphDrawMode = DrawMode_Source;
        }

        //Qt4中的多媒体只有句柄模式(Qt5.6以下绘制模式用QAbstractVideoSurface获取不到图片也只能用句柄模式)
#if (QT_VERSION < QT_VERSION_CHECK(5,6,0))
        widgetPara.videoMode = VideoMode_Hwnd;
#endif
    } else if (videoCore == VideoCore_FFmpeg) {
        //ffmpeg内核没有句柄模式
        if (widgetPara.videoMode == VideoMode_Hwnd) {
            widgetPara.videoMode = VideoMode_Opengl;
        }

        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_Vlc) {
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;

        //vlc内核下的GPU模式对应要用rgbwidget窗体
        if (widgetPara.videoMode == VideoMode_Opengl) {
            videoPara.hardware = "rgb";
        }

        //非句柄模式下非本地文件必须指定分辨率
        if (videoPara.bufferSize == "0x0" && videoType != VideoType_FileLocal) {
            videoPara.bufferSize == "640x480";
        }
    } else if (videoCore == VideoCore_Mpv) {
        //mpv内核只有句柄模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_Mdk || videoCore == VideoCore_Qtav) {
        //内核只有GPU模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    } else if (videoCore == VideoCore_HaiKang || videoCore == VideoCore_DaHua) {
        //目前不支持硬件加速
        videoPara.hardware = "none";
    } else if (videoCore == VideoCore_EasyPlayer) {
        //easyplayer内核只有句柄模式
        widgetPara.videoMode = VideoMode_Hwnd;
        //设置标签和图形绘制模式
        widgetPara.osdDrawMode = DrawMode_Source;
        widgetPara.graphDrawMode = DrawMode_Source;
    }

    //纠正缓存分辨率
    if (!videoPara.bufferSize.contains("x")) {
        videoPara.bufferSize = "0x0";
    }

    //纠正缓存大小和其他
    if (videoCore == VideoCore_FFmpeg || videoCore == VideoCore_Mdk || videoCore == VideoCore_Qtav) {
        if ((videoPara.caching < 1024000) || (videoPara.caching > 102400000)) {
            videoPara.caching = 8192000;
        }
    } else if (videoCore == VideoCore_Vlc) {
        if ((videoPara.caching < 200) || (videoPara.caching > 8000)) {
            videoPara.caching = 500;
        }
    }

    //如果地址带了摄像头参数或者桌面参数则需要取出对应的参数
    if (videoType == VideoType_Camera) {
        VideoHelper::getCameraPara(videoPara.videoCore, videoPara.videoUrl, videoPara.bufferSize, videoPara.frameRate);
        //qDebug() << TIMEMS << QString("纠正地址 -> 地址: %1 -> %2").arg(url).arg(videoPara.videoUrl);
    } else if (videoType == VideoType_Desktop) {
        VideoHelper::getDesktopPara(videoPara.videoCore, videoPara.videoUrl, videoPara.bufferSize, videoPara.frameRate, videoPara.offsetX, videoPara.offsetY, videoPara.encodeVideoScale);
    } else {
        VideoHelper::getNormalPara(videoPara.videoUrl, videoPara.transport, videoPara.encodeType, videoPara.videoFormat, videoPara.encodeVideoFps, videoPara.encodeVideoRatio, videoPara.encodeVideoScale);
    }

    //本地文件以及本地摄像头不需要连接超时时间
    if (videoType == VideoType_FileLocal || videoType == VideoType_Camera) {
        videoPara.connectTimeout = 0;
    }

    //打开休息时间不宜过短(建议最低1秒)
    if (videoPara.openSleepTime < 1000) {
        videoPara.openSleepTime = 1000;
    }

    //读取超时时间不宜过短(建议最低5秒)
    if (videoPara.readTimeout > 0 && videoPara.readTimeout < 5000) {
        videoPara.readTimeout = 5000;
    }

    //连接超时时间=0表示对网络流不做提前连接测试处理
    if (videoPara.connectTimeout > 0) {
        //连接超时时间过长则自动调整打开休息时间(至少要2秒)
        if (videoPara.openSleepTime > 0 && (videoPara.openSleepTime - videoPara.connectTimeout) < 2000) {
            videoPara.openSleepTime = videoPara.connectTimeout + 2000;
        }

        //连接超时时间过长则自动调整读取超时时间(至少要3轮)
        if (videoPara.readTimeout > 0 && (videoPara.readTimeout / videoPara.connectTimeout) < 3) {
            videoPara.readTimeout = videoPara.connectTimeout * 3;
        }
    }

#ifndef openglx
    //如果没有opengl则强制改成绘制模式
    if (widgetPara.videoMode == VideoMode_Opengl) {
        widgetPara.videoMode = VideoMode_Painter;
    }
#else
    //OpenGLES模式下不能用硬解码
    if (QCoreApplication::testAttribute(Qt::AA_UseOpenGLES)) {
        videoPara.hardware = "none";
    }
#endif

    //句柄模式不能共享解码线程
    if (widgetPara.videoMode == VideoMode_Hwnd) {
        widgetPara.sharedData = false;
    }

    //绘制模式下标签和图形绘制模式强制改成图片
    if (widgetPara.videoMode == VideoMode_Painter) {
        widgetPara.osdDrawMode = DrawMode_Image;
        widgetPara.graphDrawMode = DrawMode_Image;
    }

    return videoType;
}

VideoThread *VideoHelper::newVideoThread(QWidget *parent, const VideoCore &videoCore, const VideoMode &videoMode)
{
    return VideoUtil::newVideoThread(parent, videoCore, videoMode);
}

void VideoHelper::initVideoThread(VideoThread *videoThread, const VideoPara &videoPara)
{
    //设置一堆参数
    videoThread->setVideoCore(videoPara.videoCore);
    videoThread->setVideoUrl(videoPara.videoUrl);
    videoThread->setBufferSize(videoPara.bufferSize);
    videoThread->setFrameRate(videoPara.frameRate);
    videoThread->setProperty("offsetX", videoPara.offsetX);
    videoThread->setProperty("offsetY", videoPara.offsetY);

    videoThread->setDecodeType(videoPara.decodeType);
    videoThread->setHardware(videoPara.hardware);
    videoThread->setTransport(videoPara.transport);
    videoThread->setCaching(videoPara.caching);

    videoThread->setAudioLevel(videoPara.audioLevel);
    videoThread->setDecodeAudio(videoPara.decodeAudio);
    videoThread->setPlayAudio(videoPara.playAudio);
    videoThread->setPlayRepeat(videoPara.playRepeat);
    videoThread->setPlayStep(videoPara.playStep);
    videoThread->setCountKbps(videoPara.countKbps);
    videoThread->setSecretKey(videoPara.secretKey);
    videoThread->setAudioDevice(videoPara.audioDevice);

    videoThread->setEncodeType(videoPara.encodeType);
    videoThread->setVideoFormat(videoPara.videoFormat);
    videoThread->setEncodeVideoFps(videoPara.encodeVideoFps);
    videoThread->setEncodeVideoRatio(videoPara.encodeVideoRatio);
    videoThread->setEncodeVideoScale(videoPara.encodeVideoScale);

    videoThread->setOpenSleepTime(videoPara.openSleepTime);
    videoThread->setReadTimeout(videoPara.readTimeout);
    videoThread->setConnectTimeout(videoPara.connectTimeout);
}

void VideoHelper::initVideoPara(VideoThread *videoThread, const QString &videoUrl)
{
    QString encodeVideoScale = "1";
    VideoHelper::initVideoPara(videoThread, videoUrl, encodeVideoScale);
}

void VideoHelper::initVideoPara(VideoThread *videoThread, const QString &videoUrl, QString &encodeVideoScale)
{
    //如果是本地设备或者桌面录屏要取出其他参数
    VideoType videoType = VideoHelper::getVideoType(videoUrl);
    if (videoType == VideoType_Camera || videoType == VideoType_Desktop) {
        QString deviceName = videoUrl;
        QString resolution = "0x0";
        int frameRate = 0;
        int offsetX = 0;
        int offsetY = 0;
        if (videoType == VideoType_Camera) {
            VideoHelper::getCameraPara(VideoCore_FFmpeg, deviceName, resolution, frameRate);
        } else if (videoType == VideoType_Desktop) {
            VideoHelper::getDesktopPara(VideoCore_FFmpeg, deviceName, resolution, frameRate, offsetX, offsetY, encodeVideoScale);
        }

        videoThread->setVideoUrl(deviceName);
        videoThread->setBufferSize(resolution);
        videoThread->setFrameRate(frameRate);
        videoThread->setProperty("offsetX", offsetX);
        videoThread->setProperty("offsetY", offsetY);
    } else {
        QString url = videoUrl;
        QString transport = "none";
        EncodeType encodeType = EncodeType_Auto;
        VideoFormat videoFormat = VideoFormat_Auto;
        int encodeVideoFps = 0;
        float encodeVideoRatio = 1;
        VideoHelper::getNormalPara(url, transport, encodeType, videoFormat, encodeVideoFps, encodeVideoRatio, encodeVideoScale);

        videoThread->setVideoUrl(url);
        videoThread->setTransport(transport);
        videoThread->setEncodeType(encodeType);
        videoThread->setVideoFormat(videoFormat);
        videoThread->setEncodeVideoFps(encodeVideoFps);
        videoThread->setEncodeVideoRatio(encodeVideoRatio);
        videoThread->setEncodeVideoScale(encodeVideoScale);
    }
}
