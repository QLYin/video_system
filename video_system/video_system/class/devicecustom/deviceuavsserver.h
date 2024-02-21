#ifndef DEVICEUAVSSERVER_H
#define DEVICEUAVSSERVER_H

#include <QObject>
#include <QProcess>
#include "singleton.h"

class DeviceUavsServer : public QObject
{
    Q_OBJECT SINGLETON_DECL(DeviceUavsServer)

public:
    explicit DeviceUavsServer(QObject *parent = 0);

private:
    QProcess process;

public slots:
    //启动服务
    void start();
};

#endif // DEVICEUAVSSERVER_H
