#ifndef FRMMAIN1_H
#define FRMMAIN1_H

#include <QWidget>

namespace Ui {
class frmMain1;
}

class frmMain1 : public QWidget
{
    Q_OBJECT

public:
    explicit frmMain1(QWidget *parent = 0);
    ~frmMain1();

private:
    Ui::frmMain1 *ui;

private slots:
    void initForm();
};

#endif // FRMMAIN1_H
