#include "deviceuavsserver.h"
#include "qthelper.h"

SINGLETON_IMPL(DeviceUavsServer)
DeviceUavsServer::DeviceUavsServer(QObject *parent) : QObject(parent)
{
}

void DeviceUavsServer::start()
{
    //启动子程序
    QString bin1 = QtHelper::appPath() + "/uavspanel.exe";
    process.start(bin1);
}
