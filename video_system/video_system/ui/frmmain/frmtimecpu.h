#ifndef FRMTIMECPU_H
#define FRMTIMECPU_H

#include <QWidget>

namespace Ui {
class frmTimeCpu;
}

class frmTimeCpu : public QWidget
{
    Q_OBJECT

public:
    explicit frmTimeCpu(QWidget *parent = 0);
    ~frmTimeCpu();

private:
    Ui::frmTimeCpu *ui;

private slots:
    void initForm();
    void valueChanged(quint64 cpuPercent, quint64 memoryPercent,
                      quint64 memoryAll, quint64 memoryUse, quint64 memoryFree);

};

#endif // FRMTIMECPU_H
