#ifndef FRMROBOTDEBUG_H
#define FRMROBOTDEBUG_H

#include <QWidget>

namespace Ui {
class frmRobotDebug;
}

class frmRobotDebug : public QWidget
{
    Q_OBJECT

public:
    explicit frmRobotDebug(QWidget *parent = 0);
    ~frmRobotDebug();

private:
    Ui::frmRobotDebug *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //添加信息
    void append(int type, const QString &data, bool clear = false);

    //打印串口收发数据
    void sendData(const QString &portName, const QByteArray &data);
    void receiveData(const QString &portName, const QByteArray &data);

    //收到地图缩略图单击传过来的经纬度
    void receiveLngLat(const QString &lnglat);

private slots:
    void on_btnSend_clicked();
    void on_btnClear_clicked();
    void on_txtLngLat_textChanged(const QString &arg1);
};

#endif // FRMROBOTDEBUG_H
