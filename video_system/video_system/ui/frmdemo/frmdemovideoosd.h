#ifndef FRMDEMOVIDEOOSD_H
#define FRMDEMOVIDEOOSD_H

#include <QWidget>

namespace Ui {
class frmDemoVideoOsd;
}

class frmDemoVideoOsd : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoOsd(QWidget *parent = 0);
    ~frmDemoVideoOsd();

private:
    Ui::frmDemoVideoOsd *ui;

    //文字大小
    int fontSize;

private slots:
    //初始化窗体数据
    void initForm();

    //视频实时码率
    void receiveKbps(qreal kbps, int frameRate);
    //播放成功
    void receivePlayStart(int time);

private slots:
    void on_btnOpen_clicked();
    void on_btnUpdate_clicked();
};

#endif // FRMDEMOVIDEOOSD_H
