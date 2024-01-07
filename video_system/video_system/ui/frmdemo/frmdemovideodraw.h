#ifndef FRMDEMOVIDEODRAW_H
#define FRMDEMOVIDEODRAW_H

#include <QWidget>

namespace Ui {
class frmDemoVideoDraw;
}

class frmDemoVideoDraw : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoDraw(QWidget *parent = 0);
    ~frmDemoVideoDraw();

private:
    Ui::frmDemoVideoDraw *ui;

    //标签索引
    int osdIndex;
    //图形索引
    int graphIndex;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOpen_clicked();
    void on_btnOsd_clicked();
    void on_btnGraph_clicked();
};

#endif // FRMDEMOVIDEODRAW_H
