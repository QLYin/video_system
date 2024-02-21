#ifndef AUDIORECORDER_H
#define AUDIORECORDER_H

#include "audioinclude.h"

class AudioRecorder : public QObject
{
    Q_OBJECT
public:
    //录制状态
    enum RecorderState {
        RecorderState_Recording = 0,//录制中
        RecorderState_Paused = 1,   //暂停中
        RecorderState_Stopped = 2   //已结束
    };

    explicit AudioRecorder(QObject *parent = 0);

private:
#ifdef multimedia5
    //正在录制标志位
    bool isRecord;
    //音频输入对象
    QAudioInput *audioInput;
    //音频输入数据
    QIODevice *deviceInput;

    //音频录制对象
    QAudioRecorder *audioRecorder;
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    //多媒体会话
    QMediaCaptureSession mediaSession;
#else
    //音频探测对象
    QAudioProbe *audioProbe;
#endif
#endif

private slots:
#ifdef multimedia5
    //数据探测
    void audioBufferProbed(const QAudioBuffer &audioBuffer);
    //录制事件
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    void updateRecorderState(QMediaRecorder::RecorderState state);
#else
    void updateRecorderState(QMediaRecorder::State state);
#endif
#endif

public slots:
    //初始化参数
    void initPara(const QString &deviceName, int sampleRate, int bitRate, int channelCount, int quality, int encodingMode);

    //开始录制
    void recordStart(const QString &fileName);
    //暂停录制
    void recordPause();
    //停止录制
    void recordStop();

signals:
    //音频数据振幅
    void receiveLevel(qreal leftLevel, qreal rightLevel);
    //打印数据消息
    void receiveData(int sampleRate, int sampleSize, int byteCount);

    //录制进度
    void receiveDuration(qint64 duration);
    //录制状态变化
    void recorderStateChanged(const AudioRecorder::RecorderState &state, const QString &file);
};

#endif // AUDIORECORDER_H
