#ifndef QTMESSAGEBOX_H
#define QTMESSAGEBOX_H

#include <QDialog>
#include "singleton.h"

namespace Ui {
class QtMessageBox;
}

class QtMessageBox : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtMessageBox)

public:
    explicit QtMessageBox(QWidget *parent = 0);
    ~QtMessageBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::QtMessageBox *ui;

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

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

public:
    //设置消息图标 图形字体+图片两种方式
    void setIconMsg(const QString &png, int icon);
    //设置提示信息带类型和关闭倒计时
    void setMessage(const QString &msg, int type, int timeout = 0);
};

#endif // QTMESSAGEBOX_H
