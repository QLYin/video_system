#include "audioplayer.h"
#include "qmetatype.h"
#include "qthread.h"

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    //注册数据类型
    qRegisterMetaType<const char *>("const char *");    
}

bool AudioPlayer::getIsOk() const
{    
    return false;
}

int AudioPlayer::getVolume() const
{    
    return 100;
}

void AudioPlayer::setVolume(int volume)
{

}

bool AudioPlayer::getMuted() const
{
    return false;
}

void AudioPlayer::setMuted(bool muted)
{

}

bool AudioPlayer::getAudioLevel() const
{
    return false;
}

void AudioPlayer::setAudioLevel(bool audioLevel)
{

}

void AudioPlayer::readyRead()
{

}

void AudioPlayer::openAudioInput(int sampleRate, int channelCount, int sampleSize)
{

}

void AudioPlayer::openAudioInput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize)
{

}

void AudioPlayer::closeAudioInput()
{

}

void AudioPlayer::openAudioOutput(int sampleRate, int channelCount, int sampleSize)
{

}

void AudioPlayer::openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize)
{

}

void AudioPlayer::openAudioOutput(const QString &deviceName, int sampleRate, int channelCount, int sampleSize, int volume)
{

}

void AudioPlayer::closeAudioOutput()
{

}

void AudioPlayer::playAudioData(const char *data, qint64 len)
{

}
