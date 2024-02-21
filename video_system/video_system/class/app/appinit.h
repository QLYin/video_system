#ifndef APPINIT_H
#define APPINIT_H

#include <QObject>
#include "singleton.h"

class AppInit : public QObject
{
    Q_OBJECT SINGLETON_DECL(AppInit)

public:
    explicit AppInit(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool hideCursor;    //记住是否已经隐藏鼠标

public slots:
    void checkCursor(); //定时判断是否超时隐藏鼠标
    void start();       //启动服务
    void initOther();   //初始化杂七杂八
    void initDb();      //初始化数据库    
    void initWorkMode();//初始化不同工作模式的全局设置
    void autoLogin();   //自动登录日志

signals:
    //键盘事件
    void keyPressed(const QString &keys);
};

#endif // APPINIT_H
