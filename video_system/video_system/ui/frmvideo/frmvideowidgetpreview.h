#ifndef FRMVIDEOWIDGETPREVIEW_H
#define FRMVIDEOWIDGETPREVIEW_H

#include <QDialog>
#include <QDateTime>
#include "singleton.h"
class VideoWidget;

namespace Ui {
class frmVideoWidgetPreview;
}

class frmVideoWidgetPreview : public QDialog
{
    Q_OBJECT SINGLETON_DECL(frmVideoWidgetPreview)

public:
    explicit frmVideoWidgetPreview(QWidget *parent = 0);
    ~frmVideoWidgetPreview();

protected:
    void closeEvent(QCloseEvent *);
    void showEvent(QShowEvent *);

private:
    Ui::frmVideoWidgetPreview *ui;

    //视频控件
    VideoWidget *videoWidget;

    //唯一标识(用于存储录像文件最前面标识比如ch01)
    QString flag;
    //录像文件名称
    QString fileName;

    //录像时间
    int recordTime;
    //最后开始的时间
    QDateTime lastTime;
    //录像定时器
    QTimer *timerRecord;

private slots:
    //初始化窗体数据
    void initForm();
    //校验录像
    void checkRecord();
    //播放成功
    void receivePlayStart(int time);

public slots:
    //打开播放地址(带唯一标识/录像时间)
    void open(const QString &url, const QString &flag = QString(), int recordTime = 0, const QString &fileName = QString());
};

#endif // FRMVIDEOWIDGETPREVIEW_H
