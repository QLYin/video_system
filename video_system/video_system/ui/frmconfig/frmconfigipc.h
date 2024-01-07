#ifndef FRMCONFIGIPC_H
#define FRMCONFIGIPC_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigIpc;
}

class frmConfigIpc : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigIpc(QWidget *parent = 0);
    ~frmConfigIpc();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigIpc *ui;

    //数据库表模型+列名+列宽
    QSqlTableModel *model;
    QList<QString> columnNames;
    QList<int> columnWidths;

    //自定义委托集合
    DbDelegate *d_cbox_nvrName;
    DbDelegate *d_cbox_ipcImage;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

    //录像机名称改变和摄像机图片改变后需要重新更新委托数据
    void nvrNameChanged();
    void ipcImageChanged();

    //添加单个设备和批量添加设备
    void addDevice(const QStringList &deviceInfo);
    void addDevices(const QList<QStringList> &deviceInfos);
    //批量添加地址
    void addPlus(const QStringList &rtspMains, const QStringList &rtspSubs);

private slots:
    void on_btnAdd_clicked();
    void on_btnSave_clicked();
    void on_btnDelete_clicked();
    void on_btnReturn_clicked();
    void on_btnClear_clicked();

    void on_btnInput_clicked();
    void on_btnOutput_clicked();

    void dataout(quint8 type);
    void on_btnPrint_clicked();
    void on_btnXls_clicked();

    void on_btnPlus_clicked();
    void on_btnSearch_clicked();
};

#endif // FRMCONFIGIPC_H
