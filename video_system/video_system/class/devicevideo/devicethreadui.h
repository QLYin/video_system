#ifndef DEVICETHREADUI_H
#define DEVICETHREADUI_H

#include <QThread>
#include <QImage>

//演示线程中如何与UI交互
class DeviceThreadUI : public QThread
{
    Q_OBJECT
public:
    explicit DeviceThreadUI(QObject *parent = 0);

protected:
    void run();

private:
    //标志位用于停止线程
    volatile bool stopped;

private slots:
    void addMsg(const QString &msg, quint8 type = 0);
    void addMsgList(const QString &msg, const QString &result, const QImage &image);
};

#endif // DEVICETHREADUI_H
