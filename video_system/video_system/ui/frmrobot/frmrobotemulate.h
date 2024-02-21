#ifndef FRMROBOTEMULATE_H
#define FRMROBOTEMULATE_H

#include <QWidget>

namespace Ui {
class frmRobotEmulate;
}

class frmRobotEmulate : public QWidget
{
    Q_OBJECT

public:
    explicit frmRobotEmulate(QWidget *parent = 0);
    ~frmRobotEmulate();

private:
    Ui::frmRobotEmulate *ui;
};

#endif // FRMROBOTEMULATE_H
