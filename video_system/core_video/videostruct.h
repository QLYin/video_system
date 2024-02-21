#ifndef VIDEOSTRUCT_H
#define VIDEOSTRUCT_H

#include <QString>
#include <QList>
#include <QDebug>

//解析内核
enum VideoCore {
    VideoCore_None = 0,         //不解析处理(默认值)

    VideoCore_QMedia = 1,       //采用qmedia解析(qt自带且依赖本地解码器且部分平台支持)
    VideoCore_FFmpeg = 2,       //采用ffmpeg解析(通用性最好)
    VideoCore_Vlc = 3,          //采用vlc解析(支持本地文件最好)
    VideoCore_Mpv = 4,          //采用mpv解析(支持本地文件最好且跨平台最多)
    VideoCore_Mdk = 5,          //采用mdk解析(框架精简跨平台好)
    VideoCore_Qtav = 6,         //采用qtav解析(框架结构最好/基于ffmpeg)

    VideoCore_HaiKang = 10,     //采用海康sdk解析
    VideoCore_DaHua = 11,       //采用大华sdk解析
    VideoCore_YuShi = 12,       //采用宇视sdk解析

    VideoCore_EasyPlayer = 20   //采用easyplayer解析
};

//视频类型
enum VideoType {
    VideoType_FileLocal = 1,    //本地文件
    VideoType_FileWeb = 2,      //网络文件(可以是http/rtsp等)
    VideoType_Camera = 3,       //本地摄像头
    VideoType_Desktop = 4,      //本地桌面
    VideoType_Rtsp = 10,        //视频流rtsp
    VideoType_Rtmp = 11,        //视频流rtmp
    VideoType_Http = 12,        //视频流http
    VideoType_Other = 255       //其他未知
};

//解码处理策略
enum DecodeType {
    DecodeType_Fast = 0,        //速度优先
    DecodeType_Full = 1,        //质量优先
    DecodeType_Even = 2,        //均衡处理
    DecodeType_Fastest = 3      //最快速度(速度优先且不做任何音视频同步)
};

//编码处理策略
enum EncodeType {
    EncodeType_Auto = 0,        //自动识别处理(非264/265/aac则转码)
    EncodeType_File = 1,        //文件强制转码(可以重新定义各种参数)
    EncodeType_All = 2          //所有全部转码(会占用更大CPU)
};

//视频编码格式
enum VideoFormat {
    VideoFormat_Auto = 0,       //自动处理(源头是264就264/265就265)
    VideoFormat_H264 = 1,       //强制用264编码
    VideoFormat_H265 = 2        //强制用265编码
};

//视频采集参数
struct VideoPara {
    VideoCore videoCore;        //解析内核
    QString videoUrl;           //视频地址
    QString bufferSize;         //缓存分辨率
    int frameRate;              //视频帧率
    int offsetX;                //桌面坐标X
    int offsetY;                //桌面坐标Y

    DecodeType decodeType;      //解码处理策略
    QString hardware;           //硬件加速名称
    QString transport;          //通信协议(tcp/udp)
    int caching;                //缓存时间(不同内核不同单位/vlc是ms/ffmpeg是mb)

    bool audioLevel;            //开启音频振幅
    bool decodeAudio;           //解码音频数据
    bool playAudio;             //解码播放声音
    bool playRepeat;            //重复循环播放
    bool playStep;              //是否逐帧播放
    bool countKbps;             //统计实时码率
    QString secretKey;          //加密解密秘钥
    QString audioDevice;        //音频输出设备

    EncodeType encodeType;      //编码处理策略
    VideoFormat videoFormat;    //视频编码格式(自动处理/转264/转265)
    int encodeVideoFps;         //指定编码帧率(不指定则取源头帧率)
    float encodeVideoRatio;     //视频压缩比率(控制清晰度/默认1采用原比特/范围值0.1-1.0/值越小体积越小)
    QString encodeVideoScale;   //视频缩放尺寸(控制分辨率/默认1表示不缩放/可以是640x480表示指定分辨率)

    int openSleepTime;          //打开休息时间(最低1000/单位毫秒)
    int readTimeout;            //采集超时时间(0=不处理/单位毫秒)
    int connectTimeout;         //连接超时时间(0=不处理/单位毫秒)

    VideoPara() {
        videoCore = VideoCore_None;
        videoUrl = "";
        bufferSize = "0x0";
        frameRate = 0;
        offsetX = 0;
        offsetY = 0;

        decodeType = DecodeType_Fast;
        hardware = "none";
        transport = "auto";
        caching = 0;

        audioLevel = false;
        decodeAudio = true;
        playAudio = true;
        playRepeat = false;
        playStep = false;
        countKbps = false;
        secretKey = "";
        audioDevice = "";

        encodeType = EncodeType_Auto;
        videoFormat = VideoFormat_Auto;
        encodeVideoFps = 0;
        encodeVideoRatio = 1;
        encodeVideoScale = "1";

        openSleepTime = 3000;
        readTimeout = 0;
        connectTimeout = 500;
    }

    void reset() {
        videoUrl = "";
        bufferSize = "0x0";
        frameRate = 0;
        offsetX = 0;
        offsetY = 0;

        encodeType = EncodeType_Auto;
        encodeVideoFps = 0;
        encodeVideoRatio = 1;
        encodeVideoScale = "1";
    }
};

#endif // VIDEOSTRUCT_H
