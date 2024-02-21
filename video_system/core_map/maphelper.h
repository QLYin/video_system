#ifndef MAPHELPER_H
#define MAPHELPER_H

#include <QPoint>
#include <QStringList>

class MapHelper
{
public:
    //获取小数点经纬度值
    static QString getLngLat1(const QString &value, int len = 6);
    static QString getLngLat2(const QString &point, int len = 6);

    //文本经纬度与坐标互换
    static QPointF textToPoint(const QString &text);
    static QString pointToText(const QPointF &point);

    //获取两个经纬度的距离
    static double rad(double value);
    static double getDistance(const QString &point1, const QString &point2);
    static double getDistance(const QPointF &point1, const QPointF &point2);
    static double getDistance(double lng1, double lat1, double lng2, double lat2);

    //获取两点之间的角度
    static double getAngle(const QString &point1, const QString &point2);
    static double getAngle(const QPointF &point1, const QPointF &point2);
    static double getAngle(double lng1, double lat1, double lng2, double lat2);

    //获取两个点之间的中间点
    static QPointF getCenter(const QString &point1, const QString &point2);
    static QPointF getCenter(const QPointF &point1, const QPointF &point2);
    static QPointF getCenter(double lng1, double lat1, double lng2, double lat2);

    //初始化地图坐标转换的一些常量
    static void initMapData();
    //暂时不知道什么作用,这是官网抄过来的
    static QPointF sx(const QPointF &point, const QList<double> &b);
    //如果经度大于180或者小于-180,将经度调整到-180到180之间
    static double ft(double a, double b, double c);
    //将纬度调整到-74~74之间
    static double lt(double a, double b, double c);

    //百度地图经纬度转换处理
    static QPointF lngLatToPoint(double lng, double lat);
    static QPoint pointToPixel(const QPointF &point, int zoom);
    static QPoint pixelToTile(const QPoint &point);
    static QPoint lngLatToTileBaiDu(double lng, double lat, int zoom);

    //经纬度转墨卡托坐标
    static QPointF pointToMercator(const QPointF &point);
    //经纬度转天地图坐标,最终发现天地图坐标和谷歌坐标基本一致
    static QPoint lngLatToTileTian(double lng, double lat, int zoom);
    static QPoint lngLatToTileTian2(double lng, double lat, int zoom);

    //百度坐标转谷歌坐标
    static QPointF pointToGoogle(const QPointF &point);
    //经纬度转谷歌坐标
    static QPoint lngLatToTileGoogle(double lng, double lat, int zoom);
    static QPoint lngLatToTileGoogle2(double lng, double lat, int zoom);

    //常用转换就4种: GPS转百度、GPS转其他、百度转其他、其他转百度
    //WGS-84: 国际标准，GPS坐标（Google Earth使用、或者GPS模块）
    //GCJ-02: 中国坐标偏移标准，Google Map、高德、腾讯使用
    //BD-09:  百度坐标偏移标准，Baidu Map使用
    static QPointF gcj2bd(const QPointF &point);
    static QPointF bd2gcj(const QPointF &point);
    static double transformLng(double lng, double lat);
    static double transformLat(double lng, double lat);
    static QPointF wgs2gcj(const QPointF &point);
    static QPointF wgs2bd(const QPointF &point);
};

#endif // MAPHELPER_H
