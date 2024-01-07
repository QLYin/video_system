#include "frmtimecpu.h"
#include "ui_frmtimecpu.h"
#include "qthelper.h"

frmTimeCpu::frmTimeCpu(QWidget *parent) : QWidget(parent), ui(new Ui::frmTimeCpu)
{
    ui->setupUi(this);
    this->initForm();
}

frmTimeCpu::~frmTimeCpu()
{
    delete ui;
}

void frmTimeCpu::initForm()
{
    ui->lcdNumber->setFormat("HH:mm:ss");
    ui->lcdNumber->setStyleSheet("border:none;");

    //macos上暂时获取不到cpu和内存 要换其他方式
#ifndef Q_OS_MAC
    //CPU和内存占用 默认3秒钟查询一次,可以自己更改成想要的查询速度
    connect(ui->labCpu, SIGNAL(valueChanged(quint64, quint64, quint64, quint64, quint64)),
            this, SLOT(valueChanged(quint64, quint64, quint64, quint64, quint64)));
    ui->labCpu->setShowText(false);
    ui->labCpu->start(3000);
#else
    ui->labCpu->setVisible(false);
#endif
}

void frmTimeCpu::valueChanged(quint64 cpuPercent, quint64 memoryPercent,
                                    quint64 memoryAll, quint64 memoryUse, quint64 memoryFree)
{
    QString text = QString("CPU %1%  Mem %2%").arg(cpuPercent).arg(memoryPercent);
    ui->labCpu->setText(text);
}
