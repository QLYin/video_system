#ifndef CMDHANDLERMRG_H
#define CMDHANDLERMRG_H

#include "singleton.h"
#include <QList>
#include <QVariantMap>
#include <QQueue>

#include "tcpcmddef.h"

class IHandler 
{
public:
    virtual void handle(const QVariantMap& data) = 0;
};

struct RequestCmdInfo 
{
    int id;
    QString cmd;
    QVariantMap param;
};

class CmdHandlerMgr : public QObject
{
    Q_OBJECT SINGLETON_DECL(CmdHandlerMgr)

public:
    explicit CmdHandlerMgr(QObject *parent = 0);

public:
    bool registHandler(IHandler* handler);
    bool unRegistHandler(IHandler* handler);
    bool unregistAll();
    void handle(const QString& handle);
    void sendCmd(const QString& cmd, QVariantMap param = QVariantMap());

private:
    void sendCmdImp();
    bool isOnewayCmd(const QString& cmd);

private:
    QList<IHandler*> m_handlers;
    QQueue<RequestCmdInfo> m_cmdQue;;
    int m_currentId;

public:
    Q_SIGNAL void timeout(QString cmd);
};

#endif // CMDHANDLERMRG_H
