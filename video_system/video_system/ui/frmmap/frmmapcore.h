#ifndef FRMMAPCORE_H
#define FRMMAPCORE_H

#include <QWidget>
#include "webview.h"
#include "maphelper.h"
#include "mapbaidu.h"

namespace Ui {
class frmMapCore;
}

class frmMapCore : public QWidget
{
    Q_OBJECT

public:
    explicit frmMapCore(QWidget *parent = 0);
    ~frmMapCore();

    //提供接口给外部类使用
    MapBaiDu *getMapBaiDu();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmMapCore *ui;

    //用来控制只载入一次
    bool isShow;
    //文件标识
    QString fileFlag;
    //中心点
    QString centerPoint;

    //浏览器类
    WebView *web;
    //百度地图类
    MapBaiDu *baidu;

public:
    //设置地图保存文件标识
    void setFileFlag(const QString &fileFlag);
    //设置默认中心点坐标
    void setCenterPoint(const QString &centerPoint);

private slots:
    //初始化界面
    void initForm();

public slots:
    //获取是否已经显示过
    bool getIsShow();
    //载入地图
    void loadMap(const QString &http = "");
    //执行js函数
    void runJs(const QString &js);

signals:
    //网页载入完成
    void loadFinished(bool ok);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);
};

#endif // FRMMAPCORE_H
