#include "cmdhandlermgr.h"
#include "tcpcmddef.h"
#include <QMap>

namespace {
    static QMap<QString, QString> kCmd2SplitStr; 
}
SINGLETON_IMPL(CmdHandlerMgr)
CmdHandlerMgr::CmdHandlerMgr(QObject *parent) : QObject(parent)
{
    kCmd2SplitStr = {
        {CommandNS::kCmdSyncIpcInfoR, "init_flag"},
        {CommandNS::kCmdSyncDevInfoR, "dev_id"}
    };
}

bool CmdHandlerMgr::registHandler(IHandler* handler)
{
    m_handlers.append(handler);
    return true;
}

bool CmdHandlerMgr::unRegistHandler(IHandler* handler)
{
    m_handlers.removeOne(handler);
    return true;
}

bool CmdHandlerMgr::unregistAll()
{
    m_handlers.clear();
    return true;
}

void CmdHandlerMgr::handle(const QString& message)
{
    if (message.isEmpty())
    {
        return;
    }

    QString cmd;
    QString cmdData;
    QStringList lines = message.split("\r\n");
    if (!lines.isEmpty() && lines.at(0).contains("cmd :"))
    {
        cmd = lines.at(0).split(":").at(1).trimmed();
        qDebug() << __FUNCTION__ << cmd;
        qDebug() << __FUNCTION__ << message;
        qDebug() << __FUNCTION__ << message.indexOf("\r\n");
        auto firstIndex = message.indexOf("\r\n") + 2;
        if (message.size() > firstIndex)
        {
            cmdData = message.mid(firstIndex, message.length() - firstIndex);
        }
    }

    if (cmd.isEmpty())
    {
        return;
    }

    QVariantMap data;
    QString splitStr = kCmd2SplitStr[cmd];
    if (!splitStr.isEmpty() && !cmdData.isEmpty()) // 存在定义的分隔符则说明特殊命令 cmdData里面存在重复key
    {
        data["cmd"] = cmd;
        QStringList itemStringlist = cmdData.split(splitStr);
        QVector<QVariantMap> vecData;
        for (auto& item : itemStringlist)
        {
            if (!item.isEmpty())
            {
                QString fullItem = splitStr + item;
                QStringList lines = fullItem.split("\r\n");
                QVariantMap itemData;
                for (auto& line : lines)
                {
                    if (!line.isEmpty() && line.contains(":"))
                    {
                        auto key = line.split(":").at(0).trimmed();
                        auto value = line.split(":").at(1).trimmed();
                        itemData.insert(key, value);
                    }
                }
                if (!itemData.isEmpty())
                {
                    vecData.push_back(itemData);
                }
            }
        }
        QVariant variant;
        variant.setValue(vecData);
        data["cmdDataArrary"] = variant;
    }
    else 
    {
        for (auto& line : lines)
        {
            qDebug() << "[TcpClient]read line: " << line;
            if (!line.isEmpty() && line.contains(":"))
            {
                auto key = line.split(":").at(0).trimmed();
                auto value = line.split(":").at(1).trimmed();
                data.insert(key, value);
            }
        }
    }

    if (!data.isEmpty())
    {
        for (auto& handler : m_handlers)
        {
            handler->handle(data);
        }
    }
}