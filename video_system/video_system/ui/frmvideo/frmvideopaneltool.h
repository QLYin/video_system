#ifndef FRMVIDEOPANELTOOL_H
#define FRMVIDEOPANELTOOL_H

#include <QWidget>

class WidgetSound;
class frmVideoPanel;
class QButtonGroup;
class QAbstractButton;

namespace Ui {
class frmVideoPanelTool;
}

class frmVideoPanelTool : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoPanelTool(QWidget *parent = 0);
    ~frmVideoPanelTool();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmVideoPanelTool *ui;

    //图形字体大小
    int iconSize;
    //报警图标闪烁定时器
    QTimer *timerAlarm;

    //音量条面板+音量条控件+静音按钮
    WidgetSound *widgetSound;
    //通道按钮分组
    QButtonGroup *btnGroup;

private slots:
    //初始化界面
    void initForm();
    //初始化哪些按钮可见
    void initVisible();
    //初始化按钮图标
    void initIcon();
    //初始化声音控件
    void initSound();
    //按钮单击事件
    void buttonClicked();

    //报警图标开始闪烁
    void startAlarm();
    //报警图标停止闪烁
    void stopAlarm();
    //报警闪烁
    void doAlarm();

private slots:
    //音量改变
    void soundValueChanged(int value);
    //静音切换
    void soundMutedChanged();
    //通道切换
    void changeVideo(int channel);

    //选中通道
    void selectVideo(int channel);
    //指定通道编号加载视频地址
    void loadVideo(int channel, const QString &url);
    //交换通道
    void changeVideo(int channel1, int channel2);

    //执行全屏快捷键
    void keyPressed(const QString &keys);
    //找到当前空闲的通道
    int getFreeChannel();
    //树状列表双击
    void itemDoubleClicked(const QString &url, const QString &text, bool isNvr);
};

#endif // FRMVIDEOPANELTOOL_H
