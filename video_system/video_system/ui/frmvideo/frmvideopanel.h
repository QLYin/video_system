#ifndef FRMVIDEOPANEL_H
#define FRMVIDEOPANEL_H

#include <QWidget>

class QMenu;
class VideoBox;
class VideoWidget;
class frmIpcPolling;
class frmVideoPanelTool;

namespace Ui {
class frmVideoPanel;
}

class frmVideoPanel : public QWidget
{
    Q_OBJECT

public:
    friend class frmIpcPolling;
    friend class frmVideoPanelTool;

    explicit frmVideoPanel(QWidget *parent = 0);
    ~frmVideoPanel();    

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmVideoPanel *ui;

    //是否存在视频
    bool videoExist;
    //视频是否按下
    bool videoPressed;
    //视频移动
    bool videoMove;

    //右键菜单
    QMenu *videoMenu;
    //全屏动作
    QAction *actionFull;
    //轮询动作
    QAction *actionPoll;

    //通道交换中鼠标光标
    QPixmap pixCursor;
    //重新启动所有通道定时器
    QTimer *timerRestart;
    //通道布局盒子
    VideoBox *videoBox;

    //当前按下选中视频控件
    VideoWidget *videoSelect;
    //当前等待交换视频控件
    VideoWidget *videoSwap;
    //所有视频控件集合
    QList<VideoWidget *> videoWidgets;

    //底部操作工具栏
    frmVideoPanelTool *videoPanelTool;

private slots:
    //初始化窗体界面数据
    void initForm();
    //初始化图标
    void initIcon();
    //初始化视频控件
    void initVideo();
    //改变视频控件参数
    void changeVideoConfig();
    //初始化视频管理参数
    void initVideoManage();
    //初始化其他比如人工智能类等
    void initOther();

    //重新刷新所有通道
    void restartVideo();
    void restartVideo2();

    //初始化右键菜单
    void initMenu();
    //设置视频控件对象到视频布局盒子
    void setWidgets();

    //全屏
    void full();
    //自动全屏
    void autoFull();
    //执行是否全屏
    void doFull(bool full);

private slots:
    //开始播放和停止播放所有视频
    void play(const QStringList &urls);
    void play_video_all();
    void close_video_all();

    //删除当前视频和删除所有视频
    void delete_video_one();
    void delete_video_all();

    //截图当前视频和删除所有视频
    void snapshot_video_one();
    void snapshot_video_all();

    //额外的菜单
    void get_video_gps();
    void show_video_config();

    //悬浮条按钮单击
    void btnClicked(const QString &btnName);
    //接收到拖曳文件
    void fileDrag(const QString &url);

    //画面布局切换信号
    void changeVideo(int type, const QString &videoType, bool videoMax);

signals:
    void pollStart();
};

#endif // FRMVIDEOPANEL_H
