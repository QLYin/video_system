#ifndef DEVMANAGER_H
#define DEVMANAGER_H

#include "singleton.h"
#include "../deviceconnect/cmdhandlermgr.h"
#include "../deviceconnect/tcpcmddef.h"

class frmConfigCard;

class DevManager : public QObject, public IHandler
{
    Q_OBJECT SINGLETON_DECL(DevManager)

public:
    explicit DevManager(QObject *parent = 0);
    void handle(const QVariantMap& data) override;
    const QVector<DevInfo>& devListInfo();

    void initConfigCard(frmConfigCard* cardWidget);

private slots:
    void onCardAppend(const QVector<DevInfo>& devListInfo);
    void onCardRemove(const QVector<int> devIds);
    void onCardUpdate(const QVector<DevInfo>& devListInfo);

private:
    QVector<DevInfo> m_devList;
    frmConfigCard* m_cardWidget = nullptr;
};

#endif // DEVMANAGER_H
