#ifndef FRMMAPDEVICE_H
#define FRMMAPDEVICE_H

#include <QWidget>
#include <QDateTime>
class frmMapCore;
class MapBaiDu;
class QScreen;

namespace Ui {
class frmMapDevice;
}

class frmMapDevice : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapDevice(QWidget *parent = 0);
    ~frmMapDevice();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmMapDevice *ui;

    //百度地图界面内核
    frmMapCore *map;
    //百度地图通信类
    MapBaiDu *baidu;
    //最后单击事件
    QDateTime lastTime;

private slots:
    //初始化窗体数据
    void initForm();
    //加载设备
    void loadDevice();
    //加载摄像机名称
    void loadIpcName();

public slots:
    //设置是否是本地离线地图
    void setMapLocal(bool mapLocal);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);
    //执行js函数
    void runJs(const QString &js);

private slots:
    void on_btnZoom_clicked();
    void on_btnUpdate_clicked();
    void on_btnDevice_clicked();
    void on_btnDistance_clicked();
};

#endif // FRMMAPDEVICE_H
