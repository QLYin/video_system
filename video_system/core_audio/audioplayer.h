#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "audioinclude.h"

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
#ifdef Q_OS_LINUX
    explicit AudioPlayer(bool audioThread = false, QObject *parent = 0);
#else
    explicit AudioPlayer(bool audioThread = true, QObject *parent = 0);
#endif
    ~AudioPlayer();

private:
    //音量大小
    int volume;
    //静音状态
    bool muted;
    //音频振幅
    bool audioLevel;
    //线程运行
    bool audioThread;

    //音频输入对象
    QAudioInputx *audioInput;
    //音频输入数据
    QIODevice *deviceInput;

    //音频输出对象
    QAudioOutputx *audioOutput;
    //音频输出数据
    QIODevice *deviceOutput;

public:
    //获取音频设备
    QAudioInputx *getAudioInput();
    QAudioOutputx *getAudioOutput();

    //获取是否已经运行
    bool getIsOk() const;

    //获取和设置音量
    int getVolume() const;    
    void setVolume(int volume);

    //获取和设置静音
    bool getMuted() const;
    void setMuted(bool muted);

    //获取和设置音频振幅
    bool getAudioLevel() const;
    void setAudioLevel(bool audioLevel);

private slots:
    //读取音频输入设备数据
    void readyRead();

public slots:
    //打开和关闭音频输入
    void openAudioInput(int sampleRate, int channelCount, int sampleSize);
    void openAudioInput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize);
    void closeAudioInput();

    //打开和关闭音频输出
    void openAudioOutput(int sampleRate, int channelCount, int sampleSize);
    void openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize);
    void openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize, int volume);
    void closeAudioOutput();

    //播放音频数据
    void playAudioData(const char *data, qint64 len);

signals:
    //音频输入数据
    void receiveInputData(const QByteArray &data);
    //音频输出数据
    void receiveOutputData(const char *data, qint64 len);

    //音量大小
    void receiveVolume(int volume);
    //静音状态
    void receiveMuted(bool muted);
    //音频数据振幅
    void receiveLevel(qreal leftLevel, qreal rightLevel);
};

#endif // AUDIOPLAYER_H
