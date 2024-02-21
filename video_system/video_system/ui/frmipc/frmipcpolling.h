#ifndef FRMIPCPOLLING_H
#define FRMIPCPOLLING_H

#include <QWidget>
#include <QLabel>
class frmVideoPanel;

namespace Ui {
class frmIpcPolling;
}

class frmIpcPolling : public QWidget
{
    Q_OBJECT

public:
    explicit frmIpcPolling(QWidget *parent = 0);
    ~frmIpcPolling();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::frmIpcPolling *ui;

    //当前轮询通道索引
    int pollIndex;
    //轮询次数统计
    int pollCount;
    //轮询url地址
    QStringList pollUrls;
    //轮询定时器
    QTimer *timerPoll;
    //独立轮询信息标签
    QLabel label;

private slots:
    //初始化窗体数据
    void initForm();
    //加载配置文件
    void initConfig();
    //保存配置文件
    void saveConfig();

    //开启视频轮询
    void polling();
    //显示轮询信息
    void setText();

private slots:
    //获取轮询地址集合
    void getPollUrls();
    void on_btnPollStart_clicked();
    void on_btnPollPause_clicked();
};

#endif // FRMIPCPOLLING_H
