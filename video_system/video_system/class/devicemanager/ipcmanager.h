﻿#ifndef IPCMANAGER_H
#define IPCMANAGER_H

#include <QTimer>
#include "singleton.h"

#include "../deviceconnect/cmdhandlermgr.h"
#include "../deviceconnect/tcpcmddef.h"

class frmConfigIpc;
class IPCManager : public QObject, public IHandler
{
    Q_OBJECT SINGLETON_DECL(IPCManager)

public:
    explicit IPCManager(QObject *parent = 0);
    ~IPCManager();

    void handle(const QVariantMap& data) override;

    void initConfigIpc(frmConfigIpc* ipcWidget);
    QList<IpcInfo>& ipcList();
    QString findIp(int id);
    int findId(QString ip);

private slots:
    void onIpcDel(QStringList ids, QStringList addrs);
    void onIpcAdd(const QList<IpcInfo>& ipcList);
    void onIpcEdit(int id, const IpcInfo& ipc);
    void onTimeout();

private:
    frmConfigIpc* m_configIpc = nullptr;
    QList<IpcInfo> m_ipcList;
    QTimer* m_timer = nullptr;
};

#endif // IPCMANAGER_H
