#include "qtinputbox.h"
#include "ui_qtinputbox.h"
#include "qthelper.h"

SINGLETON_IMPL(QtInputBox)
QtInputBox::QtInputBox(QWidget *parent) : QDialog(parent), ui(new Ui::QtInputBox)
{
    ui->setupUi(this);
    this->initForm();
}

QtInputBox::~QtInputBox()
{
    delete ui;
}

void QtInputBox::showEvent(QShowEvent *)
{
    //设置按钮图标
    QtHelper::setIconBtn(ui->btnOk, ":/image/btn_ok.png", 0xf00c);
    QtHelper::setIconBtn(ui->btnCancel, ":/image/btn_close.png", 0xf00d);
    //居中显示
    QtHelper::setFormInCenter(this);
    //激活窗体
    this->activateWindow();
    //焦点设置到输入框
    ui->QtTxtValue->setFocus();
    value.clear();
}

void QtInputBox::closeEvent(QCloseEvent *)
{
    timeout = 0;
    currentSec = 0;
}

void QtInputBox::initForm()
{    
    //设置阴影
    QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框
    QtHelper::setFramelessForm(this, ui->QtFormTitle, ui->QtLabIco, ui->btnMenu_Close);
    //设置关闭按钮单击关闭窗体
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(on_btnCancel_clicked()));

    //设置标题和窗体大小
    this->setWindowTitle(ui->QtLabTitle->text());
    this->setFixedSize(QtDialogMinWidth, QtDialogMinHeight + 10);

    //按钮设置最小尺寸和图标大小
    QList<QPushButton *> btns  = ui->frame->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setMinimumWidth(QtBtnMinWidth);
        btn->setIconSize(QSize(QtIconWidth, QtIconHeight));
    }

    timeout = 0;
    currentSec = 0;

    //关闭倒计时定时器    
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(countDown()));
    timer->start();
}

void QtInputBox::countDown()
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

void QtInputBox::on_btnOk_clicked()
{
    if (ui->QtTxtValue->isVisible()) {
        value = ui->QtTxtValue->text();
    } else if (ui->QtCboxValue->isVisible()) {
        value = ui->QtCboxValue->currentText();
    }

    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::Ok);
    this->close();
}

void QtInputBox::on_btnCancel_clicked()
{
    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::Cancel);
    this->close();
}

QString QtInputBox::getValue() const
{
    return this->value;
}

void QtInputBox::setParameter(const QString &info, int type, int timeout,
                               QString placeholderText, bool pwd, const QString &defaultValue)
{

    //设置倒计时时间
    this->timeout = timeout;
    this->currentSec = 0;
    ui->QtLabCountDown->clear();
    this->countDown();

    ui->QtLabInfo->setText(info);
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);

    if (type == 0) {
        ui->QtCboxValue->setVisible(false);
        ui->QtTxtValue->setPlaceholderText(placeholderText);
        ui->QtTxtValue->setText(defaultValue);
        //密文显示
        if (pwd) {
            ui->QtTxtValue->setEchoMode(QLineEdit::Password);
        }
    } else if (type == 1) {
        ui->QtTxtValue->setVisible(false);
        ui->QtCboxValue->addItems(defaultValue.split("|"));
        //回显字符串作为默认的下拉选项
        if (!placeholderText.isEmpty()) {
            ui->QtCboxValue->setCurrentIndex(ui->QtCboxValue->findText(placeholderText));
        }
    }
}
