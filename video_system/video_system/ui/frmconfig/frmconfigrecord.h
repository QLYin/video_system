#ifndef FRMCONFIGRECORD_H
#define FRMCONFIGRECORD_H

#include <QWidget>
class QSqlTableModel;

namespace Ui {
class frmConfigRecord;
}

class frmConfigRecord : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigRecord(QWidget *parent = 0);
    ~frmConfigRecord();

private:
    Ui::frmConfigRecord *ui;

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
    void on_btnPlus_clicked();

    void on_tableView_pressed();
    void on_taskTableView_selectChanged();
};

#endif // FRMCONFIGRECORD_H
