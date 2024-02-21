#include "qtmessagebox.h"
#include "ui_qtmessagebox.h"
#include "qthelper.h"

SINGLETON_IMPL(QtMessageBox)
QtMessageBox::QtMessageBox(QWidget *parent) : QDialog(parent), ui(new Ui::QtMessageBox)
{
    ui->setupUi(this);
    this->initForm();
}

QtMessageBox::~QtMessageBox()
{
    delete ui;
}

void QtMessageBox::showEvent(QShowEvent *)
{
    //设置按钮图标
    QtHelper::setIconBtn(ui->btnOk, ":/image/btn_ok.png", 0xf00c);
    QtHelper::setIconBtn(ui->btnCancel, ":/image/btn_close.png", 0xf00d);
    //居中显示
    QtHelper::setFormInCenter(this);
    //激活窗体
    this->activateWindow();
}

void QtMessageBox::closeEvent(QCloseEvent *)
{
    timeout = 0;
    currentSec = 0;
}

void QtMessageBox::initForm()
{
    //设置阴影
    QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框
    QtHelper::setFramelessForm(this, ui->QtFormTitle, ui->QtLabIco, ui->btnMenu_Close);
    //设置关闭按钮单击关闭窗体
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnCancel_clicked()));
    //设置标题
    this->setWindowTitle(ui->QtLabTitle->text());

    //设置图标标签尺寸
    ui->QtLabIconMain->setFixedSize(QtTitleMinSize, QtTitleMinSize);

    //按钮设置最小尺寸和图标大小
    QList<QPushButton *> btns = ui->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(QtBtnMinWidth);
        btn->setIconSize(QSize(QtIconWidth, QtIconHeight));
    }

    timeout = 0;
    currentSec = 0;
    formSize = QSize(QtDialogMinWidth, QtDialogMinHeight);

    //倒计时定时器关闭窗体    
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(countDown()));
    timer->start();
}

void QtMessageBox::countDown()
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

void QtMessageBox::on_btnOk_clicked()
{
    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::Yes);
    this->close();
}

void QtMessageBox::on_btnCancel_clicked()
{
    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::No);
    this->close();
}

void QtMessageBox::setIconMsg(const QString &png, int icon)
{
    //图片存在则取图片,不存在则取图形字体
    int size = ui->QtLabIconMain->size().height();
    if (QImage(png).isNull()) {
        IconHelper::setIcon(ui->QtLabIconMain, icon, size);
    } else {
        ui->QtLabIconMain->setStyleSheet(QString("border-image:url(%1);").arg(png));
    }
}

void QtMessageBox::setMessage(const QString &msg, int type, int timeout)
{
    //设置倒计时时间
    this->timeout = timeout;
    this->currentSec = 0;
    ui->QtLabCountDown->clear();
    this->countDown();

    //不同的消息类型设置不同的图标
    if (type == 0) {
        setIconMsg(":/image/msg_info.png", 0xf05a);
        ui->btnCancel->setVisible(false);
        ui->QtLabTitle->setText("提示");
    } else if (type == 1) {
        setIconMsg(":/image/msg_question.png", 0xf059);
        ui->QtLabTitle->setText("询问");
    } else if (type == 2) {
        setIconMsg(":/image/msg_error.png", 0xf057);
        ui->btnCancel->setVisible(false);
        ui->QtLabTitle->setText("错误");
    }

    //设置消息内容
    ui->QtLabInfo->setText(msg);
    this->setWindowTitle(ui->QtLabTitle->text());
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);

    //合适尺寸再加点会显得更大气
    QSize newSize = ui->QtLabInfo->sizeHint() + QSize(100, 120);
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
}
