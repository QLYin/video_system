#include "audiohelper.h"
#include "qmath.h"
#include "qendian.h"
#include "qdebug.h"

#ifdef multimedia
void AudioHelper::getAudioDevice(QStringList &names, QStringList &ids, bool input)
{
    names.clear();
    ids.clear();

#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QList<QAudioDevice> devices = input ? QMediaDevices::audioInputs() : QMediaDevices::audioOutputs();
    foreach (QAudioDevice device, devices) {
        names << device.description();
        ids << device.id();
    }
#else
    QList<QAudioDevice> devices = QAudioDeviceInfo::availableDevices(input ? QAudio::AudioInput : QAudio::AudioOutput);
    foreach (QAudioDevice device, devices) {
        names << device.deviceName();
        ids << device.deviceName();
    }
#endif
    //qDebug() << names << ids;
}

QAudioDevice AudioHelper::getAudioDevice(const QString &name, bool input)
{
    QAudioDevice audioDevice;
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QList<QAudioDevice> devices = input ? QMediaDevices::audioInputs() : QMediaDevices::audioOutputs();
    foreach (QAudioDevice device, devices) {
        if (device.description() == name) {
            audioDevice = device;
            break;
        }
    }
#else
    QList<QAudioDevice> devices = QAudioDeviceInfo::availableDevices(input ? QAudio::AudioInput : QAudio::AudioOutput);
    foreach (QAudioDevice device, devices) {
        if (device.deviceName() == name) {
            audioDevice = device;
            break;
        }
    }
#endif
    return audioDevice;
}

void AudioHelper::initAudioFormat(QAudioFormat &format, int sampleRate, int channelCount, int sampleSize)
{
    //qDebug() << sampleRate << channelCount << sampleSize;
    format.setSampleRate(sampleRate);
    format.setChannelCount(channelCount);
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    if (sampleSize == 8) {
        format.setSampleFormat(QAudioFormat::UInt8);
    } else if (sampleSize == 16) {
        format.setSampleFormat(QAudioFormat::Int16);
    } else if (sampleSize == 32) {
        format.setSampleFormat(QAudioFormat::Int32);
    }
#else
    format.setSampleSize(sampleSize);
    format.setCodec("audio/pcm");
    format.setSampleType(sampleSize == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
#endif
}

int AudioHelper::getSampleSize(QAudioFormat format)
{
    int sampleSize = 16;
#if (QT_VERSION >= QT_VERSION_CHECK(6,2,0))
    QAudioFormat::SampleFormat sampleFormat = format.sampleFormat();
    if (sampleFormat == QAudioFormat::UInt8) {
        sampleSize = 8;
    } else if (sampleFormat == QAudioFormat::Int16) {
        sampleSize = 16;
    } else if (sampleFormat == QAudioFormat::Int32) {
        sampleSize = 32;
    } else if (sampleFormat == QAudioFormat::Float) {
        sampleSize = 32;
    }
#else
    sampleSize = format.sampleSize();
#endif
    return sampleSize;
}

quint32 AudioHelper::getMaxAmplitude(QAudioFormat format)
{
    quint32 maxAmplitude = 0;
    int sampleSize = getSampleSize(format);
    if (sampleSize == 8) {
        maxAmplitude = 0xff;
    } else if (sampleSize == 16) {
        maxAmplitude = 0x7fff;
    } else if (sampleSize == 32) {
        maxAmplitude = 0x7fffffff;
    } else {
        maxAmplitude = 0x7fffffff;
    }

    return maxAmplitude;
}

bool AudioHelper::getAudioLevel(QAudioFormat format, const char *data, qint64 len, qreal &leftLevel, qreal &rightLevel)
{
    leftLevel = rightLevel = 0;
    quint32 maxAmplitude = getMaxAmplitude(format);
    if (!maxAmplitude) {
        return false;
    }

    quint32 leftValue = 0;
    quint32 rightValue = 0;
    int sampleSize = getSampleSize(format);
    int channelCount = format.channelCount();
    int channelBytes = sampleSize / 8;
    int sampleBytes = channelCount * channelBytes;
    int numSamples = len / sampleBytes;
    const uchar *ptr = reinterpret_cast<const uchar *>(data);
    //qDebug() << sampleSize << channelCount << numSamples;

    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < channelCount; ++j) {
            quint32 value = 0;
            if (sampleSize == 8) {
                value = *reinterpret_cast<const quint8 *>(ptr);
            } else if (sampleSize == 16) {
                value = qAbs(qFromLittleEndian<qint16>(ptr));
            } else if (sampleSize == 32) {
                value = qAbs(qFromLittleEndian<qint32>(ptr));
            }

            //分左右通道
            if (j == 0) {
                leftValue = qMax(value, leftValue);
            } else {
                rightValue = qMax(value, rightValue);
            }

            ptr += channelBytes;
        }
    }

    leftValue = qMin(leftValue, maxAmplitude);
    rightValue = qMin(rightValue, maxAmplitude);
    leftLevel = qreal(leftValue) / maxAmplitude;
    rightLevel = qreal(rightValue) / maxAmplitude;
    if (sampleSize == 8) {
        leftLevel -= 0.5;
        rightLevel -= 0.5;
    }

    //单通道则右通道的值强制和左通道相等
    if (channelCount == 1) {
        rightLevel = leftLevel;
    }

    //qDebug() << TIMEMS << leftValue << leftLevel << rightValue << rightLevel;
    return true;
}

qreal AudioHelper::getAudioDecibel(const char *data, qint64 len)
{
    int decibel = 0;
    double sum = 0;
    int value = 0;
    for (int i = 0; i < len; i += 2) {
        memcpy(&value, data + i, 2);
        sum += abs(value);
    }

    sum = sum / (len / 2);
    if (sum > 0) {
        decibel = (int)(20.0 * log10(sum));
    }

    //qDebug() << TIMEMS << decibel;
    return decibel;
}
#endif
