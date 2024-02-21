#ifndef BANNERWIDGET_H
#define BANNERWIDGET_H

#include "widgethead.h"

class BannerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BannerWidget(QWidget *parent = 0);

private:
    //图形字体
    QFont iconFont;
    //窗体参数
    BannerPara bannerPara;

    //是否录制中
    bool isRecord;
    //是否裁剪中
    bool isCrop;
    //最后的消息文字
    QString text;

    //标签控件
    QLabel *label;
    //按钮对象集合
    QList<QPushButton *> btns;

public:
    //获取录像状态
    bool getIsRecord() const;
    //获取裁剪状态
    bool getIsCrop() const;
    //获取窗体参数
    BannerPara getBannerPara();

private slots:
    //处理按钮单击
    void btnClicked();

    //播放结束
    void receivePlayFinsh();
    //静音状态
    void receiveMuted(bool muted);
    //录制状态
    void recorderStateChanged(const RecorderState &state, const QString &file);

public slots:
    //设置参数
    void setBannerPara(const BannerPara &bannerPara);
    //初始化按钮
    void initButton();
    //初始化样式
    void initStyle();

    //显示文字信息
    void showInfo(const QString &text);
    //切换按钮图标和名称
    void changeStatus(const QString &objNameSrc, const QString &objNameDst, int icon);

signals:
    //工具栏按钮单击
    void btnClicked(const QString &btnName);
};

#endif // BANNERWIDGET_H
