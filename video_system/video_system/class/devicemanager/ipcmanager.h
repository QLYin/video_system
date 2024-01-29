#ifndef IPCMANAGER_H
#define IPCMANAGER_H

#include "singleton.h"

#include "../deviceconnect/cmdhandlermgr.h"
#include "../deviceconnect/tcpcmddef.h"

class IPCManager : public QObject, public IHandler
{
    Q_OBJECT SINGLETON_DECL(IPCManager)

public:
    explicit IPCManager(QObject *parent = 0);

    void handle(const QVariantMap& data) override;

public:

private:
};

#endif // IPCMANAGER_H
