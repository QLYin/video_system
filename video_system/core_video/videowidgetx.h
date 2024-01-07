#ifndef VIDEOWIDGETX_H
#define VIDEOWIDGETX_H

#include "videothread.h"
#include "abstractvideowidget.h"

class VideoWidget : public AbstractVideoWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

protected:
    //窗体尺寸发生变化
    void resizeEvent(QResizeEvent *);

private:
    //按下坐标
    QPoint lastPoint;
    //采集参数
    VideoPara videoPara;
    //采集线程
    VideoThread *videoThread;

public:
    //获取和设置采集参数
    VideoPara getVideoPara() const;
    void setVideoPara(const VideoPara &videoPara);

    //获取是否暂停状态
    bool getIsPause() const;
    //获取是否录像状态
    bool getIsRecord() const;
    //获取是否裁剪状态
    bool getIsCrop() const;

    //获取采集线程对象
    VideoThread *getVideoThread() const;

    //设置重复播放
    void setPlayRepeat(bool playRepeat);
    //设置超时时间
    void setReadTimeout(int readTimeout);

    //设置视频宽高比例
    void setAspect(double width, double height);

    //获取和设置播放位置
    qint64 getPosition();
    void setPosition(qint64 position);

    //获取和设置播放速度
    double getSpeed();
    void setSpeed(double speed);

    //获取和设置音量大小
    int getVolume();
    void setVolume(int volume);

    //获取和设置静音状态
    bool getMuted();
    void setMuted(bool muted);

    //设置逐帧播放
    void setPlayStep(bool playStep);

private slots:
    //重新调整尺寸
    void resize2();
    //线程启动和停止
    void started();
    void finished();

    //播放成功
    void receivePlayStart(int time);
    //播放结束
    void receivePlayFinsh();

    //校验接收数据
    bool checkReceive(bool clear = false);
    //收到一张图片
    void receiveImage(const QImage &image, int time);
    //接收一帧并绘制
    void receiveFrame(int width, int height, quint8 *dataRGB, int type);
    void receiveFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV);
    void receiveFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV);

    //按下处像素坐标
    void receivePoint(int type, const QPoint &point);
    //工具栏按钮单击
    void btnClicked(const QString &btnName);
    //标签信息变化
    void osdChanged();
    //图形信息变化
    void graphChanged();

    //关联事件
    void connectThreadSignal();
    //取消关联
    void disconnectThreadSignal();

public slots:
    //初始化参数
    bool init();
    //打开播放(先停止->设置地址->再播放)
    bool open(const QString &videoUrl);

    //开始播放
    void play();
    //停止播放
    void stop();

    //暂停播放
    void pause();
    //继续播放
    void next();

    //按帧播放
    void step(bool backward = false);
    //抓拍截图
    void snap(const QString &snapName = QString(), bool preview = false);

    //开始录制
    void recordStart(const QString &fileName);
    //暂停录制
    void recordPause();
    //停止录制
    void recordStop();
};

#endif // VIDEOWIDGETX_H
