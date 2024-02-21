#include "miniblink.h"
#include "webview.h"
#include "qapplication.h"
#include "qmessagebox.h"
#include "qdatetime.h"
#include "qtimer.h"
#include "qfile.h"
#include "qvariant.h"
#include "qdebug.h"

#pragma execution_character_set("utf-8")
#define TIMEMS qPrintable(QTime::currentTime().toString("hh:mm:ss zzz"))
static void onLoadingFinish(wkeWebView, void *param, const wkeString, wkeLoadingResult result, const wkeString)
{
    //qDebug() << TIMEMS << "onLoadingFinish" << result;
    //在注册函数的地方就已经传入了类指针
    miniblink *widget = (miniblink *)param;
    //0 = WKE_LOADING_SUCCEEDED, 1 = WKE_LOADING_FAILED, 2 = WKE_LOADING_CANCELED
    widget->loadFinish(result == 0);
}

static jsValue objName_receiveData(jsExecState es, void *param)
{
    if (0 == jsArgCount(es)) {
        return jsUndefined();
    }

    //挨个取出参数,设定的通用方法,只有两个参数
    jsValue arg0 = jsArg(es, 0);
    jsValue arg1 = jsArg(es, 1);
    if (!jsIsString(arg0)) {
        return jsUndefined();
    }

    //可以拿到具体的webview对象用于区别分析
    //jsGetWebView(es);

    //转换对应的数据,还有很多其他参数判断具体参见 wke.h
    QString type = QString::fromStdString(jsToTempString(es, arg0));
    QVariant data;
    if (jsIsNumber(arg1)) {
        data = jsToInt(es, arg1);
    } else if (jsIsBoolean(arg1)) {
        data = jsToBoolean(es, arg1);
    } else if (jsIsString(arg1)) {
        data = QString::fromStdString(jsToTempString(es, arg1));
    } else if (jsIsArray(arg1)) {
        //下面的转换需要根据实际项目应用进行处理
        data = QString::fromStdString(jsToTempString(es, arg1));
        if (type == "boundary") {

        } else if (type == "routepoints") {

        }
    }

    //在注册函数的地方就已经传入了类指针
    miniblink *widget = (miniblink *)param;
    widget->receiveData(type, data);
    return jsUndefined();
}

miniblink::miniblink(QWidget *parent) : QWidget(parent)
{
    //加载动态库文件
    this->init();
    //异步初始化浏览器控件(同步执行的话在Qt6.4以后会和opengl冲突导致opengl黑屏)
    QMetaObject::invokeMethod(this, "initWebView", Qt::QueuedConnection);
}

void miniblink::init()
{
    //全局只需要初始化一次
    static bool isInit = false;
    if (!isInit) {
        isInit = true;
        //不同的构建套件位数加载不同的动态库
#ifdef Q_OS_WIN64
        QString flag = "64";
        QString file = qApp->applicationDirPath() + "/miniblink_64.dll";
#else
        QString flag = "32";
        QString file = qApp->applicationDirPath() + "/miniblink.dll";
#endif
        //如果文件不存在则提示
        if (!QFile(file).exists()) {
            QMessageBox::critical(0, "错误", file + "\n文件不存在请先拷贝!");
            return;
        }

        const wchar_t *path = reinterpret_cast<const wchar_t *>(file.utf16());
        wkeSetWkeDllPath(path);
        bool ok = wkeInitialize();
        qDebug() << TIMEMS << QString("init miniblink_%1 %2").arg(flag).arg(ok ? "ok" : "error");
    }
}

void miniblink::release()
{
    wkeFinalize();
}

void miniblink::showEvent(QShowEvent *)
{
    //最开始这个设置放在构造函数中,后面发现会覆盖,于是想到一个办法就是哪个显示就绑定哪个
    //由于偷懒网页中写的是通用的方法所以会冲突,如果不同的方法就没有问题
    //miniblink内核采用了引用计数,为了节省资源,所以后面绑定的一样的则取最后一个
    //只能暂时用这个方法满足目前自身所有项目需求
    //注册通用的接收数据的方法,一定要放在这里在网页加载前执行
    //绑定js函数,让js主动调用C++函数,两个名字可以不一样但是参数必须一致
    wkeJsBindFunction("objName_receiveData", objName_receiveData, this, 2);
}

void miniblink::resizeEvent(QResizeEvent *)
{
    //处理下高分屏下的系统缩放
    qreal ratio = WebView::getDevicePixelRatio();
    wkeResize(webView, this->width() * ratio, this->height() * ratio);
}

void miniblink::setBgColor(const QColor &color)
{
    wkeViewSettings setting;
    setting.bgColor = color.rgba();
    wkeSetViewSettings(webView, &setting);
    //wkeSetTransparent(webView, true);
}

void miniblink::loadFinish(bool ok)
{
    emit loadFinished(ok);
}

void miniblink::receiveData(const QString &type, const QVariant &data)
{
    emit receiveDataFromJs(type, data);
}

void miniblink::initWebView()
{
    //创建一个浏览器控件放入句柄
    webView = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, (HWND)this->winId(), 0, 0, this->width(), this->height());
    //关联完成信号
    wkeOnLoadingFinish(webView, onLoadingFinish, this);
    //设置浏览器控件可见
    wkeShowWindow(webView, true);
    //设置右键菜单是否可见
    wkeSetContextMenuEnabled(webView, false);
    //设置是否允许触屏
    wkeSetTouchEnabled(webView, false);
    //还有更多参数说明参见 https://miniblink.net/views/doc/APIDocument/API/index.html
    //wkeJsBindFunction("objName_receiveData", objName_receiveData, this, 2);
}

void miniblink::load(const QString &url, bool file)
{
    QByteArray data = url.toUtf8();
    const char *temp = data.data();
    if (file) {
        wkeLoadFile(webView, temp);
    } else {
        wkeLoadURL(webView, temp);
    }
}

void miniblink::setHtml(const QString &html, const QString &baseUrl)
{
    QByteArray dataHtml = html.toUtf8();
    QByteArray dataUrl = baseUrl.toUtf8();
    wkeLoadHtmlWithBaseUrl(webView, dataHtml.data(), dataUrl.data());
}

void miniblink::runJs(const QString &js)
{
    QByteArray data = js.toUtf8();
    wkeRunJS(webView, data.data());
}
