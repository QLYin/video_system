#include "ipcmanager.h"
#include <QMap>


SINGLETON_IMPL(IPCManager)
IPCManager::IPCManager(QObject *parent) : QObject(parent)
{
	CmdHandlerMgr::Instance()->registHandler(this);
}

void IPCManager::handle(const QVariantMap& data)
{

}