#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QDialog>
#include "class/deviceconnect/tcpcmddef.h"

namespace Ui {
class frmSearchCard;
}

class frmSearchCard : public QDialog
{
    Q_OBJECT

public:
    explicit frmSearchCard(QWidget *parent = 0);
    ~frmSearchCard();

    void updateTableWidget(const QVector<DevInfo>& deviceInfo);
private:
    Ui::frmSearchCard*ui;
    QVector<DevInfo> m_searchCards;

public:
    //设置顶部横幅背景图片
    void setBanner(const QString &image);

private slots:
    //初始化无边框窗体
    void initStyle();
    //初始化标题
    void initTitle();
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();

private slots:
    void onAutoNetWorkClicked();
    void onBtnAddClicked();
    void onBtnSelectAllClicked();
    void onBtnSelectNoneClicked();
    void onBtnAutoNetClicked();
    void onBtnDetectConfilctClicked();
    void onBtnSetClicked();

public:
    Q_SIGNAL void cardUpdateSig(QVector<DevInfo> cards);
};

#endif // FRMLOGIN_H
