#include "qtdateselect.h"
#include "ui_qtdateselect.h"
#include "qthelper.h"

SINGLETON_IMPL(QtDateSelect)
QtDateSelect::QtDateSelect(QWidget *parent) : QDialog(parent), ui(new Ui::QtDateSelect)
{
    ui->setupUi(this);
    this->initForm();
}

QtDateSelect::~QtDateSelect()
{
    delete ui;
}

void QtDateSelect::showEvent(QShowEvent *)
{
    //设置按钮图标
    QtHelper::setIconBtn(ui->btnOk, ":/image/btn_ok.png", 0xf00c);
    QtHelper::setIconBtn(ui->btnCancel, ":/image/btn_close.png", 0xf00d);
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);
    //居中显示
    QtHelper::setFormInCenter(this);
    //激活窗体
    this->activateWindow();
}

void QtDateSelect::initForm()
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

    //设置默认日期当前日期
    QtHelper::initDataTimeEdit(ui->dateTimeStart, 0);
    QtHelper::initDataTimeEdit(ui->dateTimeEnd, 1);
    this->setFormat("yyyy-MM-dd");
    ui->btnOk->setFocus();
}

void QtDateSelect::on_btnOk_clicked()
{
    //过滤非法时间范围
    QDateTime dateStart = ui->dateTimeStart->dateTime();
    QDateTime dateEnd = ui->dateTimeEnd->dateTime();
    if (dateStart > dateEnd) {
        QtHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
        return;
    }

    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::Ok);
    this->close();
}

void QtDateSelect::on_btnCancel_clicked()
{
    //返回对话框执行结果并关闭窗体
    this->done(QMessageBox::Cancel);
    this->close();
}

QString QtDateSelect::getDateTimeStart() const
{
    return ui->dateTimeStart->dateTime().toString(format);
}

QString QtDateSelect::getDateTimeEnd() const
{
    return ui->dateTimeEnd->dateTime().toString(format);
}

void QtDateSelect::setFormat(const QString &format)
{
    this->format = format;
    ui->dateTimeStart->setDisplayFormat(format);
    ui->dateTimeEnd->setDisplayFormat(format);
}
