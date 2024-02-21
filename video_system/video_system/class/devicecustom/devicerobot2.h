#ifndef DEVICEROBOT2_H
#define DEVICEROBOT2_H

#include <QObject>
#include <QMutex>
#include "singleton.h"
class QextSerialPort;

class DeviceRobot2 : public QObject
{
    Q_OBJECT SINGLETON_DECL(DeviceRobot2)

public:
    explicit DeviceRobot2(QObject *parent = 0);

private:
    bool isOkA;             //串口A是否正常
    bool isOkB;             //串口B是否正常
    bool isOkD;             //串口D是否正常

    QextSerialPort *comA;   //串口A通信对象
    QextSerialPort *comB;   //串口B通信对象
    QextSerialPort *comD;   //串口D通信对象

private slots:
    void readDataA();       //读取串口A数据
    void readDataB();       //读取串口B数据

signals:
    //发送接收数据信号 发出去界面上显示方便调试用
    void sendData(const QString &portName, const QByteArray &data);
    void receiveData(const QString &portName, const QByteArray &data);

public slots:
    //打开串口
    void openA();
    void openB();
    void openD();

    //关闭串口
    void closeA();
    void closeB();
    void closeD();

    //发送数据给串口D
    void sendDataToD(const QByteArray &data);
};

#endif // DEVICEROBOT2_H
