#ifndef FRMCONFIGNVR_H
#define FRMCONFIGNVR_H

#include <QWidget>
class QSqlTableModel;

namespace Ui {
class frmConfigNvr;
}

class frmConfigNvr : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigNvr(QWidget *parent = 0);
    ~frmConfigNvr();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigNvr *ui;

    //数据库表模型+列名+列宽
    QSqlTableModel *model;
    QList<QString> columnNames;
    QList<int> columnWidths;

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

signals:
    //录像机名称变化信号
    void nvrNameChanged();
};

#endif // FRMCONFIGNVR_H
