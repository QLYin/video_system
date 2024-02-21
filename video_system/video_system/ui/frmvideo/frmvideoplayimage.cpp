#include "frmvideoplayimage.h"
#include "ui_frmvideoplayimage.h"
#include "qthelper.h"
#include "dbquery.h"
#include "devicehelper.h"
#include "videowidgetx.h"
#include "datareport.h"
#include "dataprint.h"
#include "datahelper.h"

frmVideoPlayImage::frmVideoPlayImage(QWidget *parent) : QWidget(parent), ui(new Ui::frmVideoPlayImage)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
    this->initVideo();
    on_btnSelect_clicked();
}

frmVideoPlayImage::~frmVideoPlayImage()
{
    delete ui;
}

VideoWidget *frmVideoPlayImage::getVideoWidget()
{
    return this->videoWidget;
}

void frmVideoPlayImage::initForm()
{
    //存储规则约定
    //1. 默认存储主目录 image_normal
    //2. 主目录下按照日期目录存放(2025-10-01/2025-10-31)
    //3. 日期目录下是单个图片文件(ch01_2021-04-07-14-08-11-222.jpg/ch02_2021-04-07-14-08-11-333.jpg)
    //4. 拓展功能可以存储对应的数据文件比如警情文字和图片文件一个目录(名称一样并且拓展名可以是txt)

    ui->widgetRight->setFixedWidth(AppData::RightWidth);

    ui->cboxCh->addItem("所有通道");
    for (int i = 1; i <= AppConfig::VideoCount; ++i) {
        ui->cboxCh->addItem(QString("通道%1").arg(i, 2, 10, QChar('0')));
    }

    ui->cboxType->addItem("存储图片");
    ui->cboxType->addItem("报警图片");

    QtHelper::initDataTimeEdit(ui->dateStart, -20);
    QtHelper::initDataTimeEdit(ui->dateEnd, 1);

    //启动定时器挨个加载图片
    timerPlay = new QTimer(this);
    connect(timerPlay, SIGNAL(timeout()), this, SLOT(loadImage()));
    timerPlay->setInterval(100);

    isPause = true;
    isStop = false;
    iconSize = AppData::BtnMinWidth / 2.5;
    ui->labTip->setText(QString("共找到 %1 个").arg(0));

    //后面对应定时器的间隔
    ui->cboxSpeed->addItem("0.5 倍速", "300");
    ui->cboxSpeed->addItem("1.0 倍速", "100");
    ui->cboxSpeed->addItem("2.0 倍速", "80");
    ui->cboxSpeed->addItem("4.0 倍速", "50");
    ui->cboxSpeed->addItem("8.0 倍速", "20");
    ui->cboxSpeed->setCurrentIndex(1);

    //设置图形字体图标
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    IconHelper::setIcon(ui->btnStop, 0xeb13, iconSize - 5);
    IconHelper::setIcon(ui->btnMute, 0xe674, iconSize - 10);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmVideoPlayImage::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);
}

void frmVideoPlayImage::initVideo()
{
    videoWidget = new VideoWidget;
    DeviceHelper::initVideoWidget(videoWidget);
    videoWidget->setBgText("视频图片");
    //加入到布局
    ui->gridLayout->addWidget(videoWidget, 0, 0);

    //绑定信号槽用来绘制实时图片
    connect(this, SIGNAL(receiveImage(QImage)), videoWidget, SLOT(setImage(QImage)));
}

void frmVideoPlayImage::loadImage()
{
    int count = listFile.count();
    if (count == 0) {
        return;
    }

    //播放到末尾
    if (listIndex >= count) {
        //自动切换到下一个
        if (ui->listWidget->currentRow() < ui->listWidget->count() - 1) {
            if (!isStop) {
                ui->listWidget->setCurrentRow(ui->listWidget->currentRow() + 1);
                on_listWidget_doubleClicked();
            }
        } else {
            on_btnStop_clicked();
            return;
        }
    }

    //不断的将图片以信号的形式发出去,对应关联了槽函数会自动绘制
    QImage image(listFile.at(listIndex));
    emit receiveImage(image);
    listIndex++;

    ui->labPosition->setText(QString("第 %1 张").arg(listIndex));
    ui->sliderPosition->setValue(listIndex);
}

void frmVideoPlayImage::addItem(const QString &text, const QString &data)
{
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(text);
    item->setData(Qt::UserRole, QString(data));
    item->setCheckState(Qt::Unchecked);
    ui->listWidget->addItem(item);
}

void frmVideoPlayImage::on_btnSelect_clicked()
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

    ui->listWidget->clear();
    QString filePath;
    if (ui->cboxType->currentText() == "存储图片") {
        filePath = AppData::ImageNormalPath;
    } else {
        filePath = AppData::ImageAlarmPath;
    }

    //过滤指定的通道
    QString channel = ui->cboxCh->currentText();
    QString ch = QString("ch%1").arg(channel.mid(2, channel.length()));

    //将有文件的日期目录加载到列表
    //并将该目录对应的通道的图片集合名称存入自定义属性
    //然后将开始时间加一天,直到大于结束时间
    while (dateStart <= dateEnd) {
        //生成对应日期的文件夹
        QString dateName = dateStart.toString("yyyy-MM-dd");
        QString savePath = QString("%1/%2").arg(filePath).arg(dateName);
        QDir saveDir(savePath);
        //判断文件夹是否存在
        if (saveDir.exists()) {
            //指定文件拓展名过滤,按照时间升序排序
            QStringList filter;
            filter << "*.jpg" << "*.png";
            QStringList fileNames = saveDir.entryList(filter, QDir::NoFilter, QDir::Time | QDir::Reversed);

            QStringList fullNames;
            foreach (QString fileName, fileNames) {
                QString name = savePath + "/" + fileName;
                if (channel == "所有通道") {
                    fullNames << name;
                } else {
                    if (fileName.startsWith(ch)) {
                        fullNames << name;
                    }
                }
            }

            int count = fullNames.count();
            if (count > 0) {
                QString text = QString("%1 %2 %3张").arg(dateName).arg(channel).arg(count);
                addItem(text, fullNames.join("|"));
            }
        }

        dateStart = dateStart.addDays(1);
    }

    ui->labTip->setText(QString("共找到 %1 个").arg(ui->listWidget->count()));
}

QStringList frmVideoPlayImage::getSelectFile(bool checked)
{
    //拿到所有勾选的文件集合
    QStringList fileNames;
    int count = ui->listWidget->count();
    for (int row = 0; row < count; row++) {
        QListWidgetItem *item = ui->listWidget->item(row);
        if (checked) {
            if (item->checkState() == Qt::Checked) {
                fileNames << item->data(Qt::UserRole).toString().split("|");
            }
        } else {
            fileNames << item->data(Qt::UserRole).toString().split("|");
        }
    }

    return fileNames;
}

void frmVideoPlayImage::on_btnDownload_clicked()
{
    QStringList fileNames = getSelectFile(true);
    if (fileNames.count() == 0) {
        QtHelper::showMessageBoxError("没有选中任何文件!", 3);
        return;
    }

    //弹出另存为对话框
    QString path = QtHelper::getExistingDirectory();
    if (path.isEmpty()) {
        return;
    }

    //挨个拷贝文件
    foreach (QString fileName, fileNames) {
        QString name = fileName.split("/").last();
        QString targetFile = path + "/" + name;
        QFile::copy(fileName, targetFile);
    }

    DbQuery::addUserLog("拷贝图片文件");
    QtHelper::showMessageBoxInfo("拷贝图片文件完成!", 3);
}

void frmVideoPlayImage::on_btnDelete_clicked()
{
    QStringList fileNames = getSelectFile(true);
    if (fileNames.count() == 0) {
        QtHelper::showMessageBoxError("没有选中任何文件!", 3);
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除选中文件吗? 删除后不能恢复!") != QMessageBox::Yes) {
        return;
    }

    //挨个删除文件
    foreach (QString fileName, fileNames) {
        QFile::remove(fileName);
    }

    DbQuery::addUserLog("删除图片文件");
    QtHelper::showMessageBoxInfo("删除图片文件完成!", 3);
    //立即重新查询
    on_btnSelect_clicked();
}

void frmVideoPlayImage::on_btnClear_clicked()
{
    QStringList fileNames = getSelectFile(false);
    if (fileNames.count() == 0) {
        QtHelper::showMessageBoxError("没有任何文件!", 3);
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有文件吗? 清空后不可恢复!") != QMessageBox::Yes) {
        return;
    }

    //挨个删除文件
    foreach (QString fileName, fileNames) {
        QFile::remove(fileName);
    }

    DbQuery::addUserLog("清空图片文件");
    QtHelper::showMessageBoxInfo("清空图片文件完成!", 3);
    //立即重新查询
    on_btnSelect_clicked();
}

QString frmVideoPlayImage::getHtml()
{
    //下面的图片是存放在资源文件中的示例图片
    //真实应用改成图片的绝对路径
    QStringList images;
#if 1
    images << ":/image/liuyifei0.jpg" << ":/image/liuyifei2.jpg" << ":/image/liuyifei1.jpg";
    images << ":/image/liuyifei0.jpg" << ":/image/liuyifei1.jpg" << ":/image/liuyifei2.jpg";
    images << ":/image/liuyifei2.jpg" << ":/image/liuyifei1.jpg" << ":/image/liuyifei0.jpg";
#else
    //下面表示从可执行文件目录下的 snap 文件夹取图片 1.jpg
    images << QtHelper::appPath() + "/snap/1.jpg";
#endif

    //还有很多参数可以设置,可以查阅对应结构体
    UavsReportData reportData;
    reportData.images = images;
    reportData.title = "主标题";
    reportData.subTitle = "副标题";
    reportData.name = "无人机-NO1";

    //获取打印区域的尺寸用于计算图片的宽度
    //多减一点是为了有点冗余
    int imageWidth = (DataPrint::dataPrint->getPrinter()->pageRect(QPrinter::DevicePixel).width() - 80) / 2;

    QStringList list;
    DataReport::creatUavsReportHead(list, reportData);
    DataReport::creatUavsReportBody(list, reportData, imageWidth);
    return list.join("");
}

void frmVideoPlayImage::dataout(quint8 type)
{
    DataContent dataContent;
    dataContent.fileName = QtHelper::appPath() + "/db/html.pdf";
    dataContent.pageMargin = QMargins(15, 20, 15, 20);

    //初始化一次保证对象实例化成功
    DataHelper::init();
    DataPrint::dataPrint->init();
    DataPrint::dataPrint->setDataContent(dataContent);

    //这里重新设置html用到了打印区域所有需要设置一遍 setDataContent 后再重新设置
    //第一次 setDataContent 后才能获取到准确的打印区域
    dataContent.html = getHtml();
    DataPrint::dataPrint->setDataContent(dataContent);

    if (type == 0) {
        DataPrint::dataPrint->print();
    } else if (type == 1) {
        DataPrint::dataPrint->open();
        DataPrint::dataPrint->close();
        QtHelper::openFile(dataContent.fileName, "导出告警报告");
    }
}

void frmVideoPlayImage::on_btnPdf_clicked()
{
    dataout(1);
}

void frmVideoPlayImage::on_btnPrint_clicked()
{
    dataout(0);
}

void frmVideoPlayImage::on_btnPlay_clicked()
{
    //不在运行阶段并且选中了则主动触发双击
    if (listIndex == 0 && ui->listWidget->currentRow() >= 0) {
        on_listWidget_doubleClicked();
        return;
    }

    if (isPause) {
        timerPlay->start();
        IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
    } else {
        timerPlay->stop();
        IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);
    }

    isPause = !isPause;
}

void frmVideoPlayImage::on_btnStop_clicked()
{
    listIndex = 0;
    listFile.clear();

    isPause = true;
    isStop = true;
    timerPlay->stop();
    videoWidget->clear();
    IconHelper::setIcon(ui->btnPlay, 0xeb12, iconSize);

    ui->sliderPosition->setValue(0);
    ui->labPosition->setText(QString("第 %1 张").arg(0));
    ui->labDuration->setText(QString("共 %1 张").arg(0));
}

void frmVideoPlayImage::on_btnMute_clicked()
{

}

void frmVideoPlayImage::on_listWidget_doubleClicked()
{
    //设置图片拉伸策略
    videoWidget->setScaleMode(ScaleMode_Auto);

    //从属性中取出该文件夹对应的所有查询到的图片名称集合
    isStop = false;
    listFile = ui->listWidget->currentItem()->data(Qt::UserRole).toString().split("|");
    int count = listFile.count();
    if (count > 0) {
        listIndex = 0;
        ui->labDuration->setText(QString("共 %1 张").arg(count));
        ui->sliderPosition->setRange(0, count - 1);
        ui->sliderPosition->setValue(0);

        isPause = false;
        timerPlay->start();
        IconHelper::setIcon(ui->btnPlay, 0xeb11, iconSize);
    }
}

void frmVideoPlayImage::on_listWidget_itemPressed(QListWidgetItem *item)
{
    //按下的时候自动切换复选框状态
    bool checked = (item->checkState() == Qt::Checked);
    item->setCheckState(checked ? Qt::Unchecked : Qt::Checked);
}

void frmVideoPlayImage::on_sliderPosition_clicked()
{
    if (listFile.count() == 0) {
        return;
    }

    listIndex = ui->sliderPosition->value();
    if (!timerPlay->isActive()) {
        this->loadImage();
    }
}

void frmVideoPlayImage::on_sliderPosition_sliderMoved(int value)
{
    if (listFile.count() == 0) {
        return;
    }

    listIndex = value;    
}

void frmVideoPlayImage::on_cboxSpeed_currentIndexChanged(int index)
{
    int interval = ui->cboxSpeed->itemData(index).toDouble();
    timerPlay->setInterval(interval);
}
