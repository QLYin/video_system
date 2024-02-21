#ifndef DEVICEMAP_H
#define DEVICEMAP_H

#include "head.h"
#include "singleton.h"

class DeviceMap : public QObject
{
    Q_OBJECT SINGLETON_DECL(DeviceMap)

public:
    explicit DeviceMap(QObject *parent = 0);
    ~DeviceMap();

private:
    //地图名称+内存地图 集合
    QList<QString> mapNames;
    QList<QPixmap> mapPixs;

public slots:
    //载入地图
    void loadMap();

    //添加到图片队列
    void appendMapPix(const QString &mapName, const QPixmap &pix);
    //添加地图数据
    void appendMapPix(const QString &mapName);
    //移除地图数据
    void removeMapPix(const QString &mapName);

    //是否已经存在地图数据
    bool existMapPix(const QString &mapName);
    //获取地图数据
    QPixmap getMapPix(const QString &mapName);

};

#endif // DEVICEMAP_H
