#include "ipcmanager.h"
#include <QMap>
#include <QMetaType>

#include "dbquery.h"
#include "frmconfigipc.h"

SINGLETON_IMPL(IPCManager)
IPCManager::IPCManager(QObject *parent) : QObject(parent)
{
	CmdHandlerMgr::Instance()->registHandler(this);

    qRegisterMetaType<IpcInfo>("IpcInfo");
    qRegisterMetaType<IpcInfo>("IpcInfo&");
    qRegisterMetaType<QList<IpcInfo>>("QList<IpcInfo>");
    qRegisterMetaType<QList<IpcInfo>>("QList<IpcInfo>&");
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
                    model->setData(model->index(count, 2), ipcItem.name);
                    model->setData(model->index(count, 3), "");
                    model->setData(model->index(count, 4), ipcItem.ipaddr);
                    model->setData(model->index(count, 5), "");
                    model->setData(model->index(count, 6), "");
                    model->setData(model->index(count, 7), ipcItem.rtsp_url0);
                    model->setData(model->index(count, 8), ipcItem.rtsp_url1);
                    model->setData(model->index(count, 9), ipcItem.resolution0);
                    model->setData(model->index(count, 10), ipcItem.resolution1);
                    model->setData(model->index(count, 11), 0);
                    model->setData(model->index(count, 12), 0);
                    model->setData(model->index(count, 13), ipcItem.user);
                    model->setData(model->index(count, 14), ipcItem.passwd);
                    model->setData(model->index(count, 15), "启用");
                    model->setData(model->index(count, 16), QString("%1_%2").arg(ipcItem.ptz_enable).arg(ipcItem.vda_enable));
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

void IPCManager::onIpcEdit(const QList<IpcInfo>& ipcList)
{
}
