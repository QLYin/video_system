#ifndef FRMCONFIGCARD_H
#define FRMCONFIGCARD_H

#include <QWidget>
#include "class/deviceconnect/tcpcmddef.h"

namespace Ui {
class frmConfigCard;
}

class frmConfigCard : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigCard(QWidget *parent = 0);
    ~frmConfigCard();

    void updateTableWidget(const QVector<DevInfo>& deviceInfo);

signals:
    void cardAppendsig(const QVector<DevInfo>& deviceInfo);
    void cardRemoveSig(const QVector<int> devIds);
    void cardUpdateSig(const QVector<DevInfo>& deviceInfo);

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigCard*ui;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();
    void onBtnSearchClicked();
    void onBtnScreenCharClicked();
    void onBtnResolutionClicked();
};

#endif // FRMCONFIGIPC_H
