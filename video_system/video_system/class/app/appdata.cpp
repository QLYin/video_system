#include "appdata.h"
#include "head.h"

//将视频面板窗体指针放全局(需要的地方直接用就好)
frmVideoPanel *AppData::videoPanel = 0;

QString AppData::Version = "V20231216";
int AppData::MapWidth = 800;
int AppData::MapHeight = 600;
bool AppData::IsMove = false;
bool AppData::IsReboot = false;
QDateTime AppData::LastLiveTime = QDateTime::currentDateTime();
QString AppData::CurrentUrl = "";
QString AppData::CurrentImage = "bg_alarm.jpg";

int AppData::RowHeight = 25;
int AppData::LeftWidth = 160;
int AppData::RightWidth = 200;
int AppData::TopHeight = 80;
int AppData::BottomHeight = 0;
int AppData::ToolHeight = 40;
int AppData::DeviceWidth = 75;
int AppData::DeviceHeight = 35;

int AppData::BtnMinWidth = 70;
int AppData::BtnMinHeight = 55;
int AppData::BtnIconSize = 18;
int AppData::BorderWidth = 4;
int AppData::SwitchBtnWidth = 63;
int AppData::SwitchBtnHeight = 25;

int AppData::IconMain1 = 0xe6a9;
QString AppData::TitleMain1 = QString::fromUtf8("备用模块");

QString AppData::DeviceIconFile = "./device/device_red.png";
QString AppData::DeviceIconFile2 = "./device/device_blue.png";
int AppData::DeviceIconSize = 25;
int AppData::DeviceIconSize2 = 25;

int AppData::GpsDeviceCount = 0;
QStringList AppData::GpsDeviceNames = QStringList();
QStringList AppData::GpsDevicePoints = QStringList();
QStringList AppData::GpsDeviceColors = QStringList();

QStringList AppData::NvrTypes = QStringList();
QStringList AppData::IpcTypes = QStringList();
QStringList AppData::VideoUrls = QStringList();

QString AppData::VideoNormalPath = QString("video_normal");
QString AppData::VideoAlarmPath = QString("video_alarm");
QString AppData::ImageNormalPath = QString("image_normal");
QString AppData::ImageAlarmPath = QString("image_alarm");

QString AppData::MapPath = QString();
QString AppData::SoundPath = QString();
QList<QString> AppData::MapNames = QList<QString>();
QList<QString> AppData::SoundNames = QList<QString>();
QList<QString> AppData::PortNames = QList<QString>();
