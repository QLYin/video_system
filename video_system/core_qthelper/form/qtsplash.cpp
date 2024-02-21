#include "qtsplash.h"
#include "ui_qtsplash.h"
#include "qthelper.h"

SINGLETON_IMPL(QtSplash)
QtSplash::QtSplash(QWidget *parent) : QDialog(parent), ui(new Ui::QtSplash)
{
    ui->setupUi(this);
    this->initForm();
}

QtSplash::~QtSplash()
{
    delete ui;
}

void QtSplash::showEvent(QShowEvent *)
{
    //居中显示
    QtHelper::setFormInCenter(this);
}

void QtSplash::closeEvent(QCloseEvent *)
{
    timeout = 0;
    currentSec = 0;
}

void QtSplash::initForm()
{
    //设置阴影
    QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框置顶显示不显示在任务栏
    QtHelper::setFramelessForm(this, true, true, false);

    timeout = 0;
    currentSec = 0;
    formSize = QSize(520, 120);

    //关闭倒计时定时器
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(countDown()));
    timer->start();

    //设置显示的提示文字
    this->setInfo("请稍等, 正在疯狂努力加载中...", GlobalConfig::FontSize + 18, GlobalConfig::FontSize + 5);
}

void QtSplash::countDown()
{
    if (timeout == 0) {
        return;
    }

    if (currentSec < timeout) {
        currentSec++;
    } else {
        this->close();
    }

    QString text = QString("关闭倒计时 %1 s").arg(timeout - currentSec + 1);
    ui->QtLabCountDown->setText(text);
}

void QtSplash::setInfo(const QString &info, int fontSizeMain, int fontSizeSub, int timeout)
{
    //设置倒计时时间
    this->timeout = timeout;
    this->currentSec = 0;
    ui->QtLabCountDown->clear();
    ui->QtLabCountDown->setVisible(timeout > 0);
    this->countDown();

    //设置下字体
    QFont font;
    font.setPixelSize(fontSizeMain);
    ui->QtLabInfo->setFont(font);
    font.setPixelSize(fontSizeSub);
    ui->QtLabCountDown->setFont(font);
    //设置提示信息文本
    ui->QtLabInfo->setText(info);

    //合适尺寸再加点会显得更大气
    QSize newSize = sizeHint() + QSize(50, 20);
    //矫正最小尺寸
    if (newSize.width() < 520) {
        newSize.setWidth(520);
    }
    if (newSize.height() < 120) {
        newSize.setHeight(120);
    }

    //重新设置下尺寸才能真正刷新改变后的颜色,尤其是Qt6
    if (newSize == formSize) {
        formSize = newSize + QSize(1, 0);
    } else {
        formSize = newSize;
    }

    this->setFixedSize(formSize);
}
