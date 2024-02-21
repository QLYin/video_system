#ifndef FRMDATAUSER_H
#define FRMDATAUSER_H

#include <QWidget>
#include <QModelIndex>

class DbPage;
class DbDelegate;

namespace Ui {
class frmDataUser;
}

class frmDataUser : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataUser(QWidget *parent = 0);
    ~frmDataUser();

private:
    Ui::frmDataUser *ui;

    //表格字段名称集合+宽度集合
    QList<QString> columnNames;
    QList<int> columnWidths;

    //构建条件语句sql+翻页类
    QString whereSql;
    DbPage *dbPage;

    //图片委托+委托集合
    DbDelegate *d_pixmap;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();

private slots:
    void on_btnSelect_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void dataout(quint8 type);
    void on_btnPrint_clicked();
    void on_btnPdf_clicked();
    void on_btnXls_clicked();    

    void on_ckTimeStart_stateChanged(int arg1);
    void on_ckTimeEnd_stateChanged(int arg1);
    void on_tableView_doubleClicked(const QModelIndex &index);
};

#endif // FRMDATAUSER_H
