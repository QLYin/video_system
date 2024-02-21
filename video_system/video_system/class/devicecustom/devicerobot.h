#ifndef DEVICEROBOT_H
#define DEVICEROBOT_H

#include <QObject>
#include <QMutex>
#include "singleton.h"
class QextSerialPort;

class DeviceRobot : public QObject
{
    Q_OBJECT SINGLETON_DECL(DeviceRobot)

public:
    //通用打开和关闭串口函数
    static void openCom(QextSerialPort *com, const QString &portName, int baudRate, bool &isOk);
    static void closeCom(QextSerialPort *com, bool &isOk);
    explicit DeviceRobot(QObject *parent = 0);

private:
    QMutex mutex;           //数据锁
    bool isOk;              //串口是否正常
    QextSerialPort *com;    //串口通信对象
    QByteArray buffer;      //数据队列

private slots:
    void readData();        //读取串口数据
    void checkData();       //处理串口数据

signals:
    //发送接收数据信号 发出去界面上显示方便调试用
    void sendData(const QString &portName, const QByteArray &data);
    void receiveData(const QString &portName, const QByteArray &data);

    //通用信号,用type来区分
    void receiveData(quint8 type, const QVariant &data);

public slots:
    //打开和关闭串口
    void open();
    void close();

    //模拟数据
    void testData(const QString &data);
};

#endif // DEVICEROBOT_H
