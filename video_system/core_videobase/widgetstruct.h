#ifndef WIDGETSTRUCT_H
#define WIDGETSTRUCT_H

#include <QString>
#include <QColor>
#include <QImage>
#include <QList>
#include <QRect>
#include <QPainterPath>
#include <QDebug>

//录制状态
enum RecorderState {
    RecorderState_Recording = 0,//录制中
    RecorderState_Paused = 1,   //暂停中
    RecorderState_Stopped = 2   //已结束
};

//保存模式
enum SaveMode {
    SaveMode_File = 0,          //保存到文件
    SaveMode_Rtmp = 1,          //推流到rtmp
    SaveMode_Rtsp = 2           //推流到rtsp
};

//保存视频文件类型
enum SaveVideoType {
    SaveVideoType_None = 0,     //不保存
    SaveVideoType_Yuv = 1,      //原始数据(yuv格式)
    SaveVideoType_Stream = 2,   //裸流数据(h264/h265)
    SaveVideoType_Mp4 = 3       //mp4文件
};

//保存音频文件类型
enum SaveAudioType {
    SaveAudioType_None = 0,     //不保存
    SaveAudioType_Pcm = 1,      //原始数据
    SaveAudioType_Wav = 2,      //wav文件
    SaveAudioType_Aac = 3,      //aac文件
};

//标签格式
enum OsdFormat {
    OsdFormat_Text = 0,         //文本
    OsdFormat_Date = 1,         //日期
    OsdFormat_Time = 2,         //时间
    OsdFormat_DateTime = 3,     //日期时间
    OsdFormat_DateTimeMs = 4,   //日期时间带毫秒
    OsdFormat_Image = 5         //图片
};

//标签位置
enum OsdPosition {
    OsdPosition_LeftTop = 0,    //左上角
    OsdPosition_LeftBottom = 1, //左下角
    OsdPosition_RightTop = 2,   //右上角
    OsdPosition_RightBottom = 3,//右下角
    OsdPosition_Center = 4,     //居中
    OsdPosition_Custom = 255    //自定义
};

//标签信息
struct OsdInfo {
    QString name;               //名字唯一标识符(方便删除或更新)
    bool visible;               //是否可见
    int fontSize;               //字号大小
    QString fontName;           //字体文件(用于区分图形字体)
    QString text;               //文本文字

    QColor color;               //文本颜色(支持透明度)
    QColor bgColor;             //背景颜色(支持透明度)
    QImage image;               //标签图片(需要指定标签格式为图片)
    QPoint point;               //标签坐标(位置自定义时候使用)
    OsdFormat format;           //标签格式
    OsdPosition position;       //标签位置

    OsdInfo() {
        name = "osd";
        visible = true;
        fontSize = 12;
        fontName = "wenquanyi";
        text = "视频标签";

        color = "#FF0000";
        bgColor = Qt::transparent;
        image = QImage();
        point = QPoint(0, 0);
        format = OsdFormat_Text;
        position = OsdPosition_LeftTop;
    }
};

//图形信息(人脸框和多边形区域等)
struct GraphInfo {
    QString name;               //名字唯一标识符(方便删除或更新)
    int borderWidth;            //边框大小
    QColor borderColor;         //边框颜色
    QColor bgColor;             //背景颜色

    QRect rect;                 //矩形区域
    QPainterPath path;          //路径集合
    QList<QPoint> points;       //点坐标集合

    GraphInfo() {
        name = "graph";
        borderWidth = 2;
        borderColor = "#FF0000";
        bgColor = Qt::transparent;
    }
};

//悬浮条位置
enum BannerPosition {
    BannerPosition_Top = 0,     //顶部
    BannerPosition_Bottom = 1,  //底部
    BannerPosition_Left = 2,    //左侧
    BannerPosition_Right = 3    //右侧
};

//悬浮条参数
#include <QMargins>
struct BannerPara {
    QMargins margin;            //边距
    int spacing;                //间距
    int bgAlpha;                //背景透明度

    QColor bgColor;             //背景颜色
    QColor textColor;           //文本颜色
    QColor pressColor;          //按下颜色
    BannerPosition position;    //悬浮条位置

    QList<int> icons;           //按钮图标代码集合
    QList<QString> names;       //按钮名称标识集合
    QList<QString> tips;        //按钮提示信息集合

    BannerPara() {
        margin = QMargins(5, 0, 3, 0);
        spacing = 2;
        bgAlpha = 200;

        bgColor = "#333333";
        textColor = "#FFFFFF";
        pressColor = "#5EC7D9";
        position = BannerPosition_Top;

        //采用iconfont图形字体
        icons = QList<int>() << 0xea1b << 0xeb15 << 0xe674 << 0xe703 << 0xe502 << 0xe74c;
        //为了避免和其他控件重名建议前面加上前缀用来区分
        names = QList<QString>() << "banner_btnRecord" << "banner_btnSnap" << "banner_btnSound" << "banner_btnCrop" << "banner_btnAlarm" << "banner_btnClose";
        tips = QList<QString>() << "录制" << "抓图" << "声音" << "放大" << "警情" << "关闭";
    }
};

//缩放显示模式
enum ScaleMode {
    ScaleMode_Auto = 0,         //自动调整(超过则等比缩放否则原图)
    ScaleMode_Aspect = 1,       //等比缩放(任何尺寸都等比缩放)
    ScaleMode_Fill = 2          //拉伸填充(任何尺寸都拉伸填充)
};

//视频显示模式
enum VideoMode {
    VideoMode_Hwnd = 0,         //句柄模式(传入句柄交给绘制)
    VideoMode_Painter = 1,      //绘制模式(采用painter绘制图片)
    VideoMode_Opengl = 2        //GPU模式(采用opengl绘制yuv等数据)
};

//标签和图形绘制模式
enum DrawMode {
    DrawMode_Cover = 0,         //绘制到控件
    DrawMode_Image = 1,         //绘制到原图
    DrawMode_Source = 2         //从源头绘制
};

//窗体参数
struct WidgetPara {
    int borderWidth;            //边框大小
    QColor borderColor;         //边框颜色
    QColor focusColor;          //焦点颜色

    QColor bgColor;             //背景颜色
    QColor textColor;           //文字颜色
    QColor fillColor;           //填充颜色

    int bgTextSize;             //文字大小
    QString bgText;             //背景文字
    QImage bgImage;             //背景图片

    QString videoFlag;          //视频通道标识
    ScaleMode scaleMode;        //缩放显示模式
    VideoMode videoMode;        //视频显示模式
    DrawMode osdDrawMode;       //标签绘制模式
    DrawMode graphDrawMode;     //图形绘制模式

    int soundValue;             //默认音量大小
    bool soundMuted;            //默认静音状态
    bool sharedData;            //共享解码线程
    bool copyImage;             //是否拷贝图片
    bool restartClear;          //重连清空画面

    bool bannerEnable;          //悬浮条是否启用
    int bannerSize;             //悬浮条尺寸大小(横向则表示高度/纵向表示宽度)
    BannerPosition bannerPosition;//悬浮条位置

    WidgetPara() {
        borderWidth = 1;
        borderColor = Qt::transparent;
        focusColor = "#22A3A9";

        bgColor = Qt::transparent;
        textColor = Qt::transparent;
        fillColor = Qt::transparent;
        //fillColor = Qt::black;

        bgTextSize = 25;
        bgText = "视频";
        bgImage = QImage();
        //bgImage = QImage(":/image/bg_novideo.png");

        videoFlag = "";
        scaleMode = ScaleMode_Auto;
        videoMode = VideoMode_Hwnd;
        osdDrawMode = DrawMode_Cover;
        graphDrawMode = DrawMode_Cover;

        soundValue = 100;
        soundMuted = false;
        sharedData = false;
        copyImage = false;
        restartClear = true;

        bannerEnable = false;
        bannerSize = 25;
        bannerPosition = BannerPosition_Top;
    }
};

//错误类型
enum VideoError {
    VideoError_Open = 1,        //打开失败
    VideoError_Save = 2,        //保存失败
    VideoError_Other = 255      //其他错误
};

#endif // WIDGETSTRUCT_H
