#ifndef FRMMAPWEB_H
#define FRMMAPWEB_H

#include <QWidget>

namespace Ui {
class frmMapWeb;
}

class frmMapWeb : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapWeb(QWidget *parent = 0);
    ~frmMapWeb();   

private:
    Ui::frmMapWeb *ui;

private slots:
    //初始化窗体数据
    void initForm();
};

#endif // FRMMAPWEB_H
