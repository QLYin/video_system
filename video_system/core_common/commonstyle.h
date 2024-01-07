#ifndef COMMONSTYLE_H
#define COMMONSTYLE_H

#include "head.h"

class CommonStyle
{
public:
    //Qt自带类窗体样式
    static void addQtControlStyle(QStringList &list);
    //自定义控件样式
    static void addCustomControlStyle(QStringList &list, int borderWidth);

    //分页导航样式
    static void addNavPageStyle(QStringList &list, int pageButtonCount);
    //导航按钮样式
    static void addNavBtnStyle(QStringList &list, int topBtnRadius, int leftBtnRadius = 5);

    //开关按钮样式
    static void addSwitchButtonStyle(QStringList &list, const QString &styleName, int btnWidth, int btnHeight);
    //加深样式
    static void addDarkStyle(QStringList &list, const QString &styleName);
    //云台控件样式
    static void addGaugeCloudStyle(QStringList &list, const QString &styleName);
    //窗体样式
    static void addFormStyle(QStringList &list, const QString &styleName, int btnMinWidth);
};

#endif // COMMONSTYLE_H
