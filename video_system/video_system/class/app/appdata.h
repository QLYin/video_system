#ifndef APPDATA_H
#define APPDATA_H

#include <QObject>
#include <QPixmap>
#include <QDateTime>
class frmVideoPanel;

class AppData
{
public:
    static frmVideoPanel *videoPanel;   //视频面板窗体

    //全局通用变量
    static QString Version;             //版本号
    static int MapWidth;                //地图宽度
    static int MapHeight;               //地图高度
    static bool IsMove;                 //设备是否可以移动
    static bool IsReboot;               //是否正在重启
    static QDateTime LastLiveTime;      //程序最后的活动时间-包括键盘+鼠标活动
    static QString CurrentUrl;          //当前选中的视频,用于云台控制等处理
    static QString CurrentImage;        //当前对应地图    

    //全局布局宽度高度值
    static int RowHeight;               //行高
    static int LeftWidth;               //左边宽度
    static int RightWidth;              //右边宽度
    static int TopHeight;               //顶部高度
    static int BottomHeight;            //底部高度
    static int ToolHeight;              //底部按钮栏高度
    static int DeviceWidth;             //设备对象宽度
    static int DeviceHeight;            //设备对象高度

    static int BtnMinWidth;             //按钮最小宽度
    static int BtnMinHeight;            //左侧导航按钮最小高度
    static int BtnIconSize;             //按钮图标宽度高度
    static int BorderWidth;             //高亮边框宽度
    static int SwitchBtnWidth;          //开关按钮宽度
    static int SwitchBtnHeight;         //开关按钮高度

    static int IconMain1;               //备用模块1图标
    static QString TitleMain1;          //备用模块1标题

    static QString DeviceIconFile;      //设备图标
    static QString DeviceIconFile2;     //设备图标2
    static int DeviceIconSize;          //设备图标大小
    static int DeviceIconSize2;         //设备图标2大小

    static int GpsDeviceCount;          //GPS设备数量
    static QStringList GpsDeviceNames;  //GPS设备名称集合
    static QStringList GpsDevicePoints; //GPS设备坐标集合
    static QStringList GpsDeviceColors; //GPS设备颜色集合

    static QStringList NvrTypes;        //录像机类型
    static QStringList IpcTypes;        //摄像机类型
    static QStringList VideoUrls;       //视频地址集合

    static QString VideoNormalPath;     //正常视频存储目录
    static QString VideoAlarmPath;      //报警视频存储目录
    static QString ImageNormalPath;     //正常图片存储目录
    static QString ImageAlarmPath;      //报警图片存储目录

    static QString MapPath;             //地图目录
    static QList<QString> MapNames;     //地图文件名称集合
    static QString SoundPath;           //声音目录
    static QList<QString> SoundNames;   //声音文件名称集合
    static QList<QString> PortNames;    //串口号集合    
};

#endif // APPDATA_H
