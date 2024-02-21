#ifndef FRMCONFIGIPCSEARCH_H
#define FRMCONFIGIPCSEARCH_H

#include "onvifhead.h"

namespace Ui {
class frmConfigIpcSearch;
}

class frmConfigIpcSearch : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigIpcSearch(QWidget *parent = 0);
    ~frmConfigIpcSearch();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigIpcSearch *ui;

    //onvif搜索对象+搜索到的设备集合
    OnvifSearch *onvifSearch;
    QList<OnvifDevice *> devices;

    //全选框+设备信息集合
    QCheckBox *ckAll;
    QList<QStringList> deviceInfos;

    //排队处理挨个获取对应的信息
    QTimer *timer;
    QList<int> listRow;
    QList<OnvifDevice *> listDevice;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化数据
    void initData();

    //加载配置文件
    void initConfig();
    //保存配置文件
    void saveConfig();
    //清空设备
    void clear();

private slots:
    //复选框选中变化
    void stateChanged(int arg1);
    //禁用单元格整行
    void disableRow(const QString &addr, int row);

    //onvif类发过来的信号(错误提示/收到设备信息返回)
    void receiveError(const QString &data);
    void receiveDevice(const OnvifDeviceInfo &deviceInfo);

private slots:
    //搜索完成信号
    void searchFinsh();
    //指定广播还是单播搜索
    void searchDevice(bool one, int interval = 0);
    void on_btnSearchAll_clicked();
    void on_btnSearchOne_clicked();

    //对视频流地址加上用户信息
    void addUser(QString &rtspAddr, const QString &userName, const QString &userPwd);
    //获取所有设备媒体信息
    void getMedia();
    //对选中设备执行获取信息
    void getMedia(int row, OnvifDevice *device);
    void on_btnMediaAll_clicked();
    void on_btnMediaOne_clicked();

    //添加设备到表格
    void addDevice(int row, bool one);
    void on_btnAddAll_clicked();
    void on_btnAddOne_clicked();

signals:
    //发出信号通知接收到设备
    void addDevices(const QList<QStringList> &deviceInfos);
};

#endif // FRMCONFIGIPCSEARCH_H
