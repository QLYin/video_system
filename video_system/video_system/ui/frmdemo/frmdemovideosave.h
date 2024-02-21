#ifndef FRMDEMOVIDEOSAVE_H
#define FRMDEMOVIDEOSAVE_H

#include <QWidget>

namespace Ui {
class frmDemoVideoSave;
}

class frmDemoVideoSave : public QWidget
{
    Q_OBJECT

public:
    explicit frmDemoVideoSave(QWidget *parent = 0);
    ~frmDemoVideoSave();

private:
    Ui::frmDemoVideoSave *ui;

private slots:
    //初始化窗体数据
    void initForm();

private slots:
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
};

#endif // FRMDEMOVIDEOSAVE_H
