#ifndef ABSTRACTSAVETHREAD_H
#define ABSTRACTSAVETHREAD_H

#include "widgethead.h"

class AbstractSaveThread : public QThread
{
    Q_OBJECT
public:
    //是否打印线程消息
    static bool debugInfo;
    //直接写入还是排队写入
    static bool directSave;

    explicit AbstractSaveThread(QObject *parent = 0);
    ~AbstractSaveThread();

protected:
    //打开后休息一下
    virtual void checkOpen();
    //线程执行内容
    virtual void run();

protected:
    //数据锁
    QMutex mutex;
    //停止线程标志位
    volatile bool stopped;
    //打开是否成功
    volatile bool isOk;
    //暂停写入标志位
    volatile bool isPause;

    //错误计数
    int errorCount;
    //唯一标识
    QString flag;
    //文件对象
    QFile file;
    //文件名称
    QString fileName;

    //保存类型
    SaveMode saveMode;
    //保存视频文件类型
    SaveVideoType saveVideoType;
    //保存音频文件类型
    SaveAudioType saveAudioType;

    //视频相关参数(宽度/高度/帧率/旋转角度)
    int videoWidth;
    int videoHeight;
    qreal frameRate;
    int rotate;

    //音频相关参数(采样率/通道数/品质)
    int sampleRate;
    int channelCount;
    int profile;

public:
    //是否已经打开
    bool getIsOk();
    //是否处于暂停
    bool getIsPause();
    //获取保存文件
    QString getFileName();
    //删除文件
    void deleteFile(const QString &fileName);

    //获取和设置唯一标识(用来打印输出方便区分)
    QString getFlag();
    void setFlag(const QString &flag);

    //获取和设置保存类型
    SaveMode getSaveMode();
    void setSaveMode(SaveMode saveMode);

    //统一格式打印信息
    void debug(const QString &head, const QString &msg);

private slots:
    //初始化
    virtual bool init();
    //保存数据
    virtual void save();
    //关闭释放
    virtual void close();

public slots:
    //开始保存
    virtual void open(const QString &fileName);
    //暂停保存
    virtual void pause();
    //停止保存
    virtual void stop();

signals:
    //保存成功
    void receiveSaveStart();
    //保存结束
    void receiveSaveFinsh();
    //保存失败
    void receiveSaveError(int error);
};

#endif // ABSTRACTSAVETHREAD_H
