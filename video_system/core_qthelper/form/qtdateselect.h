#ifndef QTDATESELECT_H
#define QTDATESELECT_H

#include <QDialog>
#include "singleton.h"

namespace Ui {
class QtDateSelect;
}

class QtDateSelect : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtDateSelect)

public:
    explicit QtDateSelect(QWidget *parent = 0);
    ~QtDateSelect();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::QtDateSelect *ui;
    QString format;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

public:
    //获取当前选择的开始时间和结束时间
    QString getDateTimeStart()  const;
    QString getDateTimeEnd()    const;

    //设置日期格式
    void setFormat(const QString &format);
};

#endif // QTDATESELECT_H
