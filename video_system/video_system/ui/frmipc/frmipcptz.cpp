#include "frmipcptz.h"
#include "ui_frmipcptz.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "deviceonvif.h"

frmIpcPtz::frmIpcPtz(QWidget *parent) : QWidget(parent), ui(new Ui::frmIpcPtz)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
}

frmIpcPtz::~frmIpcPtz()
{
    delete ui;
}

bool frmIpcPtz::eventFilter(QObject *watched, QEvent *event)
{
    //鼠标按下指定松开自动停止
    if (event->type() == QEvent::MouseButtonPress) {
        int position = 255;
        if (watched == ui->btnPtzZoomIn) {
            position = 9;
        } else if (watched == ui->btnPtzZoomOut) {
            position = 10;
        } else if (watched == ui->btnPtzFocusIn) {
            position = 11;
        } else if (watched == ui->btnPtzFocusOut) {
            position = 12;
        } else if (watched == ui->btnPtzApertureIn) {
            position = 13;
        } else if (watched == ui->btnPtzApertureOut) {
            position = 14;
        }

        QMetaObject::invokeMethod(this, "ptzControl", Qt::QueuedConnection, Q_ARG(int, position));
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMetaObject::invokeMethod(this, "ptzControl", Qt::QueuedConnection, Q_ARG(int, 255));
    }

    return QWidget::eventFilter(watched, event);
}

void frmIpcPtz::initForm()
{
    //安装事件过滤器识别松开自动停止
    ui->btnPtzZoomIn->installEventFilter(this);
    ui->btnPtzZoomOut->installEventFilter(this);

    ui->cboxPtzType->setCurrentIndex(2);
    ui->labPtzType->setVisible(AppConfig::VisiblePtzStep);
    ui->cboxPtzType->setVisible(AppConfig::VisiblePtzStep);

    //绑定云台仪表盘按下和松开事件
    ui->gaugeCloud->setAutoRepeat(false);
    connect(ui->gaugeCloud, SIGNAL(mousePressed(int, QString)), this, SLOT(mousePressed(int, QString)));
    connect(ui->gaugeCloud, SIGNAL(mouseReleased(int, QString)), this, SLOT(mouseReleased(int, QString)));

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmIpcPtz::initIcon()
{
    //设置图形字体
    int fontSize = 15;
    IconHelper::setIcon(ui->btnPtzZoomIn, 0xf067, fontSize);
    IconHelper::setIcon(ui->btnPtzZoomOut, 0xf068, fontSize);
    IconHelper::setIcon(ui->btnPtzFocusIn, 0xf067, fontSize);
    IconHelper::setIcon(ui->btnPtzFocusOut, 0xf068, fontSize);
    IconHelper::setIcon(ui->btnPtzApertureIn, 0xf067, fontSize);
    IconHelper::setIcon(ui->btnPtzApertureOut, 0xf068, fontSize);
}

void frmIpcPtz::mousePressed(int position, const QString &strPosition)
{
    DeviceHelper::addMsg(QString("按下云台 %1").arg(strPosition));
    QMetaObject::invokeMethod(this, "ptzControl", Qt::QueuedConnection, Q_ARG(int, position));
}

void frmIpcPtz::mouseReleased(int position, const QString &strPosition)
{
    DeviceHelper::addMsg(QString("松开云台 %1").arg(strPosition));
    QMetaObject::invokeMethod(this, "ptzControl", Qt::QueuedConnection, Q_ARG(int, 255));
}

void frmIpcPtz::ptzControl(int position)
{
    //移动类型
    int type = ui->cboxPtzType->currentIndex() + 1;
    //计算速度,转为小数
    double step = (double)ui->sliderPtzStep->value() / 10;

    //根据按下的不同部位发送云台控制命令
    //1. x、y、z 范围都在0-1之间。
    //2. x为负数，表示左转，x为正数，表示右转。
    //3. y为负数，表示下转，y为正数，表示上转。
    //4. z为正数，表示拉近，z为负数，表示拉远。
    //5. 通过x和y的组合，来实现云台的控制。
    //6. 通过z的组合，来实现焦距控制。

    //0-8依次表示底部/左下角/左侧/左上角/顶部/右上角/右侧/右下角/中间
    if (position == 0) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, 0.0, -step, 0.0);
    } else if (position == 1) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, -step, -step, 0.0);
    } else if (position == 2) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, -step, 0.0, 0.0);
    } else if (position == 3) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, -step, step, 0.0);
    } else if (position == 4) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, 0.0, step, 0.0);
    } else if (position == 5) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, step, step, 0.0);
    } else if (position == 6) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, step, 0.0, 0.0);
    } else if (position == 7) {
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, step, -step, 0.0);
    } else if (position == 8) {
        //先停止再复位
        DeviceOnvif::ptzControl(0, AppData::CurrentUrl, 0.0, 0.0, 0.0);
        //DeviceOnvif::ptzControl(1, AppData::CurrentUrl, 0.0, 0.0, 0.0);
    } else if (position == 9) {
        //变倍+
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, 0.0, 0.0, step);
    } else if (position == 10) {
        //变倍-
        DeviceOnvif::ptzControl(type, AppData::CurrentUrl, 0.0, 0.0, -step);
    } else if (position == 11) {
        //变焦+
    } else if (position == 12) {
        //变焦-
    } else if (position == 13) {
        //光圈+
    } else if (position == 14) {
        //光圈-
    } else {
        //停止
        DeviceOnvif::ptzControl(0, AppData::CurrentUrl, 0.0, 0.0, 0.0);
    }
}
