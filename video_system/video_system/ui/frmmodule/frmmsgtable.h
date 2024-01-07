#ifndef FRMMSGTABLE_H
#define FRMMSGTABLE_H

#include <QWidget>

namespace Ui {
class frmMsgTable;
}

class frmMsgTable : public QWidget
{
    Q_OBJECT

public:
    explicit frmMsgTable(QWidget *parent = 0);
    ~frmMsgTable();

protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *);

private:
    Ui::frmMsgTable *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化右键菜单
    void initAction();
    //执行右键菜单
    void doAction();
};

#endif // FRMMSGTABLE_H
