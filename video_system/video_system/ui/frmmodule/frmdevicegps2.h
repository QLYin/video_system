#ifndef FRMDEVICEGPS2_H
#define FRMDEVICEGPS2_H

#include <QWidget>
class frmMapCore;
class MapBaiDu;

namespace Ui {
class frmDeviceGps2;
}

class frmDeviceGps2 : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceGps2(QWidget *parent = 0);
    ~frmDeviceGps2();

private:
    Ui::frmDeviceGps2 *ui;

    //百度地图界面内核
    frmMapCore *map;
    //百度地图通信类
    MapBaiDu *baidu;

    //GPS设备名称集合
    QList<QString> listName;
    //GPS设备颜色集合
    QList<QString> listColor;
    //移动到当前点的索引
    QList<int> listIndex;
    //GPS设备坐标集合
    QList<QStringList> listPoints;

    //是否初始化过
    bool isInit;
    //定时器模拟轨迹
    QTimer *timer;

private slots:
    //初始化界面
    void initForm();
    //执行js函数
    void runJs(const QString &js);

    //初始化GPS数据
    void initData();
    //加载GPS文件
    void loadData(const QString &fileName);

    //初始化设备
    void initMarker();
    //移动设备
    void moveMarker();
};

#endif // FRMDEVICEGPS2_H
