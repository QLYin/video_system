#ifndef FRMMAPIMAGE_H
#define FRMMAPIMAGE_H

#include <QWidget>

class DeviceButton;

namespace Ui {
class frmMapImage;
}

class frmMapImage : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapImage(QWidget *parent = 0);
    ~frmMapImage();

protected:
    void hideEvent(QHideEvent *);

private:
    Ui::frmMapImage *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //加载图片
    void initImage();

private slots:
    //按钮双击弹出视频预览
    void doubleClicked(DeviceButton *btn);

private slots:
    void on_listWidget_pressed();
};

#endif // FRMMAPIMAGE_H
