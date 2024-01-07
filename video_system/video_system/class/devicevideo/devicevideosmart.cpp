#include "devicevideosmart.h"
#include "qthelper.h"
#include "videowidgetx.h"

DeviceVideoSmart::DeviceVideoSmart(QObject *parent) : QThread(parent)
{
    stopped = false;
    isPause = false;
    videoWidget = 0;

    //需要注册下这个类型
    qRegisterMetaType<QList<QRect> >("QList<QRect>");
}

void DeviceVideoSmart::run()
{
    msleep(1000);
    while (!stopped) {
        if (isPause || !AppConfig::VideoSmart || !videoWidget || !videoWidget->getIsRunning()) {
            msleep(1);
            continue;
        }

        //人工智能处理算法放在这里进行运算
        //运算好以后直接将结果以信号的形式发送出去

        //这里演示方框图形信息
        if (1) {
            //限定宽高区域
            int w = videoWidget->getVideoWidth();
            int h = videoWidget->getVideoHeight();
            int width = w - 100;
            int height = h - 100;
            int wx = w / 50;
            int hx = h / 50;

            //随机生成矩形
            QList<QRect> rects;
            for (int i = 0; i < 30; ++i) {
                int x = rand() % width;
                int y = rand() % height;
                int w = wx + rand() % 40;
                int h = hx + rand() % 30;
                rects << QRect(x, y, w, h);
            }

            //异步执行
            QMetaObject::invokeMethod(this, "receiveRects", Q_ARG(QList<QRect>, rects));
            msleep(500);
        }

        //这里演示文字标签信息
        if (1) {
            int currentCount = rand() % 15;
            int maxCount = rand() % 20 + 15;
            //qDebug() << TIMEMS << currentCount << currentThreadId();
            QMetaObject::invokeMethod(this, "receiveData", Q_ARG(QString, "dateTime"), Q_ARG(QVariant, 0));
            QMetaObject::invokeMethod(this, "receiveData", Q_ARG(QString, "currentCount"), Q_ARG(QVariant, currentCount));
            QMetaObject::invokeMethod(this, "receiveData", Q_ARG(QString, "maxCount"), Q_ARG(QVariant, maxCount));
            msleep(500);
        }

        //必须要有这个延时休息一下
        msleep(1);
    }

    stopped = false;
    isPause = false;
}

void DeviceVideoSmart::receiveKbps(qreal kbps, int frameRate)
{
    OsdInfo osd;
    osd.name = "kbps";
    osd.color = "#FF0000";
    osd.fontSize = fontSize;
    osd.position = OsdPosition_RightTop;
    osd.text = QString("%1 kbps\n%2 fps").arg((int)kbps).arg(frameRate);
    videoWidget->setOsd(osd);
}

void DeviceVideoSmart::receivePlayStart(int time)
{
    //字体大小按照视频宽度比例来
    fontSize = videoWidget->getVideoWidth() / 30;
}

void DeviceVideoSmart::receiveRects(const QList<QRect> &rects)
{
    if (!videoWidget || !videoWidget->getIsRunning()) {
        return;
    }

    int borderWidth = videoWidget->getVideoWidth() / 500;
    borderWidth = borderWidth < 1 ? 1 : borderWidth;

    //取随机颜色
    static QStringList colors = QColor::colorNames();
    int count = rects.count();
    for (int i = 0; i < count; ++i) {
        GraphInfo graph;
        graph.name = QString("graph%1").arg(i);
        graph.borderColor = colors.at(rand() % colors.count());
        graph.borderWidth = borderWidth;
        graph.rect = rects.at(i);
        videoWidget->setGraph(graph);
    }
}

void DeviceVideoSmart::receiveData(const QString &type, const QVariant &data)
{
    if (!videoWidget || !videoWidget->getIsRunning()) {
        return;
    }

    OsdInfo osd;
    osd.name = type;
    if (type == "dateTime") {
        osd.fontSize = fontSize;
        osd.color = "#FFFFFF";
        osd.format = OsdFormat_DateTime;
        osd.position = OsdPosition_LeftTop;
        videoWidget->setOsd(osd);
    } else if (type == "currentCount") {
        osd.fontSize = fontSize;
        osd.text = QString("当前人数: %1").arg(data.toInt());
        osd.color = "#D64D54";
        osd.format = OsdFormat_Text;
        osd.position = OsdPosition_LeftBottom;
        videoWidget->setOsd(osd);
    } else if (type == "maxCount") {
        osd.fontSize = fontSize;
        osd.text = QString("限定人数: %1").arg(data.toInt());
        osd.color = "#FDCD72";
        osd.format = OsdFormat_Text;
        osd.position = OsdPosition_RightBottom;
        videoWidget->setOsd(osd);
    }
}

void DeviceVideoSmart::setVideoWidget(VideoWidget *videoWidget)
{
    this->videoWidget = videoWidget;
    //关联实时码率信号将实时码率作为文字水印贴到视频上
    connect(videoWidget, SIGNAL(sig_receiveKbps(qreal, int)), this, SLOT(receiveKbps(qreal, int)));
    connect(videoWidget, SIGNAL(sig_receivePlayStart(int)), this, SLOT(receivePlayStart(int)));
}

void DeviceVideoSmart::stop()
{
    stopped = true;
}

void DeviceVideoSmart::pause()
{
    isPause = true;
}
