#ifndef FRMMAPROUTE_H
#define FRMMAPROUTE_H

#include <QWidget>
class frmMapCore;
class MapBaiDu;
class QTableWidget;

namespace Ui {
class frmMapRoute;
}

class frmMapRoute : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapRoute(QWidget *parent = 0);
    ~frmMapRoute();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmMapRoute *ui;

    //百度地图界面内核
    frmMapCore *map;
    //百度地图通信类
    MapBaiDu *baidu;

    //设备名称
    QString name;
    //模拟轨迹定时器
    QTimer *timer;
    //路径点集合用来重新绘制路径确认是否正确
    QList<QStringList> routeDatas;

    //获取当前表格控件
    QTableWidget *getTableWidget();

private slots:
    //初始化界面
    void initForm();
    //初始化配置
    void initConfig();
    //保存配置
    void saveConfig();

    //显示信息
    void setInfo(int angle, int index, int count);

    //初始化表格控件
    void initTable(QTableWidget *tableWidget);
    //添加数据到表格
    void addItem(QTableWidget *tableWidget, int index, const QString &point);

private slots:
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);
    //执行js函数
    void runJs(const QString &js);

private slots:
    void on_btnSearchData_clicked();
    //移动设备点轨迹
    void moveMarker();
    void on_btnTestData_clicked();

    void on_btnCheckData_clicked();
    void on_btnDrawData_clicked();

    void on_cboxRouteType_currentIndexChanged(int index);
};

#endif // FRMMAPROUTE_H
