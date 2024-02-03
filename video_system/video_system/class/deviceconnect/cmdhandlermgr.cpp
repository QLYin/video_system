#include "cmdhandlermgr.h"

#include <QMap>
#include <QTimer>
#include "tcpclienthelper.h"

namespace {
    static QMap<QString, QString> kCmd2SplitStr;
    static QSet<QString> kOnewayCmdList;
    static QMap<QString, QString> kResponseCmd2RequestCmd;
    static int cmdId = 0;
}
SINGLETON_IMPL(CmdHandlerMgr)
CmdHandlerMgr::CmdHandlerMgr(QObject *parent) : QObject(parent)
{
    kCmd2SplitStr = {
        {CommandNS::kCmdSyncIpcInfoR, "init_flag"},
        {CommandNS::kCmdSyncDevInfoR, "dev_id"},
        {CommandNS::kCmdDevSearch, "dev_id"}
    };

    kOnewayCmdList = {
        "SetScrCharPs",
        "MoveSenseGet",
        "HeartBeatGet",
        "nop",
        "NetSet",
        "SysSetAddDev",
        "SysSetDelDev",
        "SysSetModifyDevNum",
        "DelIpc",
        "AutoAddIpc2",
        "EditIpc"
    };

    cmdId = 0;
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

    // 判断命令 处理下一条
    // SyncFinsh这条指令必须要新发一条数据才能收到 所以命令判断无意义了 直接出队发送下一条 nop指令要配合syncdata使用
    if (!m_cmdQue.isEmpty())
    {
        m_cmdQue.dequeue();
    }

    if (!m_cmdQue.isEmpty())
    {
        sendCmdImp();
    }

    // 处理cmd数据
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
                        int colonIndex = line.indexOf(":"); // 查找冒号的位置
                        auto key = line.left(colonIndex).trimmed();
                        auto value = line.mid(colonIndex + 1).trimmed();
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

void CmdHandlerMgr::sendCmd(const QString& cmd,  QVariantMap param)
{
    cmdId++;
    RequestCmdInfo obj = { cmdId, cmd ,param };
    m_cmdQue.enqueue(obj);

    if (m_cmdQue.size() == 1)
    {
        sendCmdImp();
    }
}

void CmdHandlerMgr::sendCmdImp()
{
    if (m_cmdQue.isEmpty())
    {
        return;
    }
    auto obj = m_cmdQue.head();
    int id = obj.id;
    QString cmd = obj.cmd;
    TcpClientHelper::sendCmd(cmd, obj.param);
    m_currentId = id;
    
    if (isOnewayCmd(cmd)) // 单向的命令 无返回
    {
        // 1s后发送下一条指令
        QTimer::singleShot(1000, this, [&]()
            {
                if (!m_cmdQue.isEmpty())
                {
                    m_cmdQue.dequeue();
                }
                if (!m_cmdQue.isEmpty())
                {
                    sendCmdImp();
                }
            });
    }
    else 
    {
        QTimer::singleShot(5000, this, [=]()
            {
                if (id == m_currentId) // 5s后还未收到回包
                {
                    if (!m_cmdQue.isEmpty())
                    {
                        m_cmdQue.dequeue();
                    }
                    if (!m_cmdQue.isEmpty())
                    {
                        sendCmdImp();
                    }
                    emit timeout(cmd);
                }
            });
    }
}


bool CmdHandlerMgr::isOnewayCmd(const QString& cmd)
{
    return  kOnewayCmdList.contains(cmd);
}