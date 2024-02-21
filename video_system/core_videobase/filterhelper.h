#ifndef FILTERHELPER_H
#define FILTERHELPER_H

#include "widgethead.h"
#include "qstringlist.h"

class FilterHelper
{
public:
    //其他滤镜
    static QString getFilter();
    //旋转滤镜
    static QString getFilter(int rotate);
    //根据标签信息获取对应滤镜字符串
    static QString getFilter(const OsdInfo &osd, bool noimage);
    //根据图形信息获取对应滤镜字符串
    static QString getFilter(const GraphInfo &graph, bool hardware);

    //转换滤镜字符串到ffmpeg格式
    static QString getFilters(const QStringList &listFilter);
    //传入标签队列和图形队列获取滤镜字符串
    static QStringList getFilters(const QList<OsdInfo> &listOsd, const QList<GraphInfo> &listGraph, bool noimage = false, bool hardware = false);
};

#endif // FILTERHELPER_H
