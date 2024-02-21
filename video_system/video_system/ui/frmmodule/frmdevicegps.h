#ifndef FRMDEVICEGPS_H
#define FRMDEVICEGPS_H

#include <QWidget>
class frmMapCore;
class MapBaiDu;

namespace Ui {
class frmDeviceGps;
}

class frmDeviceGps : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceGps(QWidget *parent = 0);
    ~frmDeviceGps();

private:
    Ui::frmDeviceGps *ui;

    //百度地图界面内核
    frmMapCore *map;
    //百度地图通信类
    MapBaiDu *baidu;

    //所有GPS点
    QList<QStringList> listPoints;

private slots:
    //初始化界面
    void initForm();
    //加载设备
    void loadDevice();
    //移动设备
    void moveDevice();
    void moveDevice(int id, int count, double lngValue, double latValue, double lngOffset, double latOffset);

private slots:
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);
    //执行js函数
    void runJs(const QString &js);

    //传入经纬度移动轨迹
    void moveDevice(int id, const QString &lng, const QString &lat);
};

#endif // FRMDEVICEGPS_H
