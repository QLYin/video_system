#ifndef CMDHANDLERMRG_H
#define CMDHANDLERMRG_H

#include "singleton.h"
#include <QList>
#include <QVariantMap>

class IHandler 
{
public:
    virtual void handle(const QVariantMap& data) = 0;
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

private:

private:
    QList<IHandler*> m_handlers;
};

#endif // CMDHANDLERMRG_H
