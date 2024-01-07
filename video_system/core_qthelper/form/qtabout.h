#ifndef QTABOUT_H
#define QTABOUT_H

#pragma execution_character_set("utf-8")
#include <QDialog>
#include "singleton.h"

namespace Ui {
class QtAbout;
}

//关于信息结构体,可以自行增加其他内容
struct AboutInfo {
    QString title;      //软件标题
    QString version;    //软件版本
    QString copyright;  //版权所有
    QString http;       //官方网址
    QString tel;        //联系电话
    QString logo;       //logo图片

    AboutInfo() {
        title = "Qt项目一站式解决方案";
        version = "V20250101";
        copyright = "上海物联网技术研究中心";
        http = "https://qtchina.blog.csdn.net/";
        tel = "021-12345678 13812345678";
        logo = ":/image/logo.png";
    }
};

class QtAbout : public QDialog
{
    Q_OBJECT SINGLETON_DECL(QtAbout)

public:
    explicit QtAbout(QWidget *parent = 0);
    ~QtAbout();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::QtAbout *ui;
    //关闭窗体定时器
    QTimer *timer;

private slots:
    //初始化窗体数据
    void initForm();

public:
    //设置关于信息结构体数据
    void setInfo(const AboutInfo &info, int timeout = 0);
};

#endif // QTABOUT_H
