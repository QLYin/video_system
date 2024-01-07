#include "frmipccontrol.h"
#include "ui_frmipccontrol.h"
#include "frmipcosd.h"
#include "frmipcnetconfig.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "deviceonvif.h"
#include "onvifthread.h"

frmIpcControl::frmIpcControl(QWidget *parent) : QWidget(parent), ui(new Ui::frmIpcControl)
{
    ui->setupUi(this);
    this->initForm();
}

frmIpcControl::~frmIpcControl()
{
    delete ui;
}

void frmIpcControl::initForm()
{
    ipcOsd = new frmIpcOsd;
    ipcNetConfig = new frmIpcNetConfig;

    //关联指令执行结果
    connect(DeviceOnvif::Instance(), SIGNAL(receiveResult(QString, QString, QVariant)),
            this, SLOT(receiveResult(QString, QString, QVariant)));

    //循环处理懒得一个个写
    QList<QSlider *> sliders;
    sliders << ui->sliderBrightness << ui->sliderColorSaturation << ui->sliderContrast << ui->sliderSharpness;
    foreach (QSlider *slider, sliders) {
        //滑块拖动值变化进行文本框值设置
        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
        //滑块松开的时候立即设置图片参数
        connect(slider, SIGNAL(sliderReleased()), this, SLOT(on_btnSetImageSetting_clicked()));
        //设置滑块范围值
        slider->setRange(0, 255);
        //设置滑块初始值
        slider->setValue(125);
    }
}

void frmIpcControl::valueChanged(int value)
{
    QSlider *slider = (QSlider *)sender();
    QString text = QString::number(value);
    if (slider == ui->sliderBrightness) {
        ui->txtBrightness->setText(text);
    } else if (slider == ui->sliderColorSaturation) {
        ui->txtColorSaturation->setText(text);
    } else if (slider == ui->sliderContrast) {
        ui->txtContrast->setText(text);
    } else if (slider == ui->sliderSharpness) {
        ui->txtSharpness->setText(text);
    }
}

void frmIpcControl::receiveResult(const QString &url, const QString &cmd, const QVariant &data)
{
    //qDebug() << TIMEMS << url << cmd << data;
    if (cmd == "getImageSetting") {
        QVariantList list = data.toList();
        int brightness = list.at(0).toInt();
        int colorSaturation = list.at(1).toInt();
        int contrast = list.at(2).toInt();
        int sharpness = list.at(3).toInt();

        int brightnessMin = list.at(4).toInt();
        int brightnessMax = list.at(5).toInt();
        int colorSaturationMin = list.at(6).toInt();
        int colorSaturationMax = list.at(7).toInt();
        int contrastMin = list.at(8).toInt();
        int contrastMax = list.at(9).toInt();
        int sharpnessMin = list.at(10).toInt();
        int sharpnessMax = list.at(11).toInt();

        //设置对应滑块范围
        ui->sliderBrightness->setRange(brightnessMin, brightnessMax);
        ui->sliderColorSaturation->setRange(colorSaturationMin, colorSaturationMax);
        ui->sliderContrast->setRange(contrastMin, contrastMax);
        ui->sliderSharpness->setRange(sharpnessMin, sharpnessMax);

        //设置对应滑块的值
        ui->sliderBrightness->setValue(brightness);
        ui->sliderColorSaturation->setValue(colorSaturation);
        ui->sliderContrast->setValue(contrast);
        ui->sliderSharpness->setValue(sharpness);
    }
}

void frmIpcControl::on_btnGetImageSetting_clicked()
{
    OnvifDeviceData deviceData;
    if (DeviceOnvif::checkUrl(AppData::CurrentUrl, deviceData)) {
        QVariant data = (QList<QVariant>() << deviceData.videoSource);
        OnvifThread::Instance()->append(deviceData, "getImageSetting", data);
    }
}

void frmIpcControl::on_btnSetImageSetting_clicked()
{
    int brightness, colorSaturation, contrast, sharpness;
    brightness = ui->sliderBrightness->value();
    colorSaturation = ui->sliderColorSaturation->value();
    contrast = ui->sliderContrast->value();
    sharpness = ui->sliderSharpness->value();

    OnvifDeviceData deviceData;
    if (DeviceOnvif::checkUrl(AppData::CurrentUrl, deviceData)) {
        QVariant data = (QList<QVariant>() << deviceData.videoSource << brightness << colorSaturation << contrast << sharpness);
        OnvifThread::Instance()->append(deviceData, "setImageSetting", data);
    }
}

void frmIpcControl::on_btnSystemReboot_clicked()
{
    OnvifDeviceData deviceData;
    if (DeviceOnvif::checkUrl(AppData::CurrentUrl, deviceData)) {
        OnvifThread::Instance()->append(deviceData, "systemReboot");
    }
}

void frmIpcControl::on_btnSetDateTime_clicked()
{
    OnvifDeviceData deviceData;
    if (DeviceOnvif::checkUrl(AppData::CurrentUrl, deviceData)) {
        OnvifThread::Instance()->append(deviceData, "setDateTime");
    }
}

void frmIpcControl::on_btnSnapImage_clicked()
{
    OnvifDeviceData deviceData;
    if (DeviceOnvif::checkUrl(AppData::CurrentUrl, deviceData)) {
        QVariant data = (QList<QVariant>() << deviceData.profileToken);
        OnvifThread::Instance()->append(deviceData, "snapImage", data);
    }
}

void frmIpcControl::on_btnLoadVideo_clicked()
{
    AppEvent::Instance()->slot_restartVideo();
}

void frmIpcControl::on_btnTestAlarm_clicked()
{
    //找到第一个不为空的onvif地址
    QString url = "rtsp://192.168.1.200:554";
    foreach (QString addr, DbData::IpcInfo_OnvifAddr) {
        if (!addr.isEmpty()) {
            url = addr;
            break;
        }
    }

    OnvifEventInfo event;
    event.time = QString("%1T%2Z").arg(QDATE).arg(TIME);
    event.dataName = "LogicalState";
    event.dataValue = "1";
    QMetaObject::invokeMethod(OnvifThread::Instance(), "receiveEvent", Q_ARG(QString, url), Q_ARG(OnvifEventInfo, event));
}

void frmIpcControl::on_btnShowVideo_clicked()
{
    //真实应用场景是接收到报警信号后找到需要弹窗的视频地址
    QString url = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
    DeviceHelper::showVideo(url);
}

void frmIpcControl::on_btnIpcOsd_clicked()
{
    ipcOsd->showNormal();
    ipcOsd->raise();
    ipcOsd->activateWindow();
}

void frmIpcControl::on_btnIpcNetConfig_clicked()
{
    ipcNetConfig->showNormal();
    ipcNetConfig->raise();
    ipcNetConfig->activateWindow();
}
