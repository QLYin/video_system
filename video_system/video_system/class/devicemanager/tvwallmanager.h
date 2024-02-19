﻿#ifndef TVWALLMANAGER_H
#define TVWALLMANAGER_H

#include "singleton.h"

#include "../deviceconnect/cmdhandlermgr.h"
#include "../deviceconnect/tcpcmddef.h"
#include "../frmtvwall/frmscreen.h"

class frmTVWallWidget;
class TVWallManager : public QObject, public IHandler
{
    Q_OBJECT SINGLETON_DECL(TVWallManager)

public:
    explicit TVWallManager(QObject *parent = 0);
    void initWallWidget(frmTVWallWidget* widget);
    void handle(const QVariantMap& data) override;

    bool hasMergeScreen();
    void sendWallJoint(int row, int col);
private:
    int calculatePlanMode(const QVector<int>& arr);
    frmScreen* findScreen(int devId);

private slots:
    void onWallSet();
    void onWallScreenJoin(QVector<ScreenInfo> infos, QVector<int> indexs, bool join);
    void onWallScreenCut(int row, int col, int splitNum);
    void onWallCallVideo(int row, int col, int index, QString ip);
private:
    frmTVWallWidget* m_wallWidget = nullptr;
};

#endif // TVWALLMANAGER_H