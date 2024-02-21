#ifndef FRMROBOTDEBUG2_H
#define FRMROBOTDEBUG2_H

#include <QWidget>

namespace Ui {
class frmRobotDebug2;
}

class frmRobotDebug2 : public QWidget
{
    Q_OBJECT

public:
    explicit frmRobotDebug2(QWidget *parent = 0);
    ~frmRobotDebug2();

private:
    Ui::frmRobotDebug2 *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //添加信息
    void append(int type, const QString &data, bool clear = false);
    //校验是否选中
    bool checkNotPrint(const QString &portName, int &type, QString &flag);

    //打印串口收发数据
    void sendData(const QString &portName, const QByteArray &data);
    void receiveData(const QString &portName, const QByteArray &data);
};

#endif // FRMROBOTDEBUG2_H
