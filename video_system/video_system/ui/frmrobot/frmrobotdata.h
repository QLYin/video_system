#ifndef FRMROBOTDATA_H
#define FRMROBOTDATA_H

#include <QWidget>

namespace Ui {
class frmRobotData;
}

class frmRobotData : public QWidget
{
    Q_OBJECT

public:
    explicit frmRobotData(QWidget *parent = 0);
    ~frmRobotData();

private:
    Ui::frmRobotData *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化机器人通信服务
    void initServer();
    //初始化表格
    void initTable();
    //初始化表格字段
    void initItem();
    //加载机器人工作数据
    void loadData();
    //加载机器人运行状态
    void loadStatus();

private slots:
    //通用信号,用type来区分
    void receiveData(quint8 type, const QVariant &data);

signals:
    //发出经纬度移动轨迹
    void moveDevice(int id, const QString &lng, const QString &lat);
};

#endif // FRMROBOTDATA_H
