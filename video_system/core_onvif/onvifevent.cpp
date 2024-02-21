#include "onvifevent.h"

OnvifEvent::OnvifEvent(QObject *parent) : QObject(parent)
{
    device = (OnvifDevice *)parent;

    //事件定时器请求事件地址
    timerEvent = new QTimer(this);
    connect(timerEvent, SIGNAL(timeout()), this, SLOT(getEvent()));
    timerEvent->setInterval(10 * 60 * 1000);

    //消息定时器请求事件内容
    timerMessage = new QTimer(this);
    connect(timerMessage, SIGNAL(timeout()), this, SLOT(pullMessage()));
    timerMessage->setInterval(1 * 60 * 1000);
}

void OnvifEvent::receiveEvent(const OnvifEventInfo &event)
{
    if (!event.dataName.isEmpty()) {
        emit receiveEvent(device->onvifAddr, event);
        //正确格式 2020-10-10T08:40:14Z|LogicalState>|1
        //过滤格式 2020-10-10T08:23:11.000000Z|State|true>
        if (event.time.length() != 20) {
            return;
        }
    }

    pullMessage();
}

QString OnvifEvent::getEvent(const QString &timeout)
{
    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString file = OnvifHelper::getFile(":/onvifsend/CreatePullPointSubscription.xml");
    file = file.arg(device->getHeadData()).arg(timeout);

    //发送网络请求
    QByteArray dataSend = file.toUtf8();
    QNetworkReply *reply = device->request->post(device->eventUrl, dataSend, OnvifRequest::timeout + 500);

    //拿到请求结果并处理数据
    QByteArray dataReceive;
    bool ok = device->checkData(reply, dataReceive, "订阅事件服务");
    if (ok) {
        //解析事件请求地址
        OnvifQuery query;
        if (query.setData(dataReceive)) {
            eventAddr = query.getEventAddr(device->addrPort);
            QTimer::singleShot(100, this, SLOT(pullMessage()));
        }
    }

    //启动事件定时器
    if (!timerEvent->isActive()) {
        timerEvent->start();
    }

    //启动消息定时器
    if (!timerMessage->isActive()) {
        //timerMessage->start();
    }

    return eventAddr;
}

void OnvifEvent::pullMessage(const QString &timeout)
{
    QMutexLocker locker(&mutex);
    if (eventAddr.isEmpty()) {
        return;
    }

    emit receiveInfo(QString("请求事件 -> %1").arg(eventAddr));

    //读取文件传入带用户认证的通用头部数据和其他参数构建要发送的数据
    QString uuid = OnvifHelper::getUuid();
    QString file = OnvifHelper::getFile(":/onvifsend/PullMessages.xml");
    file = file.arg(device->getUserToken()).arg(uuid).arg(eventAddr).arg(timeout);

    //发送网络请求
    QByteArray dataSend = file.toUtf8();
    device->request->post2(eventAddr, dataSend);
}
