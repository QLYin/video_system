#ifndef ONVIFREQUEST_H
#define ONVIFREQUEST_H

#include "onvifhead.h"

class OnvifRequest : public QObject
{
    Q_OBJECT
public:
    //超时时间
    static int timeout;
    explicit OnvifRequest(QObject *parent = 0);
    ~OnvifRequest();

    //一个程序中共用一个 QNetworkAccessManager 官方手册写的
    static QNetworkAccessManager *getManager();

private:
    QNetworkAccessManager *manager;
    QNetworkAccessManager *manager2;

private slots:
    void finished(QNetworkReply *reply);
    void initRequest(QNetworkRequest &request, const QString &url);

public:
    //请求数据,返回结果对象
    QNetworkReply *getData(const QString &url, bool get, const QByteArray &data, int timeout = OnvifRequest::timeout);
    QNetworkReply *get(const QString &url, int timeout = OnvifRequest::timeout);
    QNetworkReply *post(const QString &url, const QByteArray &data, int timeout = OnvifRequest::timeout);

    //下面这个方法暂时没用上,现在都是请求数据中就带了用户认证信息
    QNetworkReply *auth(const QString &userName, const QString &userPwd,
                        const QString &url, const QByteArray &data, int timeout = OnvifRequest::timeout);

    //第二个请求对象发送请求,用于事件订阅,分开互不干扰
    void post2(const QString &url, const QByteArray &data);

signals:
    void sendData(const QString &url, const QByteArray &data);
    void receiveData(const QString &url, const QByteArray &data);
    void receiveEvent(const OnvifEventInfo &event);
};

#endif // ONVIFREQUEST_H
