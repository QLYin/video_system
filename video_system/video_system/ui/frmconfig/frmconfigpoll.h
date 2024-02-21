#ifndef FRMCONFIGPOLL_H
#define FRMCONFIGPOLL_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;
class QListWidgetItem;

namespace Ui {
class frmConfigPoll;
}

class frmConfigPoll : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigPoll(QWidget *parent = 0);
    ~frmConfigPoll();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigPoll *ui;

    //数据库表模型+列名+列宽
    QSqlTableModel *model;
    QList<QString> columnNames;
    QList<int> columnWidths;

    //自定义委托集合
    DbDelegate *d_cbox_pollGroup;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

private slots:
    //轮询分组变化
    void pollGroupChanged();
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
};

#endif // FRMCONFIGPOLL_H
