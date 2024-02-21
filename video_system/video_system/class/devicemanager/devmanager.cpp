#include "devmanager.h"
#include <QMap>
#include <QMetaType>

#include "frmconfigcard/frmconfigcard.h"
#include "frmconfigcard/frmsearchcard.h"

SINGLETON_IMPL(DevManager)
DevManager::DevManager(QObject *parent) : QObject(parent)
{
    CmdHandlerMgr::Instance()->registHandler(this);

    qRegisterMetaType<DevInfo>("DevInfo");
    qRegisterMetaType<DevInfo>("DevInfo&");
}

void DevManager::handle(const QVariantMap& data)
{
    auto cmd = data["cmd"].toString();
    if (cmd == CommandNS::kCmdSyncDevInfoR)
    {
        QVector<QVariantMap> vecData;
        QVariant variant = data["cmdDataArrary"];
        vecData = variant.value<QVector<QVariantMap>>();
        for (auto& item : vecData)
        {
            if (!item.isEmpty())
            {
                DevInfo devItem;
                devItem.dev_id = item["dev_id"].toInt();
                devItem.chn_cnt = item["chn_cnt"].toInt();
                devItem.floor = item["floor"].toInt();
                devItem.upper = item["upper"].toInt();
                devItem.width = item["width"].toInt();
                devItem.height = item["height"].toInt();
                devItem.fresh_freq = item["fresh_freq"].toInt();
                devItem.ipaddr = item["ipaddr"].toString();
                devItem.softwareversion = item["softwareversion"].toString();
                if (devItem.chn_cnt > 0)
                {
                    for (int i = 0; i < devItem.chn_cnt; ++i)
                    {
                        auto key = QString("chn_") + QString::number(i);
                        devItem.ipc_indexs.push_back(item[key].toInt());
                    }
                }

                m_devList.append(devItem);
            }
        }
        if (m_cardWidget && !m_devList.isEmpty())
        {
            m_cardWidget->updateTableWidget(m_devList);
        }
    }
    else if (cmd == CommandNS::kCmdDevSearch)
    {
        QVector<QVariantMap> vecData;
        QVariant variant = data["cmdDataArrary"];
        vecData = variant.value<QVector<QVariantMap>>();
        QVector<DevInfo> searchDevList;
        for (auto& item : vecData)
        {
            if (!item.isEmpty())
            {
                DevInfo devItem;
                devItem.dev_id = item["dev_id"].toInt();
                devItem.chn_cnt = item["chn_cnt"].toInt();
                devItem.floor = item["floor"].toInt();
                devItem.upper = item["upper"].toInt();
                devItem.width = item["width"].toInt();
                devItem.height = item["height"].toInt();
                devItem.fresh_freq = item["fresh_freq"].toInt();
                devItem.ipaddr = item["ipaddr"].toString();
                devItem.gateway = item["gateway"].toString();
                devItem.netmask = item["netmask"].toString();
                devItem.mac = item["mac"].toString();
                devItem.softwareversion = item["softwareversion"].toString();
             
                searchDevList.append(devItem);
            }
        }
        if (m_cardWidget && m_cardWidget->searchCardDialog()->isVisible() && !searchDevList.isEmpty())
        {
            m_cardWidget->searchCardDialog()->updateTableWidget(searchDevList);
        }
    }
}

QVector<DevInfo>& DevManager::devListInfo()
{
    return m_devList;
}

void DevManager::initConfigCard(frmConfigCard* cardWidget)
{
    m_cardWidget = cardWidget;
    if (m_cardWidget && !m_devList.isEmpty())
    {
        m_cardWidget->updateTableWidget(m_devList);
    }
    connect(m_cardWidget, &frmConfigCard::cardRemoveSig, this, &DevManager::onCardRemove);
    connect(m_cardWidget, &frmConfigCard::cardUpdateSig, this, &DevManager::onCardUpdate);
    connect(m_cardWidget->searchCardDialog(), &frmSearchCard::cardUpdateSig, this, &DevManager::onCardUpdate);
}

void DevManager::onCardRemove(const QVector<int> devIds)
{
    int count = 0;
    for (auto& id : devIds)
    {
        auto index = getIndexById(id);
        if (index != -1)
        {
            QVariantMap param;
            param["dev_id"] = id;
            param["chn_cnt"] = m_devList.at(index).chn_cnt;
            param["floor"] = m_devList.at(index).floor;
            param["upper"] = m_devList.at(index).upper;
            param["width"] = m_devList.at(index).width;
            param["heigth"] = m_devList.at(index).height;
            param["fresh_freq"] = m_devList.at(index).fresh_freq;
            param["softwareversion"] = m_devList.at(index).softwareversion;
            CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDevDel, param);
            m_devList.remove(index);
            count++;
        }
    }

    if (count > 0)
    {
        QVariantMap param;
        param["dev_num"] = count;
        CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDevNumSet, param);
    }

    if (m_cardWidget && !m_devList.isEmpty())
    {
        m_cardWidget->updateTableWidget(m_devList);
    }
}

void DevManager::onCardUpdate(const QVector<DevInfo>& devListInfo)
{
    if (devListInfo.isEmpty())
    {
        return;
    }
    // 遍历 devListInfo 添加或更新数据到m_devList; 并发送对应指令 且修改UI
    for (auto& info : devListInfo)
    {
        if (hasCard(info)) // 存在
        {
            // 对比ip, mac, gateway是否有变化
            auto card = getCardbyId(info.dev_id);
            if (card.ipaddr != info.ipaddr ||
                card.gateway != info.gateway ||
                card.netmask != info.netmask)
            {
                card.ipaddr = info.ipaddr;
                card.gateway = info.gateway;
                card.netmask = info.netmask;

                QVariantMap param;
                param["dev_id"] = card.dev_id;
                param["ipaddr"] = card.ipaddr;
                param["gateway"] = card.gateway;
                param["netmask"] = card.netmask;
                param["mac"] = card.mac;
                CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDevNetSet, param);
            }
        }
        else // 不存在则添加 
        {
            m_devList.push_back(info);

            QVariantMap param;
            param["dev_id"] = info.dev_id;
            param["chn_cnt"] = -1;
            param["floor"] = info.floor;
            param["upper"] = info.upper;
            param["width"] = info.width;
            param["heigth"] = info.height;
            param["fresh_freq"] = info.fresh_freq;
            param["softwareversion"] = info.softwareversion;
            CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDevAdd, param);
        }
    }

    if (m_cardWidget && !m_devList.isEmpty())
    {
        m_cardWidget->updateTableWidget(m_devList);
    }
}

bool DevManager::hasCard(DevInfo info)
{
    QSet<int> vecIds;
    for (auto& card : m_devList)
    {
        vecIds.insert(card.dev_id);
    }

    return !vecIds.isEmpty() && vecIds.contains(info.dev_id);
}

DevInfo& DevManager::getCardbyId(int devId)
{
    for (auto& card : m_devList)
    {
        if (card.dev_id == devId)
        {
            return card;
        }
    }
}

int DevManager::getIndexById(int devId)
{
    for (int i = 0; i < m_devList.size(); ++i) {
        if (m_devList[i].dev_id == devId) {          
            return i;
        }
    }

    return -1;
}