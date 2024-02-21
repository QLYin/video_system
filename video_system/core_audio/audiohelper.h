#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#include "audioinclude.h"

class AudioHelper
{
public:
#ifdef multimedia
    //获取音频输入输出设备
    static void getAudioDevice(QStringList &names, QStringList &ids, bool input);
    //根据设备名称查找设备对象
    static QAudioDevice getAudioDevice(const QString &name, bool input);

    //初始化音频格式
    static void initAudioFormat(QAudioFormat &format, int sampleRate, int channelCount, int sampleSize);

    //获取采样大小
    static int getSampleSize(QAudioFormat format);
    //获取对应音频格式最大振幅
    static quint32 getMaxAmplitude(QAudioFormat format);
    //获取对应音频格式音频数据的振幅级别
    static bool getAudioLevel(QAudioFormat format, const char *data, qint64 len, qreal &leftLevel, qreal &rightLevel);
    //获取音频数据分贝大小(16位)
    static qreal getAudioDecibel(const char *data, qint64 len);
#endif
};

#endif // AUDIOHELPER_H
