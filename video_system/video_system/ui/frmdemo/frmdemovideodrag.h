#ifndef FRMDEMOVIDEODRAG_H
#define FRMDEMOVIDEODRAG_H

#include <QWidget>

namespace Ui {
class frmDemoVideoDrag;
}

class frmDemoVideoDrag : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoDrag(QWidget *parent = 0);
    ~frmDemoVideoDrag();

private:
    Ui::frmDemoVideoDrag *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //接收到拖曳文件
    void fileDrag(const QString &url);

private slots:
    void on_btnOpen_clicked();
    void on_btnPause_clicked();
};

#endif // FRMDEMOVIDEODRAG_H
