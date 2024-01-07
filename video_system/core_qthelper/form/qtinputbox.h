#ifndef QTINPUTBOX_H
#define QTINPUTBOX_H

#include <QDialog>
#include "singleton.h"

namespace Ui {
class QtInputBox;
}

class QtInputBox : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtInputBox)

public:
    explicit QtInputBox(QWidget *parent = 0);
    ~QtInputBox();

protected:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);

private:
    Ui::QtInputBox *ui;

    //超时关闭时间
    int timeout;
    //当前已显示过的时间
    int currentSec;
    //当前值
    QString value;

private slots:
    //初始化窗体数据
    void initForm();
    //倒计时关闭
    void countDown();

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

public:
    //获取输入的值
    QString getValue() const;

    //设置输入对话框的各种参数
    void setParameter(const QString &info, int type = 0, int timeout = 0,
                      QString placeholderText = QString(), bool pwd = false,
                      const QString &defaultValue = QString());
};

#endif // QTINPUTBOX_H
