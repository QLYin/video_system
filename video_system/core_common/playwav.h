#ifndef PLAYWAV_H
#define PLAYWAV_H

#include <QTimer>
#ifdef __arm__
#include <QProcess>
#else
#ifdef playsound
#include <QtMultimedia>
#else
#include <QtGui>
#endif
#endif

class QSound;
class QSoundEffect;

class PlayWav : public QObject
{
    Q_OBJECT
public:
    static PlayWav *playWav;
    static void play(const QString &fileName, int playCount = 1);
    static void stop();
    explicit PlayWav(QObject *parent = 0);

private:
    QString fileName;       //播放文件
    int playCount;          //播放次数
    int isPlayCount;        //已播放的次数
    QTimer *timer;          //定时器播放

#ifdef __arm__
    QProcess *process;      //执行播放命令
#else
#ifdef playsound
    QSoundEffect *player;   //播放对象
#else
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QSound *player;         //播放对象
#endif
#endif
#endif

private slots:
    void play();

public slots:
    void play1(const QString &fileName, int playCount = 1);
    void stop1();
};

#endif // PLAYWAV_H
