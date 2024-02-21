#ifndef QTSPLASH_H
#define QTSPLASH_H

#include <QDialog>
#include "singleton.h"

namespace Ui {
class QtSplash;
}

class QtSplash : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtSplash)

public:
    explicit QtSplash(QWidget *parent = 0);
    ~QtSplash();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::QtSplash *ui;

    //超时关闭时间
    int timeout;
    //当前已显示过的时间
    int currentSec;
    //记住窗体的尺寸
    QSize formSize;

private slots:
    //初始化窗体数据
    void initForm();
    //倒计时关闭
    void countDown();

public:
    //设置提示信息
    void setInfo(const QString &info, int fontSizeMain, int fontSizeSub, int timeout = 0);
};

#endif // QTSPLASH_H
