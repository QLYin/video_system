#include "qttipbox.h"
#include "ui_qttipbox.h"
#include "qthelper.h"

SINGLETON_IMPL(QtTipBox)
QtTipBox::QtTipBox(QWidget *parent) : QDialog(parent), ui(new Ui::QtTipBox)
{
    ui->setupUi(this);
    this->initForm();
}

QtTipBox::~QtTipBox()
{
    delete ui;
}

void QtTipBox::closeEvent(QCloseEvent *)
{
    timeout = 0;
    currentSec = 0;
}

void QtTipBox::initForm()
{
    //设置阴影
    QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框
    QtHelper::setFramelessForm(this, ui->QtFormTitle, ui->QtLabIco, ui->btnMenu_Close);
    //设置关闭按钮单击关闭窗体
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(close()));
    //设置标题
    this->setWindowTitle(ui->QtLabTitle->text());

    //字体加大
    QFont font;
    font.setPixelSize(GlobalConfig::FontSize + 3);
    font.setBold(true);
    ui->QtLabInfo->setFont(font);

    //显示和隐藏窗体动画效果
    animation = new QPropertyAnimation(this, "pos", this);
    connect(animation, SIGNAL(finished()), this, SLOT(finished()));
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(500);

    timeout = 0;
    currentSec = 0;
    formSize = QSize(QtDialogMinWidth, QtDialogMinHeight);

    //关闭倒计时定时器
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(countDown()));
    timer->start();
}

void QtTipBox::countDown()
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

void QtTipBox::finished()
{
    if (animation->property("hide").toBool()) {
        this->setVisible(false);
    }
}

void QtTipBox::setTip(const QString &title, const QString &tip, bool fullScreen, bool center, int timeout)
{
    //设置倒计时时间
    this->timeout = timeout;
    this->currentSec = 0;
    ui->QtLabCountDown->clear();
    this->countDown();

    //设置标题及提示信息内容
    this->fullScreen = fullScreen;
    ui->QtLabTitle->setText(title);
    ui->QtLabInfo->setText(tip);
    ui->QtLabInfo->setAlignment(center ? Qt::AlignCenter : Qt::AlignLeft);
    this->setWindowTitle(ui->QtLabTitle->text());
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);

    //合适尺寸再加点会显得更大气
    QSize newSize = sizeHint() + QSize(20, 20);
    //矫正最小尺寸
    if (newSize.width() < QtDialogMinWidth) {
        newSize.setWidth(QtDialogMinWidth);
    }
    if (newSize.height() < QtDialogMinHeight) {
        newSize.setHeight(QtDialogMinHeight);
    }

    //重新设置下尺寸才能真正刷新改变后的颜色,尤其是Qt6
    if (newSize == formSize) {
        formSize = newSize + QSize(1, 0);
    } else {
        formSize = newSize;
    }

    this->setFixedSize(formSize);

    //计算屏幕尺寸
    QRect rect = QtHelper::getScreenRect(!fullScreen);
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width() + rect.x();
    int y = height - this->height() + rect.y();

    //移到右下角
    this->move(x, y);

    //启动动画
    animation->setProperty("hide", false);
    animation->stop();
    animation->setStartValue(QPoint(x, height + rect.y()));
    animation->setEndValue(QPoint(x, y));
    animation->start();
}

void QtTipBox::hide()
{
    QRect rect = QtHelper::getScreenRect(!fullScreen);
    int width = rect.width();
    int height = rect.height();
    int x = width - this->width() + rect.x();
    int y = height - this->height() + rect.y();

    //启动动画
    animation->setProperty("hide", true);
    animation->stop();
    animation->setStartValue(QPoint(x, y));
    animation->setEndValue(QPoint(x, height + rect.y()));
    animation->start();
}
