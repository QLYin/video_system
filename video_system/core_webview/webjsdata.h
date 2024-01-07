#ifndef WEBJSDATA_H
#define WEBJSDATA_H

#include <QObject>

//需要自定义继承自QObject的类来接收QWebEngineView交互数据,不然会打印很多烦人的信息
class WebJsData : public QObject
{
    Q_OBJECT
public:
    static WebJsData *Instance();
    explicit WebJsData(QObject *parent = 0);

public slots:
    //定义两个参数,这样就涵盖了所有的情况 type表示类型相当于唯一标识
    void receiveData(const QString &type, const QVariant &data);

signals:
    void receiveDataFromJs(const QString &type, const QVariant &data);
};

#endif // WEBJSDATA_H
