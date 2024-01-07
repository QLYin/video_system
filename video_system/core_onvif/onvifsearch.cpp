#include "onvifsearch.h"

OnvifSearch::OnvifSearch(QObject *parent) : QObject(parent)
{
    isOk = false;
    currentIP = "127.0.0.1";
    searchFilter = "none";
    searchInterval = 300;

    //定时器排队发送搜索命令,有好几种
    timerSearch = new QTimer(this);
    connect(timerSearch, SIGNAL(timeout()), this, SLOT(sendData()));
    timerSearch->setInterval(searchInterval);

    //onvif搜索对象
    udpSocket = new QUdpSocket(this);
    //udp设置buffer不起作用
    //udpSocket->setReadBufferSize(65536);
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,5))
    udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
#endif
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readData()));

    //定时器排除处理接收到的数据
    deviceCount = 0;
    timerData = new QTimer(this);
    connect(timerData, SIGNAL(timeout()), this, SLOT(checkData()));
    timerData->start(100);
}

OnvifSearch::~OnvifSearch()
{
    if (timerSearch->isActive()) {
        timerSearch->stop();
    }

    if (timerData->isActive()) {
        timerData->stop();
    }
}

void OnvifSearch::sendData()
{
    //依次发送数据,如果到了最后一个则停止
    //根据onvif device test工具抓包分析,只要发送前面两个就行,后面两个是ONVIF Device Manager抓包的
    //在收到结果的地方要对重复的进行过滤,因为部分设备两种协议请求都会返回
    //可以自行调整 timerSearch->stop(); 的位置来提高速度,比如很多情况下只要发送一次就可以
    writeData(OnvifHost);
    if (currentFile == ":/onvifsend/SearchDevice1.xml") {
        currentFile = ":/onvifsend/SearchDevice2.xml";
    } else if (currentFile == ":/onvifsend/SearchDevice2.xml") {
        currentFile = ":/onvifsend/SearchDevice3.xml";
    } else if (currentFile == ":/onvifsend/SearchDevice3.xml") {
        currentFile = ":/onvifsend/SearchDevice4.xml";
    } else if (currentFile == ":/onvifsend/SearchDevice4.xml") {
        timerSearch->stop();
    }
}

void OnvifSearch::writeData(const QString &ip)
{
    QByteArray data = OnvifHelper::getFile(currentFile);
    if (!data.isEmpty()) {
        data = QString(data).arg(OnvifHelper::getUuid()).toUtf8();
        udpSocket->writeDatagram(data, QHostAddress(ip), OnvifPort);
        emit sendData(data);
    }
}

void OnvifSearch::readData(const QString &fileName)
{
    //从文件读取数据解析,主要方便用来测试各种摄像机返回的数据
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QByteArray data = file.readAll();
        data.replace("\\\"", "\"");
        checkData(data, "");
        file.close();
    }
}

void OnvifSearch::readData()
{
    QByteArray data;
    QHostAddress host;
    quint16 port = 0;

    //QMutexLocker locker(&mutex);
    while (udpSocket->hasPendingDatagrams()) {
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(), data.size(), &host, &port);
        QString ip = host.toString();
#if 0
        //改成了通过队列来处理数据 不然设备很多的时候在这里处理会导致部分数据丢失
        checkData(data, ip);
        emit receiveData(data);
#else
        ips << ip;
        buffers << data;
#endif
        deviceCount++;
        //qDebug() << TIMEMS << deviceCount << ip << port << data.size();
    }
}

void OnvifSearch::checkData()
{
    //QMutexLocker locker(&mutex);
    if (buffers.size() > 0) {
        QString ip = ips.takeFirst();
        QByteArray data = buffers.takeFirst();
        checkData(data, ip);
        emit receiveData(data);
    }
}

void OnvifSearch::checkData(const QByteArray &data, const QString &ip)
{
    OnvifQuery query;
    if (!query.setData(data)) {
        return;
    }

    //定义结构体存储设备信息
    OnvifDeviceInfo deviceInfo;
    query.getSearchInfo(deviceInfo, ip);

    QString addr = deviceInfo.onvifAddr;
    if (addr.isEmpty()) {
        return;
    }

    //已经存在的地址不用继续
    //这里为什么不增加直接过滤IP作为重复设备的判断,因为IP可能一样但是具体的设备地址可能是不一样的
    if (checkExist(addr)) {
        return;
    }

    //按照过滤条件过滤设备 适用于设备很多而且设备类型很多的情况
    if (searchFilter != "none") {
        //默认80端口的不会带 :80  前面有 http: https: 所以要取后面的 : 索引位置来判断
        if (searchFilter == ":80") {
            if (addr.indexOf(":", 8) >= 8) {
                return;
            }
        } else {
            if (!addr.contains(searchFilter)) {
                return;
            }
        }
    }

    deviceInfos << deviceInfo;
    emit receiveDevice(deviceInfo);
    emit receiveInfo(QString("发现设备 -> %1").arg(addr));
}

bool OnvifSearch::checkExist(const QString &addr)
{
    //过滤掉重复的设备,发送搜索设备的命令有好几种,某些设备支持多种命令,所以会返回多次
    bool exist = false;
    foreach (OnvifDeviceInfo deviceInfo, deviceInfos) {
        if (deviceInfo.onvifAddr == addr) {
            exist = true;
            break;
        }
    }
    return exist;
}

void OnvifSearch::setSearchFilter(const QString &searchFilter)
{
    this->searchFilter = searchFilter;
}

void OnvifSearch::setSearchInterval(int searchInterval)
{
    this->searchInterval = searchInterval;
    timerSearch->setInterval(searchInterval < 100 ? 100 : searchInterval);
}

void OnvifSearch::clear()
{
    deviceInfos.clear();
}

bool OnvifSearch::search(const QString &localIP, const QString &deviceIP)
{
    if (timerSearch->isActive()) {
        timerSearch->stop();
    }

    deviceCount = 0;
    ips.clear();
    buffers.clear();

    deviceInfos.clear();
    if (!OnvifHelper::isIP(localIP)) {
        return false;
    }

    //如果改变了IP地址则重新绑定,还未成功则先绑定
    if (currentIP != localIP) {
        isOk = false;
    }

    if (!isOk) {
        currentIP = localIP;
        udpSocket->abort();
        isOk = udpSocket->bind(QHostAddress(localIP), 0, QUdpSocket::ShareAddress);
    }

    if (!isOk) {
        emit receiveError(QString("绑定组播 -> 失败: %1").arg(udpSocket->errorString()));
        return false;
    }

    //单播搜索对指定的地址发送搜索命令
    if (!deviceIP.isEmpty()) {
        //判断是否是IP还是地址(http开头表示指定onvif地址进行搜索)
        //http://192.168.1.128/onvif/device_service
        if (deviceIP.startsWith("http")) {
            QString ip = OnvifHelper::getUrlIP(deviceIP);
            QStringList list = deviceIP.split("/");
            if (list.count() < 3) {
                emit receiveError(QString("地址错误 -> %1").arg(deviceIP));
                return false;
            }

            int port = 80;
            QString info = list.at(2);
            if (info.contains(":")) {
                list = info.split(":");
                ip = list.at(0);
                port = list.at(1).toInt();
            }

            //先判断地址是否通
            if (!OnvifHelper::hostLive(ip, port)) {
                emit receiveError(QString("地址不通 -> %1").arg(deviceIP));
                return false;
            }

            //构建一个设备
            OnvifDeviceInfo deviceInfo;
            deviceInfo.onvifAddr = deviceIP;
            deviceInfo.deviceIp = ip;
            deviceInfos << deviceInfo;
            emit receiveDevice(deviceInfo);
            emit receiveInfo(QString("发现设备 -> %1").arg(deviceIP));
        } else {
            currentFile = ":/onvifsend/SearchDevice2.xml";
            writeData(deviceIP);
        }
    } else {
        //间隔小于0.1s则认为只需要发送一次数据
        if (searchInterval >= 100) {
            currentFile = ":/onvifsend/SearchDevice1.xml";
            timerSearch->start();
        } else {
            //searchInterval=2 则发送 SearchDevice2.xml
            currentFile = QString(":/onvifsend/SearchDevice%1.xml").arg(searchInterval);
        }

        //之前是直接全部放在这里发送,发现部分设备要好几次才能回来
        //改成定时器排队发送多种广播搜索数据,就没有问题
        sendData();
    }

    return true;
}

QList<OnvifDeviceInfo> OnvifSearch::getDeviceInfos()
{
    return this->deviceInfos;
}

QStringList OnvifSearch::getDeviceAddrs()
{
    QStringList addrs;
    foreach (OnvifDeviceInfo deviceInfo, deviceInfos) {
        addrs << deviceInfo.onvifAddr;
    }
    return addrs;
}
