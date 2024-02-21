#include "frmvideoupload.h"
#include "ui_frmvideoupload.h"
#include "qthelper.h"
#include "devicehelper.h"

frmVideoUpload::frmVideoUpload(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoUpload)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
    this->initConfig();
}

frmVideoUpload::~frmVideoUpload()
{
    delete ui;
}

void frmVideoUpload::initForm()
{
    ui->frameRight->setFixedWidth(AppData::RightWidth);

    ui->cboxCh->addItem("所有通道");
    for (int i = 1; i <= AppConfig::VideoCount; ++i) {
        ui->cboxCh->addItem(QString("通道%1").arg(i, 2, 10, QChar('0')));
    }

    ui->cboxType->addItem("存储视频");
    ui->cboxType->addItem("报警视频");

    QtHelper::initDataTimeEdit(ui->dateStart, -7);
    QtHelper::initDataTimeEdit(ui->dateEnd, 0);

    //初始化通用的表格属性
    QtHelper::initTableView(ui->widget->getTable(), AppData::RowHeight, true, false);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));

    //绑定通用文件上传窗体信号
    ui->widget->initStyle();
    connect(ui->widget, SIGNAL(finshed()), this, SLOT(finshed()));
    connect(ui->widget, SIGNAL(error(QString)), this, SLOT(error(QString)));
}

void frmVideoUpload::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);
}

void frmVideoUpload::initConfig()
{
    ui->txtUploadHost->setText(AppConfig::UploadHost);
    connect(ui->txtUploadHost, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUploadPort->setText(QString::number(AppConfig::UploadPort));
    connect(ui->txtUploadPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmVideoUpload::saveConfig()
{
    AppConfig::UploadHost = ui->txtUploadHost->text().trimmed();
    AppConfig::UploadPort = ui->txtUploadPort->text().trimmed().toInt();
    AppConfig::writeConfig();
}

void frmVideoUpload::finshed()
{
    ui->frameRight->setEnabled(true);
    QtHelper::showMessageBoxInfo("恭喜你所有文件发送完成!");
}

void frmVideoUpload::error(const QString &text)
{
    ui->frameRight->setEnabled(true);
    QString msg = QString("上传文件发生出错.\n原因: %1.").arg(text);
    QtHelper::showMessageBoxError(msg, 3);
}

void frmVideoUpload::on_btnSelect_clicked()
{
    QDate dateStart = ui->dateStart->date();
    QDate dateEnd = ui->dateEnd->date();
    if (dateStart > dateEnd) {
        QtHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
        return;
    }

    //将日期转换为日期时间计算相差的天数,超过最大天数则提示不用继续
    if (dateStart.daysTo(dateEnd) >= 60) {
        QtHelper::showMessageBoxError("每次最大只能查询60天内!", 3);
        return;
    }

    //清空数据
    ui->widget->clear();

    QString filePath;
    QString type = ui->cboxType->currentText();
    if (type == "存储视频") {
        filePath = AppData::VideoNormalPath;
    } else {
        filePath = AppData::VideoAlarmPath;
    }

    //拓展名集合
    QStringList filter;
    filter << "*.mp4" << "*.h264" << "*.h265";

    //获取所有文件夹名称,根据时间查询对应通道对应类型视频
    //如果开始时间小于或者等于结束时间,则将开始时间对应文件夹下的视频文件添加到列表
    //然后将开始时间加一天,直到大于结束时间
    while (dateStart <= dateEnd) {
        //生成对应日期的文件夹
        QString path = QString("%1/%2").arg(filePath).arg(dateStart.toString("yyyy-MM-dd"));
        dateStart = dateStart.addDays(1);
        QDir dir(path);
        //判断文件夹是否存在
        if (!dir.exists()) {
            continue;
        }

        //指定文件拓展名过滤,按照时间升序排序
        QStringList names = dir.entryList(filter, QDir::Files | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
        foreach (QString name, names) {
            //如果是选择的所有通道,则不过滤视频文件
            if (ui->cboxCh->currentText() == "所有通道") {
                ui->widget->addFile(type, name, path);
            } else {
                //对应通道的视频文件添加进来
                QString chVideo = name.split("_").first();
                QString chName = QString("ch%1").arg(ui->cboxCh->currentIndex(), 2, 10, QChar('0'));
                if (chVideo == chName) {
                    ui->widget->addFile(type, name, path);
                }
            }
        }
    }
}

void frmVideoUpload::on_btnUpload_clicked()
{
    if (ui->widget->getFileNames().count() == 0) {
        QtHelper::showMessageBoxError("没有要上传的文件请仔细检查!");
        return;
    }

    ui->frameRight->setEnabled(false);
    qApp->processEvents();
    ui->widget->upload(AppConfig::UploadHost, AppConfig::UploadPort, 0xFFFF);
}
