#include "devicerobot2.h"
#include "qthelper.h"
#include "devicerobot.h"
#ifndef nocom
#include "qextserialport.h"
#endif

SINGLETON_IMPL(DeviceRobot2)
DeviceRobot2::DeviceRobot2(QObject *parent) : QObject(parent)
{
    isOkA = false;
    isOkB = false;
    isOkD = false;

    //初始化串口类
#ifndef nocom
    comA = new QextSerialPort(QextSerialPort::Polling, this);
    comB = new QextSerialPort(QextSerialPort::Polling, this);
    comD = new QextSerialPort(QextSerialPort::Polling, this);
#endif

    //启动定时器读取数据
    QTimer *timerReadA = new QTimer(this);
    connect(timerReadA, SIGNAL(timeout()), this, SLOT(readDataA()));
    timerReadA->start(100);

    QTimer *timerReadB = new QTimer(this);
    connect(timerReadB, SIGNAL(timeout()), this, SLOT(readDataB()));
    timerReadB->start(100);
}

void DeviceRobot2::readDataA()
{
#ifndef nocom
    //串口通信失败或者没有数据可处理则返回
    if (!isOkD || !isOkA || comA->bytesAvailable() <= 0) {
        return;
    }

    //收到的数据转给串口D
    QByteArray data = comA->readAll();
    emit receiveData(AppConfig::PortNameA, data);
    comD->write(data);
    emit sendData(AppConfig::PortNameD, data);
#endif
}

void DeviceRobot2::readDataB()
{
#ifndef nocom
    //串口通信失败或者没有数据可处理则返回
    if (!isOkD || !isOkB || comB->bytesAvailable() <= 0) {
        return;
    }

    //收到的数据转给串口D
    QByteArray data = comB->readAll();
    emit receiveData(AppConfig::PortNameB, data);
    comD->write(data);
    emit sendData(AppConfig::PortNameD, data);
#endif
}

void DeviceRobot2::openA()
{
    DeviceRobot::openCom(comA, AppConfig::PortNameA, AppConfig::BaudRateA, isOkA);
}

void DeviceRobot2::openB()
{
    DeviceRobot::openCom(comB, AppConfig::PortNameB, AppConfig::BaudRateB, isOkB);
}

void DeviceRobot2::openD()
{
    DeviceRobot::openCom(comD, AppConfig::PortNameD, AppConfig::BaudRateD, isOkD);
}

void DeviceRobot2::closeA()
{
    DeviceRobot::closeCom(comA, isOkA);
}

void DeviceRobot2::closeB()
{
    DeviceRobot::closeCom(comB, isOkB);
}

void DeviceRobot2::closeD()
{
    DeviceRobot::closeCom(comD, isOkD);
}

void DeviceRobot2::sendDataToD(const QByteArray &data)
{
#ifndef nocom
    if (isOkD) {
        comD->write(data);
        emit sendData(AppConfig::PortNameD, data);
    }
#endif
}

