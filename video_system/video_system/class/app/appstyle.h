#ifndef APPSTYLE_H
#define APPSTYLE_H

#include "head.h"

class AppStyle
{
public:
    //当前样式名称
    static QString styleName;
    //其他控件样式 一般这个函数里面的内容不同的项目不一样
    static void addCustomStyle(QStringList &list);
    //初始化全局样式
    static void initStyle(const QString &styleName = AppConfig::StyleName);
};

#endif // APPSTYLE_H
