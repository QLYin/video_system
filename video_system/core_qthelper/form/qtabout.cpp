#include "qtabout.h"
#include "ui_qtabout.h"
#include "qthelper.h"

SINGLETON_IMPL(QtAbout)
QtAbout::QtAbout(QWidget *parent) : QDialog(parent), ui(new Ui::QtAbout)
{
    ui->setupUi(this);
    this->initForm();
}

QtAbout::~QtAbout()
{
    delete ui;
}

void QtAbout::showEvent(QShowEvent *)
{
    QtHelper::setFormInCenter(this);
}

void QtAbout::initForm()
{
    //设置阴影
    QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框置顶显示不显示在任务栏
    QtHelper::setFramelessForm(this, true, true, false);
    //设置关闭自动隐藏
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(hide()));
    //重新设置不支持拖动
    this->setProperty("canMove", false);

    //设置不同的图标
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 5);
    IconHelper::setIcon(ui->btnMenu_Close, GlobalConfig::IconClose, GlobalConfig::FontSize + 3);

    //启动定时器关闭界面
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(hide()));

    //设置一个默认值的信息
    this->setInfo(AboutInfo());
}

void QtAbout::setInfo(const AboutInfo &info, int timeout)
{
    //设置标题
    ui->QtLabName->setText(info.title);
    ui->QtLabTitle->setText(QString("关于%1").arg(info.title));
    this->setWindowTitle(ui->QtLabTitle->text());
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);

    //设置html内容
    QStringList list;
    list << QString("<html><body>");
    list << QString("<p>版本: %1</p>").arg(info.version);
    list << QString("<p>版权: %1</p>").arg(info.copyright);
    list << QString("<p>电话: %1</p>").arg(info.tel);
    list << QString("<p>网址: <a href=\"%1\"><span style=\"text-decoration:none;color:#ffffff;\">%1</a></p>").arg(info.http);
    list << QString("</body></html>");
    ui->QtLabInfo->setText(list.join("\n"));
    //设置可以打开超链接
    ui->QtLabInfo->setOpenExternalLinks(true);

    //自适应内容以便留出更多的空间拖动
    //ui->QtLabInfo->adjustSize();
    //下面的方法更合适,只需要宽度留出空间就行
    QSize size = ui->QtLabInfo->sizeHint();
    ui->QtLabInfo->resize(size.width(), ui->QtLabInfo->height());

    //设置logo图片
    QtHelper::setPixmap(ui->QtLabImage, info.logo);

    //启动定时器关闭窗体
    timer->stop();
    if (timeout > 0) {
        timer->start(timeout * 1000);
    }

    //重新设置下尺寸才能真正刷新改变后的颜色,尤其是Qt6
    if (this->width() == 720) {
        this->setFixedSize(721, 300);
    } else {
        this->setFixedSize(720, 300);
    }
}
