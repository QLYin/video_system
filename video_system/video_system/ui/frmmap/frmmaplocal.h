#ifndef FRMMAPLOCAL_H
#define FRMMAPLOCAL_H

#include <QWidget>

namespace Ui {
class frmMapLocal;
}

class frmMapLocal : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapLocal(QWidget *parent = 0);
    ~frmMapLocal();

private:
    Ui::frmMapLocal *ui;

private slots:
    //初始化窗体数据
    void initForm();
};

#endif // FRMMAPLOCAL_H
