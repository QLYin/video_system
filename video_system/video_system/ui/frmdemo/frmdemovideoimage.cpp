#include "frmdemovideoimage.h"
#include "ui_frmdemovideoimage.h"
#include "qthelper.h"

frmDemoVideoImage::frmDemoVideoImage(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoVideoImage)
{
    ui->setupUi(this);
    this->initForm();
}

frmDemoVideoImage::~frmDemoVideoImage()
{
    delete ui;
}

void frmDemoVideoImage::initForm()
{
    QString snapPath = QtHelper::appPath() + "/image_normal/" + QDATE;
    ui->cboxUrl->addItem(snapPath);
    ui->cboxUrl->addItem("E:/myQt/可执行文件监控/bin_video_demo/snap");
    ui->cboxUrl->lineEdit()->setText(AppConfig::VideoImageUrl);

    //设置视频控件参数
    //ui->videoWidget->setScaleMode(ScaleMode_Fill);
    ui->videoWidget->setVideoMode(VideoMode_Painter);

    //绑定信号槽用来绘制实时图片
    connect(this, SIGNAL(receiveImage(QImage)), ui->videoWidget, SLOT(setImage(QImage)));

    //启动定时器挨个加载图片
    timerPlay = new QTimer(this);
    connect(timerPlay, SIGNAL(timeout()), this, SLOT(loadImage()));
    //这里调整速度
    timerPlay->setInterval(100);

    //最简单例子传入图片就显示
    //QImage image(qApp->applicationDirPath() + "/snap/2021-03-12-13-51-16-609.jpg");
    //emit receiveImage(image);
}

void frmDemoVideoImage::loadImage()
{
    int count = listFile.count();
    if (count == 0) {
        return;
    }

    //播放到末尾
    if (listIndex >= count) {
        //这里将索引=0是为了继续循环播放
        listIndex = 0;
        //也可以停止播放
        //timerPlay->stop();
        //return;
    }

    //不断的将图片以信号的形式发出去,对应关联了槽函数会自动绘制
    QImage image(listFile.at(listIndex));
    emit receiveImage(image);
    listIndex++;
}

void frmDemoVideoImage::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        AppConfig::VideoImageUrl = ui->cboxUrl->currentText();
        AppConfig::writeConfig();

        //从文件夹加载所有图片名称
        QString dirName = AppConfig::VideoImageUrl;
        QDir dir(dirName);
        if (dir.exists()) {
            QStringList filter;
            filter << "*.png" << "*.jpg";
            QStringList list = dir.entryList(filter);
            foreach (QString file, list) {
                listFile << QString("%1/%2").arg(dirName).arg(file);
            }
        }

        if (listFile.count() > 0) {
            listIndex = 0;
            timerPlay->start();
            ui->btnOpen->setText("关闭");
        }
    } else {
        listIndex = 0;
        listFile.clear();
        timerPlay->stop();
        ui->videoWidget->stop();
        ui->btnOpen->setText("打开");
        ui->btnPause->setText("暂停");
    }
}

void frmDemoVideoImage::on_btnPause_clicked()
{
    if (ui->btnOpen->text() == "打开") {
        return;
    }

    if (ui->btnPause->text() == "暂停") {
        timerPlay->stop();
        ui->btnPause->setText("继续");
    } else {
        timerPlay->start();
        ui->btnPause->setText("暂停");
    }
}
