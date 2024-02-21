#ifndef FRMROBOTLOG_H
#define FRMROBOTLOG_H

#include <QWidget>

namespace Ui {
class frmRobotLog;
}

class frmRobotLog : public QWidget
{
    Q_OBJECT

public:
    explicit frmRobotLog(QWidget *parent = 0);
    ~frmRobotLog();

private:
    Ui::frmRobotLog *ui;
};

#endif // FRMROBOTLOG_H
