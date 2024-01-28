#ifndef FRMCONFIGCARD_H
#define FRMCONFIGCARD_H

#include <QWidget>
#include "class/deviceconnect/cmdhandlermgr.h"
#include "class/deviceconnect/tcpcmddef.h"

namespace Ui {
class frmConfigCard;
}

class frmConfigCard : public QWidget, public IHandler
{
    Q_OBJECT

public:
    explicit frmConfigCard(QWidget *parent = 0);
    ~frmConfigCard();

    void handle(const QVariantMap& data);
    const QVector<DevInfo>& devListInfo();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigCard*ui;
    QVector<DevInfo> m_devList;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

    void updateTableWidget();
private slots:
    void onBtnSearchClicked();
    void onBtnScreenCharClicked();
    void onBtnResolutionClicked();
};

#endif // FRMCONFIGIPC_H
