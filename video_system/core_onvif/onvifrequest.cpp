#include "onvifrequest.h"

int OnvifRequest::timeout = 1500;
OnvifRequest::OnvifRequest(QObject *parent) : QObject(parent)
{
    //全局公用一个请求对象
    manager = getManager();

    //第二个请求对象用于事件订阅,异步获取数据
    manager2 = new QNetworkAccessManager;
    connect(manager2, SIGNAL(finished(QNetworkReply *)), this, SLOT(finished(QNetworkReply *)));
}

OnvifRequest::~OnvifRequest()
{
}

QNetworkAccessManager *OnvifRequest::getManager()
{
    static QNetworkAccessManager *manager = 0;
    if (manager == 0) {
        manager = new QNetworkAccessManager;
    }
    return manager;
}

void OnvifRequest::finished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray dataReceive = reply->readAll();
        if (dataReceive.length() > 0) {
            emit receiveData(reply->url().toString(), dataReceive);
            OnvifQuery query;
            if (query.setData(dataReceive)) {
                emit receiveEvent(query.getEventInfo());
            }
        }
    }

    reply->deleteLater();
}

void OnvifRequest::initRequest(QNetworkRequest &request, const QString &url)
{
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/soap+xml;charset=utf-8;");
    //设置成close表示每次都是短连接,不然不能正常退出
    request.setRawHeader("Connection", "close");
    //request.setRawHeader("Connection", "keep-alive");
#if 0
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::SslV3);
    request.setSslConfiguration(config);
#endif
}

QNetworkReply *OnvifRequest::getData(const QString &url, bool get, const QByteArray &data, int timeout)
{
    if (url.isEmpty()) {
        return 0;
    }

    if (!get && data.isEmpty()) {
        return 0;
    }

    QNetworkRequest request;
    initRequest(request, url);
    QNetworkReply *reply = 0;
    if (get) {
        reply = manager->get(request);
    } else {
        reply = manager->post(request, data);
        emit sendData(url, data);
    }

    //开启事件循环同步获取数据并设置超时
    if (timeout > 0) {
        QEventLoop eventLoop;
        QTimer timer;
        QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
        timer.setSingleShot(true);
        timer.start(timeout);
        connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }

    return reply;
}

QNetworkReply *OnvifRequest::get(const QString &url, int timeout)
{
    return getData(url, true, QByteArray(), timeout);
}

QNetworkReply *OnvifRequest::post(const QString &url, const QByteArray &data, int timeout)
{
    return getData(url, false, data, timeout);
}

QNetworkReply *OnvifRequest::auth(const QString &userName, const QString &userPwd,
                                  const QString &url, const QByteArray &data, int timeout)
{
    QNetworkReply *reply = getData(url, false, data, timeout);
    //当请求失败返回需要认证的时候,重新组织数据发送一次,带用户认证的数据
    if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
        QNetworkRequest request = reply->request();
        QByteArray raw = reply->rawHeader("WWW-Authenticate");

        //Digest realm="Login to 4M00BD6PAJ8BED0",qop="auth",
        //nonce="b252aWYtZGlnZXN0OjQzNDkyNzk1NDAw", opaque="", stale="false"
        QString realm, qop, nonce, opaque;
        QString uri = request.url().path();
        QString nc = "00000001";

        //需要带上服务器返回的口令一起打包,意味着当前回复不是攻击
        QString cnonce = "Q6S5MMWDA2XXdAS5D1C6II5fK6DA65SD";
        if (raw.startsWith("Digest")) {
            QString str = raw.mid(7);
            QStringList list = str.split(',', SkipEmptyPartsx);
            foreach (QString sub, list) {
                sub = sub.trimmed();
                if (sub.startsWith("realm")) {
                    realm = sub.mid(6).replace("\"", "");
                } else if (sub.startsWith("qop")) {
                    qop = sub.mid(4).replace("\"", "");
                } else if (sub.startsWith("nonce")) {
                    nonce = sub.mid(6).replace("\"", "");
                } else if (sub.startsWith("opaque")) {
                    opaque = sub.mid(7).replace("\"", "");
                }
            }
        }

        QString A1 = userName + ":" + realm + ":" + userPwd;
        QString A2 = "POST:" + uri;
        QString HH = nonce + ":" + nc + ":" + cnonce + ":" + qop;
        QByteArray HA1 = QCryptographicHash::hash(A1.toUtf8(), QCryptographicHash::Md5).toHex().toLower();
        QByteArray HA2 = QCryptographicHash::hash(A2.toUtf8(), QCryptographicHash::Md5).toHex().toLower();
        QByteArray HHH = HA1 + ":" + HH.toUtf8() + ":" + HA2;
        QString response = QCryptographicHash::hash(HHH, QCryptographicHash::Md5).toHex();

        QStringList list;
        list << " ";
        list << QString("username=\"%1\"").arg(userName);
        list << QString("realm=\"%1\"").arg(realm);
        list << QString("qop=\"%1\"").arg(qop);
        list << QString("algorithm=\"%1\"").arg("MD5");
        list << QString("uri=\"%1\"").arg(uri);
        list << QString("nonce=\"%1\"").arg(nonce);
        list << QString("nc=%6").arg(nc);
        list << QString("cnonce=\"%1\"").arg(cnonce);
        list << QString("opaque=\"%1\"").arg(opaque);
        list << QString("response=\"%1\"").arg(response);
        QString rawAuth = QString("Digest %1").arg(list.join(","));

        //重新设置认证信息并发送数据
        request.setRawHeader("Host", request.url().host().toUtf8());
        request.setRawHeader("Authorization", rawAuth.toUtf8());
        reply = manager->post(request, data);

        //开启事件循环同步获取数据
        QEventLoop eventLoop;
        connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }

    return reply;
}

void OnvifRequest::post2(const QString &url, const QByteArray &data)
{
    if (url.isEmpty() || data.isEmpty()) {
        return;
    }

    QNetworkRequest request;
    initRequest(request, url);
    manager2->post(request, data);
    emit sendData(url, data);
}
