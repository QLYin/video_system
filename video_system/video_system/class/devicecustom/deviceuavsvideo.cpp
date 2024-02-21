#include "deviceuavsvideo.h"
#include "qthelper.h"

SINGLETON_IMPL(DeviceUavsVideo)
DeviceUavsVideo::DeviceUavsVideo(QObject *parent) : QObject(parent)
{
    isOk = false;
}

QStringList DeviceUavsVideo::getFiles(const QString &dirName)
{
    QStringList listFile;
    //从文件夹加载所有图片名称
    QDir dir(dirName);
    if (dir.exists()) {
        QStringList filter;
        filter << "*.png" << "*.jpg";
        QStringList list = dir.entryList(filter);
        foreach (QString file, list) {
            listFile << QString("%1/%2").arg(dirName).arg(file);
        }
    }
    return listFile;
}

void DeviceUavsVideo::setVideoWidgets(QList<VideoWidget *> videoWidgets)
{
    this->videoWidgets = videoWidgets;
}

void DeviceUavsVideo::start()
{
    if (isOk) {
        return;
    }

    isOk = true;

    //绑定信号槽用来绘制实时图片
    connect(this, SIGNAL(receiveImage1(QImage)), videoWidgets.at(0), SLOT(setImage(QImage)));
    connect(this, SIGNAL(receiveImage2(QImage)), videoWidgets.at(1), SLOT(setImage(QImage)));

    listFile1 = getFiles(QtHelper::appPath() + "/image_normal/2021-04-07");
    listFile2 = getFiles(QtHelper::appPath() + "/image_normal/2021-04-08");

    //这里将图片采集发给对应的视频控件显示
    //这里用定时读取文件夹图片作为数据源显示两个通道

    QTimer *timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(load1()));
    timer1->start(100);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(load2()));
    timer2->start(200);
}

void DeviceUavsVideo::load1()
{
    int count = listFile1.count();
    if (count == 0) {
        return;
    }

    //播放到末尾
    if (listIndex1 >= count) {
        listIndex1 = 0;
    }

    //不断的将图片以信号的形式发出去,对应关联了槽函数会自动绘制
    QImage image(listFile1.at(listIndex1));
    emit receiveImage1(image);
    listIndex1++;
}

void DeviceUavsVideo::load2()
{
    int count = listFile2.count();
    if (count == 0) {
        return;
    }

    //播放到末尾
    if (listIndex2 >= count) {
        listIndex2 = 0;
    }

    //不断的将图片以信号的形式发出去,对应关联了槽函数会自动绘制
    QImage image(listFile2.at(listIndex2));
    emit receiveImage2(image);
    listIndex2++;
}
