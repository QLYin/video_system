#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
#include <QLayout>
#include <QUrl>
#include <QColor>
#include <QDateTime>
#include "webjsdata.h"

#ifdef webkit
class QWebView;
#elif webengine
class QWebEngineView;
#elif webie
class QAxWidget;
#elif webminiblink
class miniblink;
#endif

#if (defined savehtmlfile) || (defined webie)
#define SaveFile true
#else
#define SaveFile false
#endif

//浏览器控件核心类
class WebView : public QObject
{
    Q_OBJECT
public:
    explicit WebView(QWidget *parent = 0);
    //初始化web控件配置信息
    static void initWebSetting();
    //获取高分屏对应的缩放比
    static qreal getDevicePixelRatio();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    //最后的加载完成时间
    QDateTime lastTime;
    //js数据交互类
    WebJsData *webJsData;

    //不同的浏览器内核
#ifdef webkit
    QWebView *webView;
#elif webengine
    QWebEngineView *webView;
#elif webie
    QAxWidget *webView;
#elif webminiblink
    miniblink *webView;
#endif

private slots:
    //网页载入完成
    void loadFinish(bool ok);

public slots:
    //设置放置浏览器控件的布局
    void setLayout(QLayout *layout);
    //设置控件是否可见
    void setVisible(bool visible);
    //设置背景透明
    void setBgColor(const QColor &color);

    //加载网页
    void load(const QString &url, const QString &html = "", const QString &baseUrl = "");
    //执行js函数
    void runJs(const QString &js);

signals:
    //网页载入完成
    void loadFinished(bool ok);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);
};

#endif // WEBVIEW_H
