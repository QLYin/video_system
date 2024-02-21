#ifndef FRMCONFIGUSER_H
#define FRMCONFIGUSER_H

#include <QWidget>

class QSqlTableModel;
class DbDelegate;

namespace Ui {
class frmConfigUser;
}

class frmConfigUser : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigUser(QWidget *parent = 0);
    ~frmConfigUser();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigUser *ui;

    //数据库表模型+列名+列宽
    QSqlTableModel *model;
    QList<QString> columnNames;
    QList<int> columnWidths;

    //对应权限委托 宽度+类型+值
    bool isLoad;
    QList<int> delegateWidth;
    QList<QString> delegateType;
    QList<QStringList> delegateValue;

public:
    //设置委托对应参数
    void setDelegate(const QList<int> &delegateWidth,
                     const QList<QString> &delegateType,
                     const QList<QStringList> &delegateValue);

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

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
};

#endif // FRMCONFIGUSER_H
