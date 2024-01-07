#include "maphelper.h"
#include "qmath.h"
#include "qmap.h"
#include "qdebug.h"

QString MapHelper::getLngLat1(const QString &value, int len)
{
    QStringList list = value.split(".");
    //旧方法直接取长度可能位数不足导致不对称
    QString dot = list.at(1).left(len);
    //不足在末尾补0字符 -len表示从后面开始 len表示从前面开始
    dot = QString("%1").arg(dot, -len, QChar('0'));
    QString result = QString("%1.%2").arg(list.at(0)).arg(dot);
    return result;
}

QString MapHelper::getLngLat2(const QString &point, int len)
{
    QString result;
    if (point.contains(",")) {
        QStringList list = point.split(",");
        QString lng = MapHelper::getLngLat1(list.at(0), len);
        QString lat = MapHelper::getLngLat1(list.at(1), len);
        result = QString("%1,%2").arg(lng).arg(lat);
    }
    return result;
}

QPointF MapHelper::textToPoint(const QString &text)
{
    QPointF point;
    QStringList list = text.split(",");
    if (list.count() == 2) {
        point = QPointF(list.at(0).toDouble(), list.at(1).toDouble());
    }
    return point;
}

QString MapHelper::pointToText(const QPointF &point)
{
    return QString("%1,%2").arg(point.x()).arg(point.y());
}

double MapHelper::rad(double value)
{
    return value * M_PI / 180.0;
}

double MapHelper::getDistance(const QString &point1, const QString &point2)
{
    return getDistance(textToPoint(point1), textToPoint(point2));
}

double MapHelper::getDistance(const QPointF &point1, const QPointF &point2)
{
    return getDistance(point1.x(), point1.y(), point2.x(), point2.y());
}

double MapHelper::getDistance(double lng1, double lat1, double lng2, double lat2)
{
    double radLat1 = rad(lat1);
    double radLat2 = rad(lat2);
    double a = radLat1 - radLat2;
    double b = rad(lng1) - rad(lng2);
    double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
    //地球半径=6378.137公里
    s = s * 6378.137;
    //下面输出的是公里
    //s = qRound(s * 10000) / 10000;
    //下面输出的是米
    s = qRound(s * 1000);
    return s;
}

double MapHelper::getAngle(const QString &point1, const QString &point2)
{
    return getAngle(textToPoint(point1), textToPoint(point2));
}

double MapHelper::getAngle(const QPointF &point1, const QPointF &point2)
{
    if (point1 == point2) {
        return 0;
    }

    return getAngle(point1.x(), point1.y(), point2.x(), point2.y());
}

double MapHelper::getAngle(double lng1, double lat1, double lng2, double lat2)
{
    double angle = atan2(qAbs(lng1 - lng2), qAbs(lat1 - lat2));
    if (lng2 >= lng1) {
        if (lat2 >= lat1) {
        } else {
            angle = M_PI - angle;
        }
    } else {
        if (lat2 >= lat1) {
            angle = 2 * M_PI - angle;
        } else {
            angle = M_PI + angle;
        }
    }

    angle = angle * 180 / M_PI;
    return angle;
}

QPointF MapHelper::getCenter(const QString &point1, const QString &point2)
{
    return getCenter(textToPoint(point1), textToPoint(point2));
}

QPointF MapHelper::getCenter(const QPointF &point1, const QPointF &point2)
{
    if (point1 == point2) {
        return QPointF();
    }

    return getCenter(point1.x(), point1.y(), point2.x(), point2.y());
}

QPointF MapHelper::getCenter(double lng1, double lat1, double lng2, double lat2)
{
    double aLon = 0, aLat = 0;
    double bLon = lng1 - lng2;
    double bLat = lat1 - lat2;

    if (bLon > 0) {
        aLon = lng1 - qAbs(bLon) / 2;
    } else {
        aLon = lng2 - qAbs(bLon) / 2;
    }

    if (bLat > 0) {
        aLat = lat1 - qAbs(bLat) / 2;
    } else {
        aLat = lat2 - qAbs(bLat) / 2;
    }

    return QPointF(aLon, aLat);
}

QList<int> wm;
QList<QList<double> > bv;
QList<double> bv1, bv2, bv3, bv4, bv5, bv6;
void MapHelper::initMapData()
{
    //只需要初始化一次
    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        //下面这些是百度平面坐标(墨卡托坐标)转百度坐标用到的一些常量,官网指定的
        wm << 75 << 60 << 45 << 30 << 15 << 0;
        bv1 << -0.0015702102444 << 111320.7020616939 << 1704480524535203L << -10338987376042340L << 26112667856603880L
            << -35149669176653700L << 26595700718403920L << -10725012454188240L << 1800819912950474L << 82.5;
        bv2 << 8.277824516172526E-4 << 111320.7020463578 << 6.477955746671607E8 << -4.082003173641316E9 << 1.077490566351142E10
            << -1.517187553151559E10 << 1.205306533862167E10 << -5.124939663577472E9 << 9.133119359512032E8 << 67.5;
        bv3 << 0.00337398766765 << 111320.7020202162 << 4481351.045890365 << -2.339375119931662E7 << 7.968221547186455E7
            << -1.159649932797253E8 << 9.723671115602145E7 << -4.366194633752821E7 << 8477230.501135234 << 52.5;
        bv4 << 0.00220636496208 << 111320.7020209128 << 51751.86112841131 << 3796837.749470245 << 992013.7397791013
            << - 1221952.21711287 << 1340652.697009075 << -620943.6990984312 << 144416.9293806241 << 37.5;
        bv5 << -3.441963504368392E-4 << 111320.7020576856 << 278.2353980772752 << 2485758.690035394 << 6070.750963243378
            << 54821.18345352118 << 9540.606633304236 << -2710.55326746645 << 1405.483844121726 << 22.5;
        bv6 << -3.218135878613132E-4 << 111320.7020701615 << 0.00369383431289 << 823725.6402795718 << 0.46104986909093
            << 2351.343141331292 << 1.58060784298199 << 8.77738589078284 << 0.37238884252424 << 7.45;
        bv << bv1 << bv2 << bv3 << bv4 << bv5 << bv6;
    }
}

QPointF MapHelper::sx(const QPointF &point, const QList<double> &b)
{
    double c = b.at(0) + b.at(1) * qAbs(point.x());
    double d = qAbs(point.y()) / b.at(9);
    double e = b.at(2) + b.at(3) * d + b.at(4) * d * d + b.at(5) * d * d * d + b.at(6) * d * d * d * d + b.at(7) * d * d * d * d * d + b.at(8) * d * d * d * d * d * d;
    double f = c * (0 > point.x() ? -1 : 1);
    double g = e * (0 > point.y() ? -1 : 1);
    return QPointF(f, g);
}

double MapHelper::ft(double a, double b, double c)
{
    for (; a > c;) {
        a -= c - b;
    }
    for (; a < b;) {
        a += c - b;
    }
    return a;
}

double MapHelper::lt(double a, double b, double c)
{
    a = qMax(a, b);
    a = qMin(a, c);
    return a;
}

QPointF MapHelper::lngLatToPoint(double lng, double lat)
{
    //初始化地图常量数据
    initMapData();

    double x = ft(lng, -180, 180);
    double y = lt(lat, -74, 74);
    QPointF point(x, y);
    QList<double> b;
    int count = wm.count();

    //北纬
    for (int d = 0; d < count; d++) {
        if (point.y() >= wm.at(d)) {
            b = bv.at(d);
            break;
        }
    }

    //南纬
    if (b.count() <= 0) {
        for (int d = count - 1; 0 <= d; d--) {
            if (point.y() <= -wm.at(d)) {
                b = bv.at(d);
                break;
            }
        }
    }

    QPointF p = sx(point, b);
    return p;
}

QPoint MapHelper::pointToPixel(const QPointF &point, int zoom)
{
    int x = (int)qFloor(point.x() * qPow(2, zoom - 18));
    int y = (int)qFloor(point.y() * qPow(2, zoom - 18));
    return QPoint(x, y);
}

QPoint MapHelper::pixelToTile(const QPoint &point)
{
    int x = (int)qFloor(point.x() * 1.0 / 256);
    int y = (int)qFloor(point.y() * 1.0 / 256);
    return QPoint(x, y);
}

QPoint MapHelper::lngLatToTileBaiDu(double lng, double lat, int zoom)
{
    QPointF pointf = lngLatToPoint(lng, lat);
    QPoint point = pointToPixel(pointf, zoom);
    point = pixelToTile(point);
    return point;
}

QPointF MapHelper::pointToMercator(const QPointF &point)
{
    double x = point.x() * 20037508.3427892 / 180;
    double y = qLn(qTan((90 + point.y()) * M_PI / 360)) / (M_PI / 180);
    y = y * 20037508.3427892 / 180;
    return QPointF(x, y);
}

QPoint MapHelper::lngLatToTileTian(double lng, double lat, int zoom)
{
    double topTileFromX = -20037508.3427892;
    double topTileFromY = 20037508.3427892;

    static QMap<int, double> scales;
    if (scales.count() == 0) {
        scales.insert(18, 0.597164283559817);
        scales.insert(17, 1.19432856685505);
        scales.insert(16, 2.38865713397468);
        scales.insert(15, 4.77731426794937);
        scales.insert(14, 9.55462853563415);
        scales.insert(13, 19.1092570712683);
        scales.insert(12, 38.2185141425366);
        scales.insert(11, 76.4370282850732);
        scales.insert(10, 152.8740565704);
        scales.insert(9, 305.7481128);
        scales.insert(8, 611.49622628138);
        scales.insert(7, 1222.99245256249);
        scales.insert(6, 2445.98490512499);
        scales.insert(5, 4891.96981024998);
        scales.insert(4, 9783.93962049996);
        scales.insert(3, 19567.8792409999);
        scales.insert(2, 39135.7584820001);
        scales.insert(1, 78271.5169639999);
    }

    QPointF point = pointToMercator(QPointF(lng, lat));
    double scale = scales.value(zoom) * 256;
    int x =  qRound((point.x() - topTileFromX) / scale);
    int y =  qRound((topTileFromY - point.y()) / scale);
    return QPoint(x, y);
}

QPoint MapHelper::lngLatToTileTian2(double lng, double lat, int zoom)
{
    double topTileFromX = -180;
    double topTileFromY = 90;

    static QMap<int, double> scales;
    if (scales.count() == 0) {
        scales.insert(18, 5.36441802978515E-06);
        scales.insert(17, 1.07288360595703E-05);
        scales.insert(16, 2.1457672119140625E-05);
        scales.insert(15, 4.29153442382814E-05);
        scales.insert(14, 8.58306884765629E-05);
        scales.insert(13, 0.000171661376953125);
        scales.insert(12, 0.00034332275390625);
        scales.insert(11, 0.0006866455078125);
        scales.insert(10, 0.001373291015625);
        scales.insert(9, 0.00274658203125);
        scales.insert(8, 0.0054931640625);
        scales.insert(7, 0.010986328125);
        scales.insert(6, 0.02197265625);
        scales.insert(5, 0.0439453125);
        scales.insert(4, 0.087890625);
        scales.insert(3, 0.17578125);
        scales.insert(2, 0.3515625);
        scales.insert(1, 0.703125);
    }

    double scale = scales.value(zoom) * 256;
    int x =  qRound((lng - topTileFromX) / scale);
    int y =  qRound((topTileFromY - lat) / scale);
    return QPoint(x, y);
}

QPointF MapHelper::pointToGoogle(const QPointF &point)
{
    QPointF pointf;
    double x = point.x() - 0.0065;
    double y = point.y() - 0.006;
    double z = qSqrt(x * x + y * y) - 0.00002 * qSin(y * M_PI);
    double theta = qAtan2(y, x) - 0.000003 * qCos(x * M_PI);
    pointf.setX(z * qCos(theta));
    pointf.setY(z * qSin(theta));
    return pointf;
}

QPoint MapHelper::lngLatToTileGoogle(double lng, double lat, int zoom)
{
    int x = qFloor((lng + 180) / 360 * qPow(2, zoom));
    int y = qFloor((1 - qLn(qTan(lat * M_PI / 180) + 1 / qCos(lat * M_PI / 180)) / M_PI) / 2 * qPow(2, zoom));
    return QPoint(x, y);
}

QPoint MapHelper::lngLatToTileGoogle2(double lng, double lat, int zoom)
{
    int x = qFloor((lng + 180) / 360 * qPow(2, zoom));
    int y = (1 << zoom) - (qFloor((1 - qLn(qTan(lat * M_PI / 180) + 1 / qCos(lat * M_PI / 180)) / M_PI) / 2 * qPow(2, zoom))) - 1;
    return QPoint(x, y);
}

//圆周率转换量
double x_pi = M_PI * 3000.0 / 180.0;
QPointF MapHelper::gcj2bd(const QPointF &point)
{
    double x = point.x();
    double y = point.y();
    double z = qSqrt(x * x + y * y) + 0.00002 * qSin(y * x_pi);
    double theta = qAtan2(y, x) + 0.000003 * qCos(x * x_pi);
    double lng = z * qCos(theta) + 0.0065;
    double lat = z * qSin(theta) + 0.006;
    return QPointF(lng, lat);
}

QPointF MapHelper::bd2gcj(const QPointF &point)
{
    double x = point.x() - 0.0065;
    double y = point.y() - 0.006;
    double z = qSqrt(x * x + y * y) - 0.00002 * qSin(y * x_pi);
    double theta = qAtan2(y, x) - 0.000003 * qCos(x * x_pi);
    double lng = z * qCos(theta);
    double lat = z * qSin(theta);
    return QPointF(lng, lat);
}

double MapHelper::transformLng(double lng, double lat)
{
    double ret = 300.0 + lat + 2.0 * lng + 0.1 * lat * lat + 0.1 * lat * lng + 0.1 * qSqrt(qAbs(lat));
    ret += (20.0 * qSin(6.0 * lat * M_PI) + 20.0 * qSin(2.0 * lat * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * qSin(lat * M_PI) + 40.0 * qSin(lat / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (150.0 * qSin(lat / 12.0 * M_PI) + 300.0 * qSin(lat / 30.0 * M_PI)) * 2.0 / 3.0;
    return ret;
}

double MapHelper::transformLat(double lng, double lat)
{
    double ret = -100.0 + 2.0 * lat + 3.0 * lng + 0.2 * lng * lng + 0.1 * lat * lng + 0.2 * qSqrt(qAbs(lat));
    ret += (20.0 * qSin(6.0 * lat * M_PI) + 20.0 * qSin(2.0 * lat * M_PI)) * 2.0 / 3.0;
    ret += (20.0 * qSin(lng * M_PI) + 40.0 * qSin(lng / 3.0 * M_PI)) * 2.0 / 3.0;
    ret += (160.0 * qSin(lng / 12.0 * M_PI) + 320 * qSin(lng * M_PI / 30.0)) * 2.0 / 3.0;
    return ret;
}

//卫星椭球坐标投影到平面地图坐标系的投影因子
double a = 6378245.0;
//椭球的偏心率
double ee = 0.00669342162296594323;
QPointF MapHelper::wgs2gcj(const QPointF &point)
{
    double x = point.x();
    double y = point.y();
    double lng = transformLng(y - 35.0, x - 105.0);
    double lat = transformLat(y - 35.0, x - 105.0);

    double rad = y / 180.0 * M_PI;
    double magic = qSin(rad);
    magic = 1 - ee * magic * magic;
    double sqrtMagic = qSqrt(magic);

    lng = x + (lng * 180.0) / (a / sqrtMagic * qCos(rad) * M_PI);
    lat = y + (lat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * M_PI);
    return QPointF(lng, lat);
}

QPointF MapHelper::wgs2bd(const QPointF &point)
{
    //GPS转百度要经过两重转换
    return gcj2bd(wgs2gcj(point));
}
