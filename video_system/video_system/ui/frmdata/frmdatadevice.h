#ifndef FRMDATADEVICE_H
#define FRMDATADEVICE_H

#include <QWidget>

class DbDelegate;

namespace Ui {
class frmDataDevice;
}

class frmDataDevice : public QWidget
{
    Q_OBJECT

public:
    explicit frmDataDevice(QWidget *parent = 0);
    ~frmDataDevice();

private:
    Ui::frmDataDevice *ui;

    //列名+列宽
    QList<QString> columnNames;
    QList<int> columnWidths;

    //自定义委托集合
    DbDelegate *d_pixmap;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();
    //初始化数据
    void initData();
    //加载数据
    void loadData();

private slots:
    void on_btnSelect_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void dataout(quint8 type);
    void on_btnPrint_clicked();
    void on_btnPdf_clicked();
    void on_btnXls_clicked();    
};

#endif // FRMDATADEVICE_H
