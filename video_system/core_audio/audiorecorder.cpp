#include "audiorecorder.h"
#include "audiohelper.h"
#include "qurl.h"

AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent)
{
#ifdef multimedia5
    isRecord = false;
    //实例化音频录制对象
    audioRecorder = new QAudioRecorder(this);
    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this, SIGNAL(receiveDuration(qint64)));

#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    //本类已经有同名信号需要换种方式
    connect(audioRecorder, &QMediaRecorder::recorderStateChanged, this, &AudioRecorder::updateRecorderState);
    mediaSession.setAudioInput(new QAudioInput(this));
    mediaSession.setRecorder(audioRecorder);
    //目前Qt6暂时不提供音频探测类
#else
    //实例化音频探测对象
    audioProbe = new QAudioProbe;
    connect(audioProbe, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(audioBufferProbed(QAudioBuffer)));
    audioProbe->setSource(audioRecorder);
    connect(audioRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)));
#endif
#endif
    emit recorderStateChanged(RecorderState_Stopped, "");
}

#ifdef multimedia5
void AudioRecorder::audioBufferProbed(const QAudioBuffer &audioBuffer)
{
    QAudioFormat format = audioBuffer.format();
    const char *data = audioBuffer.constData<char>();
    int len = audioBuffer.byteCount();
    qreal leftLevel, rightLevel;
    AudioHelper::getAudioLevel(format, data, len, leftLevel, rightLevel);
    emit receiveLevel(leftLevel, rightLevel);
    emit receiveData(format.sampleRate(), AudioHelper::getSampleSize(format), len);
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
void AudioRecorder::updateRecorderState(QMediaRecorder::RecorderState state)
#else
void AudioRecorder::updateRecorderState(QMediaRecorder::State state)
#endif
{
    QString file = audioRecorder->actualLocation().toString();
    switch (state) {
        case QMediaRecorder::RecordingState:
            emit recorderStateChanged(RecorderState_Recording, file);
            break;
        case QMediaRecorder::PausedState:
            emit recorderStateChanged(RecorderState_Paused, file);
            break;
        case QMediaRecorder::StoppedState:
            emit recorderStateChanged(RecorderState_Stopped, file);
            break;
    }
}
#endif

void AudioRecorder::initPara(const QString &deviceName, int sampleRate, int bitRate, int channelCount, int quality, int encodingMode)
{
#ifdef multimedia5
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QMediaFormat format;
    format.setFileFormat(QMediaFormat::MP3);
    format.setAudioCodec(QMediaFormat::AudioCodec::MP3);
    audioRecorder->setMediaFormat(format);

    audioRecorder->setAudioSampleRate(sampleRate);
    audioRecorder->setAudioBitRate(bitRate);
    audioRecorder->setAudioChannelCount(channelCount);
    audioRecorder->setQuality(QMediaRecorder::Quality(quality));
    audioRecorder->setEncodingMode(encodingMode == 0 ? QMediaRecorder::ConstantQualityEncoding : QMediaRecorder::ConstantBitRateEncoding);

    //设置音频输入设备
    mediaSession.audioInput()->setDevice(AudioHelper::getAudioDevice(deviceName, true));
#else
    //音频编码配置
    QAudioEncoderSettings settings;
    settings.setCodec("audio/pcm");
    settings.setSampleRate(sampleRate);
    settings.setBitRate(bitRate);
    settings.setChannelCount(channelCount);
    settings.setQuality(QMultimedia::EncodingQuality(quality));
    settings.setEncodingMode(encodingMode == 0 ? QMultimedia::ConstantQualityEncoding : QMultimedia::ConstantBitRateEncoding);
    audioRecorder->setAudioSettings(settings);
    audioRecorder->setContainerFormat("audio/x-wav");

    //设置音频输入设备
    audioRecorder->setAudioInput(deviceName);
#endif
#endif
}

void AudioRecorder::recordStart(const QString &fileName)
{
#ifdef multimedia5
    //目录不存在则新建
    QString path = QFileInfo(fileName).path();
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }

    if (!isRecord) {
        isRecord = true;
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    }

    audioRecorder->record();
#endif
}

void AudioRecorder::recordPause()
{
#ifdef multimedia5
    if (isRecord) {
        audioRecorder->pause();
    }
#endif
}

void AudioRecorder::recordStop()
{
#ifdef multimedia5
    if (isRecord) {
        isRecord = false;
        audioRecorder->stop();
    }
#endif
}
