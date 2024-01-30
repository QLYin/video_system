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
                devItem.ipaddr = item["gateway"].toString();
                devItem.ipaddr = item["netmask"].toString();
                devItem.ipaddr = item["mac"].toString();
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

const QVector<DevInfo>& DevManager::devListInfo()
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
    connect(m_cardWidget, &frmConfigCard::cardAppendsig, this, &DevManager::onCardAppend);
    connect(m_cardWidget, &frmConfigCard::cardRemoveSig, this, &DevManager::onCardRemove);
    connect(m_cardWidget, &frmConfigCard::cardUpdateSig, this, &DevManager::onCardUpdate);
}

void DevManager::onCardAppend(const QVector<DevInfo>& devListInfo)
{

}

void DevManager::onCardRemove(const QVector<int> devIds)
{

}

void DevManager::onCardUpdate(const QVector<DevInfo>& devListInfo)
{

}