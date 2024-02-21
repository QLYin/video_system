#ifndef QTTIPBOX_H
#define QTTIPBOX_H

#include <QDialog>
#include "singleton.h"
class QPropertyAnimation;

namespace Ui {
class QtTipBox;
}

class QtTipBox : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtTipBox)

public:
    explicit QtTipBox(QWidget *parent = 0);
    ~QtTipBox();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::QtTipBox *ui;

    //超时关闭时间
    int timeout;
    //当前已显示过的时间
    int currentSec;
    //记住窗体的尺寸
    QSize formSize;

    //是否全屏
    bool fullScreen;
    //窗体切换动画
    QPropertyAnimation *animation;

private slots:
    //初始化窗体数据
    void initForm();
    //倒计时关闭
    void countDown();
    //动画完成
    void finished();

public:
    //设置提示信息
    void setTip(const QString &title, const QString &tip, bool fullScreen = false, bool center = true, int timeout = 0);
    //隐藏界面
    void hide();
};

#endif // QTTIPBOX_H
