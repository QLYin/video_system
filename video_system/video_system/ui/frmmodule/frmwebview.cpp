#include "frmwebview.h"
#include "ui_frmwebview.h"
#include "qthelper.h"

frmWebView::frmWebView(QWidget *parent) : QWidget(parent), ui(new Ui::frmWebView)
{
    ui->setupUi(this);
    this->initForm();
}

frmWebView::~frmWebView()
{
    delete ui;
}

void frmWebView::showEvent(QShowEvent *)
{
    if (!isShow) {
        isShow = true;
        QTimer::singleShot(100, this, SLOT(loadWeb()));
    }
}

void frmWebView::initForm()
{
    isShow = false;
    url = "http://www.baidu.com";

    //初始化通用浏览器控件
    web = new WebView(this);
    web->setLayout(ui->layout);
#ifdef webkit
    web->setBgColor("#FFFFFF");
#endif

    //关联加载完成信号+接收数据信号
    connect(web, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(web, SIGNAL(receiveDataFromJs(QString, QVariant)), this, SLOT(receiveDataFromJs(QString, QVariant)));
}

void frmWebView::loadWeb()
{
    web->load(url);
}

void frmWebView::loadFinished(bool ok)
{

}

void frmWebView::receiveDataFromJs(const QString &type, const QVariant &data)
{

}

void frmWebView::setUrl(const QString &url)
{
    this->url = url;
}
