#include "frmmapcore.h"
#include "ui_frmmapcore.h"
#include "qthelper.h"

frmMapCore::frmMapCore(QWidget *parent) : QWidget(parent), ui(new Ui::frmMapCore)
{
    ui->setupUi(this);
    this->initForm();
}

frmMapCore::~frmMapCore()
{
    delete ui;
}

MapBaiDu *frmMapCore::getMapBaiDu()
{
    return this->baidu;
}

void frmMapCore::showEvent(QShowEvent *)
{
    if (!isShow) {
        isShow = true;
        QTimer::singleShot(100, this, SLOT(loadMap()));
    }
}

void frmMapCore::setFileFlag(const QString &fileFlag)
{
    this->fileFlag = fileFlag;
}

void frmMapCore::setCenterPoint(const QString &centerPoint)
{
    this->centerPoint = centerPoint;
}

void frmMapCore::initForm()
{
    isShow = false;
    fileFlag = "map_baidu";
    centerPoint = AppConfig::MapCenter;

    //初始化百度地图类
    baidu = new MapBaiDu(this);
    baidu->setMapVersionKey(AppConfig::MapVersionKey);
    //初始化通用浏览器控件
    web = new WebView(this);
    web->setLayout(ui->layout);
#ifdef webkit
    web->setBgColor("#FFFFFF");
#endif

    //关联加载完成信号+接收数据信号
    connect(web, SIGNAL(loadFinished(bool)), this, SIGNAL(loadFinished(bool)));
    connect(web, SIGNAL(receiveDataFromJs(QString, QVariant)), this, SIGNAL(receiveDataFromJs(QString, QVariant)));
}

bool frmMapCore::getIsShow()
{
    return this->isShow;
}

void frmMapCore::loadMap(const QString &http)
{
    //设置保存文件名称
    QString fileName = QString("%1/config/%2.html").arg(QtHelper::appPath()).arg(fileFlag);
    QString url = "file:///" + fileName;

    baidu->setSaveFile(SaveFile);
    baidu->setFileName(fileName);
    //设置默认的中心点坐标,建议采用中心点坐标的方式
    baidu->setMapCenterPoint(centerPoint);

    //如果传入了网址则取网址
    if (!http.isEmpty()) {
        url = http;
        baidu->setSaveFile(true);
    }

    //下面为两种方式加载网页,如果内容为空则加载网页文件否则加载内容
    //一般为了保密建议加载内容,这样看不到生成的网页文件
    //可能在linux上的webkit内核需要用load的方式加载
    QString html = baidu->newMap();
    if (baidu->getSaveFile()) {
        web->load(url);
    } else {
        //如果是从本地加载网页文件记得一定要手动设置这个本地路径
        QString baseUrl = QString("%1/config/").arg(QtHelper::appPath());
        web->load("", html, baseUrl);
    }
}

void frmMapCore::runJs(const QString &js)
{
    web->runJs(js);
}
