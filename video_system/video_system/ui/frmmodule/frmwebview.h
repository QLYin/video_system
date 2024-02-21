#ifndef FRMWEBVIEW_H
#define FRMWEBVIEW_H

#include <QWidget>
#include "webview.h"

namespace Ui {
class frmWebView;
}

class frmWebView : public QWidget
{
    Q_OBJECT

public:
    explicit frmWebView(QWidget *parent = 0);
    ~frmWebView();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmWebView *ui;

    //用来控制只载入一次
    bool isShow;
    //网页地址
    QString url;
    //通用浏览器组件
    WebView *web;

private slots:
    //初始化界面
    void initForm();
    //加载网页
    void loadWeb();

private slots:
    //网页载入完成
    void loadFinished(bool ok);
    //收到网页发出来的数据
    void receiveDataFromJs(const QString &type, const QVariant &data);

public:
    //设置要加载的网页地址
    void setUrl(const QString &url);
};

#endif // FRMWEBVIEW_H
