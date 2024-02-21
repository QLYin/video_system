#include "webview.h"
#include "qapplication.h"
#include "qevent.h"
#include "qdatetime.h"
#include "qdebug.h"
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include "qscreen.h"
#endif

#ifdef webkit
#include <QtWebKit>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWebKitWidgets>
#endif
#elif webengine
#include <QtWebEngineWidgets>
#elif webie
#include <QAxWidget>
#elif webminiblink
#include "miniblink.h"
#endif

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
WebView::WebView(QWidget *parent) : QObject(parent)
{
    //初始化web控件配置信息
    WebView::initWebSetting();
    lastTime = QDateTime::currentDateTime();

    //数据交互类
    webJsData = new WebJsData(this);
    connect(webJsData, SIGNAL(receiveDataFromJs(QString, QVariant)),
            this, SIGNAL(receiveDataFromJs(QString, QVariant)));

#ifdef webkit
    webView = new QWebView(parent);
#elif webengine
    webView = new QWebEngineView(parent);
    //设置对应的通信通道
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("objName", webJsData);
    webView->page()->setWebChannel(channel);
#elif webie
    webView = new QAxWidget(parent);
    //注册浏览器控件
    webView->setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
    //不显示警告信息
    webView->setProperty("DisplayAlerts", false);
    //不显示滚动条
    webView->setProperty("DisplayScrollBars", true);
#elif webminiblink
    webView = new miniblink(parent);
#endif

    //关联加载完成信号
#if defined(webkit) || defined(webengine)
    connect(webView->page(), SIGNAL(loadFinished(bool)), this, SLOT(loadFinish(bool)));
#elif webminiblink
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinish(bool)));
    //miniblink通过浏览器控件那边传过来
    connect(webView, SIGNAL(receiveDataFromJs(QString, QVariant)), this, SIGNAL(receiveDataFromJs(QString, QVariant)));
#endif

    //设置没有右键菜单
#if defined(webkit) || defined(webengine) || defined(webie) || defined(webminiblink)
    webView->setContextMenuPolicy(Qt::NoContextMenu);
#endif

    //安装事件过滤器
#ifdef webminiblink
    webView->installEventFilter(this);
#endif
}

void WebView::initWebSetting()
{
    //全局配置只需要设置一次就行
    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        QString webcore = "other";
#ifdef webkit
        webcore = "webkit";
        QWebSettings *webSetting = QWebSettings::globalSettings();
        webSetting->setAttribute(QWebSettings::PluginsEnabled, true);
        webSetting->setAttribute(QWebSettings::JavascriptEnabled, true);
        webSetting->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
        webSetting->setAttribute(QWebSettings::LocalStorageEnabled, true);
        webSetting->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
#elif webengine
        webcore = "webengine";
        QWebEngineSettings *webSetting = QWebEngineProfile::defaultProfile()->settings();
        webSetting->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        webSetting->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        webSetting->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
        webSetting->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        webSetting->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
#elif webie
        webcore = "ie";
#elif webminiblink
        webcore = "miniblink";
#endif
        qDebug() << TIMEMS << QString("init webview ok, core: %1").arg(webcore);
    }
}

qreal WebView::getDevicePixelRatio()
{
    qreal ratio = 1;
    //5.5开始才有 devicePixelRatio
#if (QT_VERSION >= QT_VERSION_CHECK(5,5,0))
    //找到当前屏幕索引
    int screenIndex = 0;
    int screenCount = qApp->screens().count();
    if (screenCount > 1) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        for (int i = 0; i < screenCount; ++i) {
            if (qApp->screens().at(i)->geometry().contains(pos)) {
                screenIndex = i;
                break;
            }
        }
    }
    //当前屏幕对应的缩放比
    ratio = qApp->screens().at(screenIndex)->devicePixelRatio();
#endif
    return ratio;
}

bool WebView::eventFilter(QObject *watched, QEvent *event)
{
    //测试发现miniblink内核(其他内核正常),网页中的右键有消息会传给父类
    if (event->type() == QEvent::MouseButtonPress) {
        return true;
    }
    return QObject::eventFilter(watched, event);
}

void WebView::loadFinish(bool ok)
{
    //过滤掉重复的信号(miniblink内核可能会重复发两次加载完成信号)
    QDateTime now = QDateTime::currentDateTime();
    int offset = lastTime.msecsTo(now);
    if (offset <= 200) {
        return;
    }

#ifdef webkit
    webView->page()->mainFrame()->addToJavaScriptWindowObject("objName", webJsData);
#endif

    lastTime = now;
    emit loadFinished(ok);
}

void WebView::setLayout(QLayout *layout)
{
    //把控件加入到布局
#if defined(webkit) || defined(webengine) || defined(webie) || defined(webminiblink)
    layout->addWidget(webView);
#endif
}

void WebView::setVisible(bool visible)
{
#if defined(webkit) || defined(webengine) || defined(webie) || defined(webminiblink)
    webView->setVisible(visible);
#endif
}

void WebView::setBgColor(const QColor &color)
{
#if defined(webkit) || defined(webie)
    webView->setStyleSheet(QString("background:rgba(%1,%2,%3,%4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
#elif webengine
    webView->page()->setBackgroundColor(color);
#elif webminiblink
    webView->setBgColor(color);
#endif
}

void WebView::load(const QString &url, const QString &html, const QString &baseUrl)
{
    //如果存在网址则优先直接加载网址
    if (!url.isEmpty()) {
#if defined(webkit) || defined(webengine)
        webView->load(QUrl(url));
#elif webie
        webView->dynamicCall("Navigate(const QString&)", url);
#elif webminiblink
        webView->load(url);
#endif
    } else {
#if defined(webkit) || defined(webengine)
        webView->setHtml(html, QUrl(baseUrl));
#elif webminiblink
        webView->setHtml(html, baseUrl);
#endif
    }
}

void WebView::runJs(const QString &js)
{
    if (js.isEmpty()) {
        return;
    }
#ifdef webkit
    webView->page()->mainFrame()->evaluateJavaScript(js);
#elif webengine
    webView->page()->runJavaScript(js);
#elif webminiblink
    webView->runJs(js);
#endif
}
