#include "devicemap.h"
#include "qthelper.h"

SINGLETON_IMPL(DeviceMap)
DeviceMap::DeviceMap(QObject *parent) : QObject(parent)
{
}

DeviceMap::~DeviceMap()
{
}

void DeviceMap::loadMap()
{
    //将图片文件全部转为内存图片
    foreach (QString mapName, AppData::MapNames) {        
        QPixmap pix = QPixmap(QString("%1/%2").arg(AppData::MapPath).arg(mapName));
        pix = pix.scaled(AppData::MapWidth, AppData::MapHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mapNames.append(mapName);
        mapPixs.append(pix);
    }
}

void DeviceMap::appendMapPix(const QString &mapName, const QPixmap &pix)
{
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        mapPixs[index] = pix;
    } else {
        mapNames.append(mapName);
        mapPixs.append(pix);
    }
}

void DeviceMap::appendMapPix(const QString &mapName)
{
    //插入新图片
    QPixmap pix = QPixmap(QString("%1/%2").arg(AppData::MapPath).arg(mapName));
    pix = pix.scaled(AppData::MapWidth, AppData::MapHeight);
    mapNames.append(mapName);
    mapPixs.append(pix);
}

void DeviceMap::removeMapPix(const QString &mapName)
{
    //移除图片
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        mapNames.removeAt(index);
        mapPixs.removeAt(index);
    }
}

bool DeviceMap::existMapPix(const QString &mapName)
{
    return mapNames.contains(mapName);
}

QPixmap DeviceMap::getMapPix(const QString &mapName)
{
    QPixmap pix;
    int index = mapNames.indexOf(mapName);
    if (index >= 0) {
        pix = mapPixs.at(index);
    }

    return pix;
}
