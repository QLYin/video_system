#include "devicerobot.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "dbquery.h"
#ifndef nocom
#include "qextserialport.h"
#endif

void DeviceRobot::openCom(QextSerialPort *com, const QString &portName, int baudRate, bool &isOk)
{
    //如果已经打开则先关闭
    if (isOk) {
        DeviceRobot::closeCom(com, isOk);
    }

#ifndef nocom
    //重新设置串口号波特率
    com->setPortName(portName);
    com->setBaudRate((BaudRateType)baudRate);
    if (com->open(QIODevice::ReadWrite)) {
        isOk = true;
    }
#endif

    QString msg = QString("串口 %1 打开%2").arg(portName).arg(isOk ? "成功" : "失败");
    DeviceHelper::addMsg(msg, isOk ? 0 : 1);
    DbQuery::addUserLog("运行日志", msg);
    QtHelper::showTipBox("提示", msg, AppConfig::FullScreen, true, 3);
}

void DeviceRobot::closeCom(QextSerialPort *com, bool &isOk)
{
#ifndef nocom
    com->close();
    isOk = false;
#endif
}

SINGLETON_IMPL(DeviceRobot)
DeviceRobot::DeviceRobot(QObject *parent) : QObject(parent)
{
    isOk = false;
#ifndef nocom
    com = new QextSerialPort(QextSerialPort::Polling, this);
#endif

    //启动定时器读取数据
    QTimer *timerRead = new QTimer(this);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));
    timerRead->start(100);

    //启动定时器处理数据
    QTimer *timerCheck = new QTimer(this);
    connect(timerCheck, SIGNAL(timeout()), this, SLOT(checkData()));
    timerCheck->start(300);
}

void DeviceRobot::readData()
{
#ifndef nocom
    //串口通信失败或者没有数据可处理则返回
    if (!isOk || com->bytesAvailable() <= 0) {
        return;
    }

    //需要加锁防止冲突
    mutex.lock();
    //将收到的数据存入队列
    QByteArray data = com->readAll();
    buffer.append(data);
    mutex.unlock();
    emit receiveData(AppConfig::PortNameC, data);
#endif
}

void DeviceRobot::checkData()
{
    //数据队列数据字节不够则返回
    //协议规约: 帧头0x8F -> 标识0x01~0x04 -> 数据data -> 帧尾0xF8
    //示例数据: 0x8F 0x02 0x27 0xF8
    int bufferLen = buffer.length();
    //打印下缓冲区的数据长度是否会一直增加,正式运行阶段要注释掉下面这行
    emit receiveData(0, bufferLen);
    if (bufferLen < 4) {
        return;
    }

    //判断是否收到一条完整的数据
    //先找到头部字节,有可能前面有杂数据
    quint8 dataStart = 0x8F;
    quint8 dataEnd = 0xF8;
    int indexStart = buffer.indexOf(dataStart);
    int indexEnd = buffer.indexOf(dataEnd);
    //帧头帧尾都必须找到
    if (indexStart < 0 || indexEnd < 0) {
        return;
    }

    //需要加锁防止冲突
    mutex.lock();

    //帧头不在第一位则移除前面杂数据
    if (indexStart > 0) {
        buffer = buffer.mid(indexStart, bufferLen);
        //切记这里需要重新计算,否则后面的计算可能会导致出错
        bufferLen = buffer.length();
        indexStart = 0;
        indexEnd = buffer.indexOf(dataEnd);
    }

    //取出功能码,有多种功能码
    quint8 fun = buffer.at(1);
    //根据不同的功能码处理对应的信息包
    QString data;
    QByteArray temp;

    //测试的几条数据
    //8F 01 27 0D FC C5 74 06 5B A5 F8
    //8F 02 27 F8
    //8F 03 01 01 02 F8
    //8F 04 10 00 64 32 F8
    //极端测试数据 黏包+杂数据+大数据
    //00 11 8F 01 27 0D FC C5 74 06 5B A5 F8 AA 8F 02 27 F8 8F 03 01 01 02 F8 55 8F 04 10 00 64 32 F8 BB CC

    if (fun == 0x01) {
        //0x8F 0x01 0x27 0x0D 0xFC 0xC5 0x74 0x06 0x5B 0xA5 0xF8
        //其中0x27代表纬度整数位 0x0D 0xFC 0xC5代表纬度小数位
        //0x27=39 0x0DFCC5=916677 纬度=39.916677
        //其中0x74代表经度整数位 0x06 0x5B 0xA5代表经度小数位
        //0x74=116 0x065BA5=416677 经度=116.416677
        if (bufferLen >= 11) {
            quint8 latInteger = buffer.at(2);
            temp = buffer.mid(3, 3);
            //只有3字节前面要补零才能成为一个整型
            temp.insert(0, data_zero);
            int latDecimal = QtHelper::byteToInt(temp);

            quint8 lngInteger = buffer.at(6);
            temp = buffer.mid(7, 3);
            //只有3字节前面要补零才能成为一个整型
            temp.insert(0, data_zero);
            int lngDecimal = QtHelper::byteToInt(temp);
            data = QString("%1|%2|%3|%4").arg(lngInteger).arg(lngDecimal).arg(latInteger).arg(latDecimal);
        }
    } else if (fun == 0x02) {
        //0x8F 0x02 速度0x00 0xF8
        //速度取值范围为0x00 ~ 0xC8
        //用这个值减去100得到实际底盘速度值-100 ~ 100 表示速度值从-100%到100%
        quint8 twist = buffer.at(2);
        int value = twist - 100;
        data = QString("%1").arg(value);
    } else if (fun == 0x03) {
        //0x8F 0x03 belt obliqueDrawer flatFrawer armauto 0xF8
        if (bufferLen >= 7) {
            quint8 belt = buffer.at(2);
            quint8 obliqueDrawer = buffer.at(3);
            quint8 flatFrawer = buffer.at(4);
            quint8 armauto = buffer.at(5);
            data = QString("%1|%2|%3|%4").arg(belt).arg(obliqueDrawer).arg(flatFrawer).arg(armauto);
        }
    } else if (fun == 0x04) {
        if (bufferLen >= 7) {
            quint8 linearModule = buffer.at(2);
            quint8 hallPutter1 = buffer.at(3);
            quint8 hallPutter2 = buffer.at(4);
            quint8 hallPutter3 = buffer.at(5);
            data = QString("%1|%2|%3|%4").arg(linearModule).arg(hallPutter1).arg(hallPutter2).arg(hallPutter3);
        }
    }

    if (!data.isEmpty()) {
        emit receiveData(fun, data);
        qDebug() << TIMEMS << fun << data;
    }

    //可能会有黏包,需要将尾部下一个数据包的数据放到下一次处理
    int index = indexEnd + 1;
    buffer = buffer.mid(index, bufferLen - index);
    mutex.unlock();

    //如果后面还有数据则立即再处理类似递归处理,在大量数据频繁涌入的时候特别有用
    if (buffer.length() > 0) {
        checkData();
    }
}

void DeviceRobot::open()
{
    DeviceRobot::openCom(com, AppConfig::PortNameC, AppConfig::BaudRateC, isOk);
}

void DeviceRobot::close()
{
    DeviceRobot::closeCom(com, isOk);
}

void DeviceRobot::testData(const QString &data)
{
    QByteArray temp = QtHelper::hexStrToByteArray(data);
    buffer.append(temp);
    emit receiveData(AppConfig::PortNameC, temp);
}
