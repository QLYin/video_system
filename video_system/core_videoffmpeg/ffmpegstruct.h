#ifndef FFMPEGSTRUCT_H
#define FFMPEGSTRUCT_H

#include "ffmpeginclude.h"
#include "widgetstruct.h"

//滤镜数据结构体
struct FilterData {
    //是否启用
    bool enable;
    //重新初始化
    bool init;
    //是否初始化成功
    bool isOk;
    //视频旋转角度
    int rotate;

    //标签信息集合(最终通过滤镜贴到视频数据上)
    QList<OsdInfo> listOsd;
    //图形信息集合(最终通过滤镜贴到视频数据上)
    QList<GraphInfo> listGraph;

    //源头帧格式
    AVPixelFormat formatSrc;
    //输入帧格式
    AVPixelFormat formatIn;
    //输出帧格式
    AVPixelFormat formatOut;

    //滤镜输入上下文
    AVFilterContext *filterSrcCtx;
    //滤镜输出上下文
    AVFilterContext *filterSinkCtx;
    //滤镜容器
    AVFilterGraph *filterGraph;

    FilterData() {
        enable = true;
        init = true;
        isOk = false;
        rotate = 0;

        formatSrc = AV_PIX_FMT_YUV420P;
        formatIn = AV_PIX_FMT_YUV420P;
        formatOut = AV_PIX_FMT_YUV420P;

        filterSrcCtx = NULL;
        filterSinkCtx = NULL;
        filterGraph = NULL;
    }
};

#endif // FFMPEGSTRUCT_H
