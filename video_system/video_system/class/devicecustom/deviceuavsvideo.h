#ifndef DEVICEUAVSVIDEO_H
#define DEVICEUAVSVIDEO_H

#include <QObject>
#include <QMutex>
#include "singleton.h"
#include "videowidgetx.h"

class DeviceUavsVideo : public QObject
{
    Q_OBJECT SINGLETON_DECL(DeviceUavsVideo)

public:
    explicit DeviceUavsVideo(QObject *parent = 0);

private:
    bool isOk;              //是否启动运行
    int listIndex1;         //图片队列索引
    int listIndex2;         //图片队列索引
    QStringList listFile1;  //图片文件队列
    QStringList listFile2;  //图片文件队列

    //指定目录获取图片队列
    QStringList getFiles(const QString &dirName);

    //视频控件集合
    QList<VideoWidget *> videoWidgets;

public slots:
    //设置对象集合
    void setVideoWidgets(QList<VideoWidget *> videoWidgets);
    //启动服务
    void start();

    //测试两个通道
    void load1();
    void load2();

signals:
    void receiveImage1(const QImage &image);
    void receiveImage2(const QImage &image);
};

#endif // DEVICEUAVSVIDEO_H
