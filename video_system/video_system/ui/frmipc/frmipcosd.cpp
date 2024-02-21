#include "frmipcosd.h"
#include "ui_frmipcosd.h"
#include "qthelper.h"
#include "deviceonvif.h"

frmIpcOsd::frmIpcOsd(QWidget *parent) : QWidget(parent),  ui(new Ui::frmIpcOsd)
{
    ui->setupUi(this);
    this->initForm();
    QtHelper::setFormInCenter(this);
}

frmIpcOsd::~frmIpcOsd()
{
    delete ui;
}

void frmIpcOsd::initForm()
{
    this->setWindowTitle("OSD配置");
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->setFixedSize(this->size());

    ui->cboxPositionType->addItem("自定义", "Custom");
    ui->cboxPositionType->addItem("左上角", "UpperLeft");
    ui->cboxPositionType->addItem("右上角", "UpperRight");
    ui->cboxPositionType->addItem("左下角", "LowerLeft");
    ui->cboxPositionType->addItem("右下角", "LowerRight");

    ui->cboxTextType->addItem("日期时间", "DateAndTime");
    ui->cboxTextType->addItem("文本文字", "Plain");

    connect(AppEvent::Instance(), SIGNAL(receivePoint(int, QPoint)), this, SLOT(receivePoint(int, QPoint)));
}

void frmIpcOsd::receivePoint(int type, const QPoint &point)
{
    ui->txtPosition->setText(QString("%1, %2").arg(point.x()).arg(point.y()));
}

OnvifOsdInfo frmIpcOsd::getOSD()
{
    OnvifOsdInfo osd;
    osd.token = ui->txtToken->text();
    osd.config = ui->txtConfig->text();
    osd.positionType = ui->cboxPositionType->itemData(ui->cboxPositionType->currentIndex()).toString();
    QString position = ui->txtPosition->text();
    QStringList xy = position.split(",");
    if (xy.count() == 2) {
        osd.position = QPoint(xy.at(0).toInt(), xy.at(1).toInt());
    }

    osd.fontSize = ui->txtFontSize->text().toInt();
    osd.textType = ui->cboxTextType->itemData(ui->cboxTextType->currentIndex()).toString();
    osd.text = ui->txtText->text();
    return osd;
}

void frmIpcOsd::on_btnGetOSDs_clicked()
{
    ui->listWidget->clear();
    QList<OnvifOsdInfo> osds = DeviceOnvif::getOSDs(AppData::CurrentUrl);
    foreach (OnvifOsdInfo osd, osds) {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(osd.token);
        item->setData(Qt::UserRole, osd.data());
        ui->listWidget->addItem(item);
    }
}

void frmIpcOsd::on_btnCreateOSD_clicked()
{
    DeviceOnvif::osdControl(0, AppData::CurrentUrl, getOSD());
    on_btnGetOSDs_clicked();
}

void frmIpcOsd::on_btnSetOSD_clicked()
{
    if (ui->listWidget->count() <= 0) {
        return;
    }

    DeviceOnvif::osdControl(1, AppData::CurrentUrl, getOSD());
    on_btnGetOSDs_clicked();
}

void frmIpcOsd::on_btnDeleteOSD_clicked()
{
    if (ui->listWidget->count() <= 0 || ui->listWidget->currentRow() < 0) {
        return;
    }

    DeviceOnvif::osdControl(2, AppData::CurrentUrl, getOSD());
    on_btnGetOSDs_clicked();
}

void frmIpcOsd::on_listWidget_itemPressed(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    QVariantList list = data.value<QVariantList>();
    if (list.count() == 10) {
        ui->txtToken->setText(list.at(0).toString());
        ui->txtConfig->setText(list.at(1).toString());
        ui->cboxPositionType->setCurrentIndex(ui->cboxPositionType->findData(list.at(2).toString()));
        QPoint position = list.at(3).toPoint();
        ui->txtPosition->setText(QString("%1, %2").arg(position.x()).arg(position.y()));
        ui->txtFontSize->setText(list.at(4).toString());
        ui->cboxTextType->setCurrentIndex(ui->cboxTextType->findData(list.at(6).toString()));
        ui->txtText->setText(list.at(7).toString());
    }
}
