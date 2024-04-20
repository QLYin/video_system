#include "ipcmanager.h"
#include <QMap>
#include <QMetaType>

#include "dbquery.h"
#include "frmconfigipc.h"
#include "onvifthread.h"
#include "deviceonvif.h"

SINGLETON_IMPL(IPCManager)
IPCManager::IPCManager(QObject *parent) : QObject(parent)
{
	CmdHandlerMgr::Instance()->registHandler(this);

    qRegisterMetaType<IpcInfo>("IpcInfo");
    qRegisterMetaType<IpcInfo>("IpcInfo&");
    qRegisterMetaType<QList<IpcInfo>>("QList<IpcInfo>");
    qRegisterMetaType<QList<IpcInfo>>("QList<IpcInfo>&");

    if (AppConfig::SynTime)
    {
        int interval = AppConfig::SynTimeInterval > 0 ? AppConfig::SynTimeInterval : 10;
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &IPCManager::onTimeout);
        m_timer->start(interval* 1000); // 启动定时器
    }
}

IPCManager::~IPCManager()
{
    if (m_timer)
    {
        m_timer->stop();
    }
}

void IPCManager::onTimeout() 
{
    if (!m_configIpc)
    {
        return;
    }

    QSqlTableModel* model = m_configIpc->sqlModel();
    if (!model)
    {
        return;
    }

    int count = model->rowCount();
    for (int i = 0; i < count; ++i)
    {
        auto rtspurl = model->index(i, 8).data().toString();
        if (!rtspurl.isEmpty())
        {
            OnvifDeviceData deviceData;
            if (DeviceOnvif::checkUrl(rtspurl, deviceData))
            {
                OnvifThread::Instance()->append(deviceData, "setDateTime");
            }
        }
    }

}

void IPCManager::handle(const QVariantMap& data)
{
	auto cmd = data["cmd"].toString();
	if (cmd == CommandNS::kCmdSyncIpcInfoR)
	{
        // 先清理ipc表格
        QSqlTableModel* model = nullptr;
        if (m_configIpc)
        {
            model = m_configIpc->sqlModel();
            DbQuery::clearIpcInfo();
            AppEvent::Instance()->slot_saveIpcInfo(true);
            model->select();
        }
        QVector<QVariantMap> vecData;
        QVariant variant = data["cmdDataArrary"];
        vecData = variant.value<QVector<QVariantMap>>();
        m_ipcList.clear();
        for (auto& item : vecData)
        {
            if (!item.isEmpty())
            {
                IpcInfo ipcItem;
                ipcItem.init_flag = item["init_flag"].toInt();
                ipcItem.id = item["id"].toInt();
                ipcItem.name = item["name"].toString();
                ipcItem.user = item["user"].toString();
                ipcItem.passwd = item["passwd"].toString();
                ipcItem.ipaddr = item["ipaddr"].toString();
                ipcItem.ptz_enable = item["ptz_enable"].toInt();
                ipcItem.vda_enable = item["vda_enable"].toInt();
                ipcItem.rtsp_url0 = item["rtsp_url0"].toString();
                ipcItem.rtsp_url1 = item["rtsp_url1"].toString();
                ipcItem.resolution0 = item["resolution0"].toInt();
                ipcItem.resolution1 = item["resolution1"].toInt();
                m_ipcList.append(ipcItem);
                if (model)
                {
                    int count = model->rowCount();
                    model->insertRows(count,1);
                    model->setData(model->index(count, 0), ipcItem.id);
                    model->setData(model->index(count, 1), ipcItem.name);
                    model->setData(model->index(count, 2), "网络视频");
                    model->setData(model->index(count, 3), "");
                    model->setData(model->index(count, 4), ipcItem.ipaddr);
                    QString onvifAddr = QString("http://%1/onvif/device_service").arg(ipcItem.ipaddr);
                    model->setData(model->index(count, 5), onvifAddr);
                    model->setData(model->index(count, 6), "");
                    model->setData(model->index(count, 7), "");
                    model->setData(model->index(count, 8), ipcItem.rtsp_url0);
                    model->setData(model->index(count, 9), ipcItem.rtsp_url1);
                    auto resolution0 = IPC::index2Name(ipcItem.resolution0);
                    model->setData(model->index(count, 10), resolution0.isEmpty() ? "自动" : resolution0);
                    auto resolution1 = IPC::index2Name(ipcItem.resolution1);
                    model->setData(model->index(count, 11), resolution1.isEmpty() ? "自动" : resolution1);
                    model->setData(model->index(count, 12), 0);
                    model->setData(model->index(count, 13), 0);
                    model->setData(model->index(count, 14), ipcItem.user);
                    model->setData(model->index(count, 15), ipcItem.passwd);
                    model->setData(model->index(count, 16), "启用");
                    model->setData(model->index(count, 17), QString("%1_%2").arg(ipcItem.ptz_enable).arg(ipcItem.vda_enable));
                }             
            }
        }

        if (model)
        {
            model->submitAll();
            AppEvent::Instance()->slot_saveIpcInfo(true);
            model->select();
        }
	}

}

void IPCManager::initConfigIpc(frmConfigIpc* ipcWidget)
{
    m_configIpc = ipcWidget;

    connect(m_configIpc, &frmConfigIpc::ipcDelSig, this, &IPCManager::onIpcDel);
    connect(m_configIpc, &frmConfigIpc::ipcAddSig, this, &IPCManager::onIpcAdd);
    connect(m_configIpc, &frmConfigIpc::ipcEditSig, this, &IPCManager::onIpcEdit);
}

void IPCManager::onIpcDel(QStringList ids, QStringList addrs)
{
    qDebug() << __FUNCTION__ << ",ids : " << ids;
    qDebug() << __FUNCTION__ << ",addrs : " << addrs;

    if (ids.isEmpty() || addrs.isEmpty())
    {
        return;
    }

    auto len = ids.length();
    for (int i = 0; i < len; ++i)
    {
        QVariantMap param;
        auto key = QString("ipc_%1").arg(ids.at(i));
        param[key] = addrs.at(i);
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdIPCDel, param);
    }
}

void IPCManager::onIpcAdd(const QList<IpcInfo>& ipcList)
{
    qDebug() << __FUNCTION__ << ",ipcList size : " << ipcList.size();

    QVariantMap param;
    param["num"] = ipcList.size();
    QVariant variant;
    variant.setValue(ipcList);
    param["ipcList"] = variant;
    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdIPCAutoAdd, param);
}

void IPCManager::onIpcEdit(int id, const IpcInfo& ipc)
{
    QVariantMap param;
    param["id"] = id;
    QVariant variant;
    variant.setValue(ipc);
    param["ipcItem"] = variant;

    CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDIPCModify, param);
}

QList<IpcInfo>& IPCManager::ipcList()
{
    return m_ipcList;
}


QString IPCManager::findIp(int id)
{
    QString ip;
    for (auto& ipc : m_ipcList)
    {
        if (ipc.id == id)
        {
            ip = ipc.ipaddr;
            break;
        }
    }
    return ip;
}

int IPCManager::findId(QString ip)
{
    int id;
    for (auto& ipc : m_ipcList)
    {
        if (ipc.ipaddr == ip)
        {
            id = ipc.id;
            break;
        }
    }
    return id;
}