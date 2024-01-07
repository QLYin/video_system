#ifndef FFMPEGFILTER_H
#define FFMPEGFILTER_H

#include "ffmpeginclude.h"
#include "ffmpegstruct.h"
#include "abstractvideothread.h"

class FFmpegFilter
{
public:
    //获取滤镜字符串
    static QString getFilter(const FilterData &filterData);
    //初始化滤镜
    static int initFilter(AbstractVideoThread *thread, AVStream *stream, AVCodecContext *avctx, FilterData &filterData);
    //释放滤镜数据
    static void freeFilter(FilterData &filterData);
};

#endif // FFMPEGFILTER_H
