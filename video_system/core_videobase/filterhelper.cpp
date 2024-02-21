#include "filterhelper.h"

//官方文档: https://ffmpeg.org/ffmpeg-filters.html
//视频滤镜: https://blog.csdn.net/yang1fei2/article/details/127939788
//音频滤镜: https://blog.csdn.net/yang1fei2/article/details/127777958
//播放示例: ffplay -i f:/mp4/push/1.mp4 -vf eq=brightness=0.5:contrast=100.0:saturation=3.0
//转换示例: ffmpeg -y -i f:/mp4/push/1.mp4 -vf drawtext=text='%{pts\:hms}':fontsize=30:fontcolor=#ff0000:x=10:y=10 f:/mp4/out.mp4

QString FilterHelper::getFilter()
{
    //图片模糊: boxblur=2:2
    //图片黑白: lutyuv=u=128:v=128
    //格式转换: format=pix_fmts=yuvj422p
    //局部裁剪: crop=2/3*in_w:2/3*in_h 另一种写法 crop=100:100:50:60
    //缩放大小: scale=200:100 看起来是马赛克效果
    //网格线条: drawgrid=w=100:h=100:t=2:c=red@0.5
    //高斯模糊: delogo=x=0:y=0:w=100:h=77:band=10
    //边缘检测: edgedetect=low=0.1:high=0.4
    //出场入场: fade=in:0:300/fade=out:0:300
    //跳帧播放: framestep=5
    //同屏显示: tile=3x2:nb_frames=6:padding=7:margin=2
    //图片参数: eq=contrast=1.0:brightness=0.8:saturation=1.0:gamma=1.0
    //尖锐程度: unsharp=13:13
    //图片参数: 对比度contrast=1.0[-1000.0,1000.0]/明亮度brightness=0.0[-1.0,1.0]/饱和度saturation=1.0[0.0,3.0]/伽马gamma=1.0[0.1,10.0]
    //组合滤镜: [in]boxblur=2:2,drawtext=x=0:y=100:text='HelloWorld':fontcolor=green:fontsize=50[text];movie=./osd.png[wm];[text][wm]overlay=0:0[out]
    //多图滤镜: movie=./osd.png[wm1];movie=./osd.png[wm2];[in][wm1]overlay=0:0[a];[a][wm2]overlay=0:300[out];

    QString filter;
    //filter = "crop=100:160:50:60";
    return filter;
}

QString FilterHelper::getFilter(int rotate)
{
    //0=逆时针旋转90度并垂直翻转
    //1=顺时针旋转90度
    //2=逆时针旋转90度
    //3=顺时针旋转90度并水平翻转
    //hflip=水平翻转
    //vflip=垂直翻转
    //180度=先水平翻转再垂直翻转(查阅ffplay代码获知)

    QString filter;
    if (rotate == 90) {
        filter = "transpose=1";
    } else if (rotate == 180) {
        filter = "hflip,vflip";
    } else if (rotate == 270) {
        filter = "transpose=2";
    } else if (rotate > 0) {
        filter = QString("rotate=%1*PI/180").arg(rotate);
    }

    return filter;
}

QString FilterHelper::getFilter(const OsdInfo &osd, bool noimage)
{
    QString filter;
    if (!osd.visible) {
        return filter;
    }

    //图片滤镜对应宽高计算标识不一样
    QString h = "h-th";
    QString w = "w-tw";
    bool isImage = (osd.format == OsdFormat_Image);
    if (isImage) {
        h = "H-h";
        w = "W-w";
        if (noimage) {
            return filter;
        }
    }

    //计算对应的坐标位置
    QString offset = "10";
    QString x = QString::number(osd.point.x());
    QString y = QString::number(osd.point.y());
    if (osd.position == OsdPosition_LeftTop) {
        x = offset;
        y = offset;
    } else if (osd.position == OsdPosition_LeftBottom) {
        x = offset;
        y = h + "-" + offset;
    } else if (osd.position == OsdPosition_RightTop) {
        x = w + "-" + offset;
        y = offset;
    } else if (osd.position == OsdPosition_RightBottom) {
        x = w + "-" + offset;
        y = h + "-" + offset;
    } else if (osd.position == OsdPosition_Center) {
        x = "(" + w + ")/2";
        y = "(" + h + ")/2";
    }

    QStringList list;
    if (isImage) {
        //movie=./osd.png[wm];[in][wm]overlay=50:50[out] overlay=x=W-w:y=H-h
        filter = QString("movie=./%1[wm];[in][wm]overlay=x=%2:y=%3[out]").arg(osd.name).arg(x).arg(y);
    } else {
        //安卓上的库如果自己编译的带了freetype则可以打开
#ifdef Q_OS_ANDROID
        return filter;
#endif
        //drawtext=text='hello':fontsize=30:fontcolor=#ffffff:x=10:y=10
        //linux上需要对应的ffmpeg库编译的时候指定freetype否则不能用drawtext
        //时间格式: https://baike.baidu.com/item/strftime/9569073 pts时间 %{pts\\:hms}
        QString text = osd.text;
        if (osd.format == OsdFormat_Date) {
            text = "%{localtime:%Y-%m-%d}";
        } else if (osd.format == OsdFormat_Time) {
            text = "%{localtime:%H\\\\:%M\\\\:%S}";
        } else if (osd.format == OsdFormat_DateTime) {
            text = "%{localtime}";
        } else if (osd.format == OsdFormat_DateTimeMs) {
            //目前没有查到毫秒如何显示
            text = "%{localtime:%Y-%m-%d %H\\\\:%M\\\\:%S}";
        }

        //如果有:需要转义
        list << QString("text='%1'").arg(text.replace(":", "\\:"));
        list << QString("fontsize=%1").arg(osd.fontSize);
        QColor color = osd.color;
        list << QString("fontcolor=%1@%2").arg(color.name()).arg(color.alphaF());

        //每个文本都可以对应自己的字体文件
        list << QString("fontfile=./%1.ttf").arg(osd.fontName);
        list << QString("x=%1").arg(x);
        list << QString("y=%1").arg(y);
        //list << QString("alpha=%1").arg(0.5);

        //背景颜色
        QColor bgColor = osd.bgColor;
        if (bgColor != Qt::transparent) {
            list << QString("box=1:boxcolor=%1@%2").arg(bgColor.name()).arg(bgColor.alphaF());
        }

        filter = QString("drawtext=%1").arg(list.join(":"));
    }

    return filter;
}

QString FilterHelper::getFilter(const GraphInfo &graph, bool hardware)
{
    //drawbox=x=10:y=10:w=100:h=100:c=#ffffff@1:t=2
    QString filter;
    //有个现象就是硬解码下的图形滤镜会导致原图颜色不对
    if (hardware) {
        return filter;
    }

    //暂时只实现了矩形区域
    QRect rect = graph.rect;
    if (rect.isEmpty()) {
        return filter;
    }

    //过滤关键字用于电子放大
    if (graph.name == "crop") {
        filter = QString("crop=%1:%2:%3:%4").arg(rect.width()).arg(rect.height()).arg(rect.x()).arg(rect.y());
        return filter;
    }

    QStringList list;
    list << QString("x=%1").arg(rect.x());
    list << QString("y=%1").arg(rect.y());
    list << QString("w=%1").arg(rect.width());
    list << QString("h=%1").arg(rect.height());

    QColor color = graph.borderColor;
    list << QString("c=%1@%2").arg(color.name()).arg(color.alphaF());

    //背景颜色不透明则填充背景颜色
    if (graph.bgColor == Qt::transparent) {
        list << QString("t=%1").arg(graph.borderWidth);
    } else {
        list << QString("t=%1").arg("fill");
    }

    filter = QString("drawbox=%1").arg(list.join(":"));
    return filter;
}

QString FilterHelper::getFilters(const QStringList &listFilter)
{
    //挨个取出图片滤镜对应的图片和坐标
    int count = listFilter.count();
    QStringList listImage, listPosition, listTemp;
    for (int i = 0; i < count; ++i) {
        QString filter = listFilter.at(i);
        if (filter.startsWith("movie=")) {
            QStringList list = filter.split(";");
            QString movie = list.first();
            QString overlay = list.last();
            movie.replace("[wm]", "");
            overlay.replace("[wm]", "");
            overlay.replace("[in]", "");
            overlay.replace("[out]", "");
            listImage << movie;
            listPosition << overlay;
        } else {
            listTemp << filter;
        }
    }

    //图片滤镜字符串在下面重新处理
    QString filterImage, filterAll;
    QString filterOther = listTemp.join(",");

    //存在图片水印需要重新调整滤镜字符串
    //1张图: movie=./osd.png[wm0];[in][wm0]overlay=0:0[out]
    //2张图: movie=./osd.png[wm0];movie=./osd.png[wm1];[in][wm0]overlay=0:0[a];[a][wm1]overlay=0:0[out]
    //3张图: movie=./osd.png[wm0];movie=./osd.png[wm1];movie=./osd.png[wm2];[in][wm0]overlay=0:0[a0];[a0][wm1]overlay=0:0[a1];[a1][wm2]overlay=0:0[out]
    count = listImage.count();
    if (count > 0) {
        //加上标识符和头部和尾部标识符
        for (int i = 0; i < count; ++i) {
            QString flag = QString("[wm%1]").arg(i);
            listImage[i] = listImage.at(i) + flag;
            listPosition[i] = flag + listPosition.at(i);
            listPosition[i] = (i == 0 ? "[in]" : QString("[a%1]").arg(i - 1)) + listPosition.at(i);
            listPosition[i] = listPosition.at(i) + (i == (count - 1) ? "[out]" : QString("[a%1]").arg(i));
        }

        QStringList filters;
        for (int i = 0; i < count; ++i) {
            filters << listImage.at(i);
        }
        for (int i = 0; i < count; ++i) {
            filters << listPosition.at(i);
        }

        //图片滤镜集合最终字符串
        filterImage = filters.join(";");

        //存在其他滤镜则其他滤镜在前面
        if (listTemp.count() > 0) {
            filterImage.replace("[in]", "[other]");
            filterAll = "[in]" + filterOther + "[other];" + filterImage;
        } else {
            filterAll = filterImage;
        }
    } else {
        filterAll = filterOther;
    }

    return filterAll;
}

QStringList FilterHelper::getFilters(const QList<OsdInfo> &listOsd, const QList<GraphInfo> &listGraph, bool noimage, bool hardware)
{
    //滤镜内容字符串集合
    QStringList listFilter;

    //加入标签信息
    foreach (OsdInfo osd, listOsd) {
        QString filter = FilterHelper::getFilter(osd, noimage);
        if (!filter.isEmpty()) {
            listFilter << filter;
        }
    }

    //加入图形信息
    foreach (GraphInfo graph, listGraph) {
        QString filter = FilterHelper::getFilter(graph, hardware);
        if (!filter.isEmpty()) {
            listFilter << filter;
        }
    }

    //加入其他滤镜
    QString filter = FilterHelper::getFilter();
    if (!filter.isEmpty()) {
        listFilter << filter;
    }

    return listFilter;
}
