#include "appconfig.h"
#include "qthelper.h"

QString AppConfig::ConfigFile = "config.ini";

int AppConfig::OpenGLType = 0;
int AppConfig::IndexStart = 0;
int AppConfig::WorkMode = 0;
int AppConfig::NavStyle = 3;
int AppConfig::SynTimeInterval = 10;
QString AppConfig::StyleName = ":/qss/blackvideo.css";
QString AppConfig::LogoImage = ":/logo.svg";
QString AppConfig::CompanyHttp = "https://qtchina.blog.csdn.net/";
QString AppConfig::CompanyTel = "021-12345678";
QString AppConfig::Copyright = QString::fromUtf8("物联网技术研究中心");
QString AppConfig::TitleCn = QString::fromUtf8("安防视频监控管理平台");
QString AppConfig::TitleEn = "Security video management platform";

int AppConfig::RecordsPerpage = 28;
int AppConfig::PageButtonCount = 5;
int AppConfig::MsgListCount = 20;
int AppConfig::MsgTableCount = 0;
int AppConfig::TimeHideCursor = 0;
int AppConfig::TimeAutoFull = 0;
int AppConfig::AlarmSoundCount = 0;
int AppConfig::WindowOpacity = 70;
int AppConfig::TipInterval = 3;

bool AppConfig::AutoRun = false;
bool AppConfig::AutoLogin = false;
bool AppConfig::AutoPwd = false;
bool AppConfig::SynTime = false;
bool AppConfig::SaveLog = false;
bool AppConfig::SaveRunTime = false;
bool AppConfig::RightInfo = true;
bool AppConfig::SaveUrl = true;

bool AppConfig::FormMax = false;
bool AppConfig::FormFull = false;
QRect AppConfig::FormGeometry = QRect(0, 0, 1200, 850);
QRect AppConfig::PollGeometry = QRect(0, 0, 450, 60);
QString AppConfig::SelectDirName = ".";
QString AppConfig::ExpandItem = "-1";

QString AppConfig::LastLoginer = "admin";
QString AppConfig::LastFormMain = "btnView";
QString AppConfig::LastFormVideo = "btnVideoPlayLocal";
QString AppConfig::LastFormMap = "btnMapWeb";
QString AppConfig::LastFormData = "btnDataUser";
QString AppConfig::LastFormConfig = "btnConfigSystem";
QString AppConfig::DeviceIP = "113.118.203.234";

bool AppConfig::FullScreen = false;
bool AppConfig::VisibleIpcSearch = true;
bool AppConfig::VisiblePollPlus = false;
bool AppConfig::VisiblePtzStep = false;

int AppConfig::ScaleMode = 2;
int AppConfig::VideoMode = 0;
int AppConfig::BorderWidth = 2;
int AppConfig::BannerPosition = 0;
int AppConfig::SoundValue = 100;
bool AppConfig::SoundMuted = true;
bool AppConfig::SharedData = true;

QString AppConfig::Hardware = "none";
QString AppConfig::Transport = "tcp";
int AppConfig::DecodeType = 0;
int AppConfig::Caching = 8192000;
bool AppConfig::AudioLevel = false;
bool AppConfig::PlayAudio = true;
bool AppConfig::CountKbps = false;
int AppConfig::EncodeType = 0;
float AppConfig::EncodeVideoRatio = 1;
int AppConfig::ReadTimeout = 5000;
int AppConfig::ConnectTimeout = 0;

int AppConfig::VideoCount = 64;
QString AppConfig::VideoType = "1_4";
bool AppConfig::VideoMax = false;
bool AppConfig::VideoDrag = true;
bool AppConfig::SaveVideo = false;
bool AppConfig::VideoSmart = false;
bool AppConfig::OnvifEvent = false;
bool AppConfig::OnvifNtp = true;

bool AppConfig::CheckOffline = false;
bool AppConfig::TreeBig = false;
bool AppConfig::TreeRtsp = true;
int AppConfig::RtspType = 1;
int AppConfig::AlarmSaveTime = 30;
int AppConfig::OpenIpcPolicy = 1;
int AppConfig::OpenNvrPolicy = 1;
int AppConfig::OpenMaxCount = 2;
int AppConfig::TableDataPolicy = 0;
int AppConfig::ChannelBgText = 1;
QString AppConfig::DefaultChName = QString::fromUtf8("通道");
QString AppConfig::DefaultIpcName = QString::fromUtf8("摄像机#1");
QString AppConfig::DefaultNvrName = QString::fromUtf8("录像机#1");

bool AppConfig::EnableChannelx = true;
bool AppConfig::EnableChannel1 = true;
bool AppConfig::EnableChannel4 = true;
bool AppConfig::EnableChannel6 = true;
bool AppConfig::EnableChannel8 = true;
bool AppConfig::EnableChannel9 = true;
bool AppConfig::EnableChannel13 = true;
bool AppConfig::EnableChannel16 = true;
bool AppConfig::EnableChannel25 = true;
bool AppConfig::EnableChannel36 = true;
bool AppConfig::EnableChannel64 = true;

bool AppConfig::EnableMain1 = false;
bool AppConfig::EnableWeather = false;
bool AppConfig::EnableVideoPanelTool = true;
bool AppConfig::EnableMapImage = false;
bool AppConfig::EnableVideoPlayWeb = true;
bool AppConfig::EnableVideoPlayNvr = true;
bool AppConfig::EnableVideoPlayImage = true;
bool AppConfig::EnableVideoUpload = false;
bool AppConfig::EnableConfigPoll = true;
bool AppConfig::EnableConfigUser = true;
bool AppConfig::EnableConfigRecord = true;
bool AppConfig::EnableConfigOther = false;
bool AppConfig::EnableTitleMenu = true;
bool AppConfig::EnableModelMenu = true;

bool AppConfig::AutoPoll = false;
bool AppConfig::Polling = false;
int AppConfig::PollInterval = 30;
int AppConfig::PollType = 4;
int AppConfig::PollRtsp = 1;
QString AppConfig::PollGroupNames = QString::fromUtf8("默认分组");
QString AppConfig::PollGroupLast = QString::fromUtf8("默认分组");

int AppConfig::PlusType = 1;
int AppConfig::PlusNumber = 16;
bool AppConfig::PlusNvr = false;
QString AppConfig::PlusAddr = "192.168.1.10";
QString AppConfig::PlusMain = "rtsp://admin:12345@[Addr]:554/Streaming/Channels/101?transportmode=unicast";
QString AppConfig::PlusSub = "rtsp://admin:12345@[Addr]:554/Streaming/Channels/102?transportmode=unicast";

int AppConfig::NvrCompanyType = 1;
int AppConfig::NvrVideoType = 1;
QString AppConfig::NvrUserName = "admin";
QString AppConfig::NvrUserPwd = "admin";
QString AppConfig::NvrDeviceIP = "192.168.0.64";
bool AppConfig::NvrSyncPosition = false;
bool AppConfig::NvrAutoCh = false;
int AppConfig::NvrStreamType = 0;
QString AppConfig::NvrDateTimeFormat = "yyyy-MM-dd HH:mm:ss";
QString AppConfig::NvrDateTimeStart = QDateTime::currentDateTime().addDays(-1).toString(AppConfig::NvrDateTimeFormat);
QString AppConfig::NvrDateTimeEnd = QDateTime::currentDateTime().addDays(0).toString(AppConfig::NvrDateTimeFormat);

QString AppConfig::SearchUserName = "admin";
QString AppConfig::SearchUserPwd = "Admin123456";
QString AppConfig::SearchLocalIP = "192.168.1.2";
QString AppConfig::SearchDeviceIP = "192.168.1.128";
QString AppConfig::SearchFilter = "none";
QString AppConfig::SearchFilters = "none|:80|:8000|:2000|.1.";
int AppConfig::SearchInterval = 300;
int AppConfig::SearchTimeout = 1500;
bool AppConfig::SearchClear = false;

bool AppConfig::LocalAutoInfo = true;
int AppConfig::LocalConnMode = 0;
QString AppConfig::LocalDbType = "Sqlite";
QString AppConfig::LocalDbName = "video_system";
QString AppConfig::LocalHostName = "127.0.0.1";
int AppConfig::LocalHostPort = 3306;
QString AppConfig::LocalUserName = "root";
QString AppConfig::LocalUserPwd = "root";

QString AppConfig::MapVersionKey = "3.0&ak=M3vTdD2f7ZjMFKold3TbOnEc6eYHx869";
//百度地图 城市中心点坐标 https://www.cnblogs.com/hhhz/p/7591852.html
QString AppConfig::MapCenter = "121.41400,31.18280";
int AppConfig::MapZoom = 19;

QString AppConfig::RouteStartAddr = "121.425740,31.176567";
QString AppConfig::RouteEndAddr = "121.428545,31.203614";
int AppConfig::RouteType = 2;
int AppConfig::RoutePointCount = 30;
QString AppConfig::RouteDeviceName = QString::fromUtf8("国产大飞机C919");
int AppConfig::RouteMoveInterval = 2;
int AppConfig::RouteMoveMode = 0;

QString AppConfig::UploadHost = "127.0.0.1";
int AppConfig::UploadPort = 6000;

QString AppConfig::ColorTextNormal = "white";
QString AppConfig::ColorTextWarn = "gold";
QString AppConfig::ColorTextAlarm = "darkorange";
QString AppConfig::ColorIconAlarm = "tomato";
QString AppConfig::ColorMsgWait = "goldenrod";
QString AppConfig::ColorMsgSolved = "skyblue";

QString AppConfig::PortNameA = "COM1";
QString AppConfig::PortNameB = "COM2";
QString AppConfig::PortNameC = "COM3";
QString AppConfig::PortNameD = "COM4";
int AppConfig::BaudRateA = 9600;
int AppConfig::BaudRateB = 9600;
int AppConfig::BaudRateC = 115200;
int AppConfig::BaudRateD = 9600;

QString AppConfig::TcpServerHost = "127.0.0.1";
int AppConfig::TcpServerPort = 6000;
int AppConfig::TcpListenPort = 6000;
int AppConfig::UdpListenPort = 6000;

QString AppConfig::WeatherHttp = "http://tianqi.2345.com";
QString AppConfig::WeatherCity = QString::fromUtf8("上海");
int AppConfig::WeatherInterval = 60;
int AppConfig::WeatherStyle = 0;

int AppConfig::IndexDemo = 0;
QString AppConfig::VideoImageUrl = "image_normal";
QString AppConfig::VideoWidgetUrl = "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
QString AppConfig::VideoSaveUrl = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoPlusUrl1 = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoPlusUrl2 = "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
QString AppConfig::VideoPlayUrl1 = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoPlayUrl2 = "http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4";
QString AppConfig::VideoPlayUrl3 = "http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4";
QString AppConfig::VideoPlayUrl4 = "http://vfx.mtime.cn/Video/2022/12/17/mp4/221217153424902164.mp4";
QString AppConfig::VideoAudioUrl = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoDragUrl = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoChangeUrl1 = "http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4";
QString AppConfig::VideoChangeUrl2 = "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
QString AppConfig::VideoDrawUrl = "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";
QString AppConfig::VideoOsdUrl = "http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4";

void AppConfig::readConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    OpenGLType = set.value("OpenGLType", OpenGLType).toInt();
    //IndexStart = set.value("IndexStart", IndexStart).toInt();
    WorkMode = set.value("WorkMode", WorkMode).toInt();
    NavStyle = set.value("NavStyle", NavStyle).toInt();
    StyleName = set.value("StyleName", StyleName).toString();
    LogoImage = set.value("LogoImage", LogoImage).toString();
    CompanyHttp = set.value("CompanyHttp", CompanyHttp).toString();
    CompanyTel = set.value("CompanyTel", CompanyTel).toString();
    Copyright = set.value("Copyright", Copyright).toString();
    TitleCn = set.value("TitleCn", TitleCn).toString();
    TitleEn = set.value("TitleEn", TitleEn).toString();
    SynTimeInterval = set.value("SynTimeInterval", SynTimeInterval).toInt();
    set.endGroup();

    set.beginGroup("AppConfig2");
    RecordsPerpage = set.value("RecordsPerpage", RecordsPerpage).toInt();
    PageButtonCount = set.value("PageButtonCount", PageButtonCount).toInt();
    MsgListCount = set.value("MsgListCount", MsgListCount).toInt();
    MsgTableCount = set.value("MsgTableCount", MsgTableCount).toInt();
    TimeHideCursor = set.value("TimeHideCursor", TimeHideCursor).toInt();
    TimeAutoFull = set.value("TimeAutoFull", TimeAutoFull).toInt();
    AlarmSoundCount = set.value("AlarmSoundCount", AlarmSoundCount).toInt();
    WindowOpacity = set.value("WindowOpacity", WindowOpacity).toInt();
    TipInterval = set.value("TipInterval", TipInterval).toInt();
    set.endGroup();

    set.beginGroup("AppConfig3");
    AutoRun = set.value("AutoRun", AutoRun).toBool();
    AutoLogin = set.value("AutoLogin", AutoLogin).toBool();
    AutoPwd = set.value("AutoPwd", AutoPwd).toBool();
    SynTime = set.value("SynTime", SynTime).toBool();
    SaveLog = set.value("SaveLog", SaveLog).toBool();
    SaveRunTime = set.value("SaveRunTime", SaveRunTime).toBool();
    RightInfo = set.value("RightInfo", RightInfo).toBool();
    SaveUrl = set.value("SaveUrl", SaveUrl).toBool();
    set.endGroup();

    set.beginGroup("AppConfig4");
    FormMax = set.value("FormMax", FormMax).toBool();
    FormFull = set.value("FormFull", FormFull).toBool();
    FormGeometry = set.value("FormGeometry", FormGeometry).toRect();
    PollGeometry = set.value("PollGeometry", PollGeometry).toRect();
    SelectDirName = set.value("SelectDirName", SelectDirName).toString();
    ExpandItem = set.value("ExpandItem", ExpandItem).toString();    
    set.endGroup();

    set.beginGroup("AppConfig5");
    LastLoginer = set.value("LastLoginer", LastLoginer).toString();
    LastFormMain = set.value("LastFormMain", LastFormMain).toString();
    LastFormVideo = set.value("LastFormVideo", LastFormVideo).toString();
    LastFormMap = set.value("LastFormMap", LastFormMap).toString();
    LastFormData = set.value("LastFormData", LastFormData).toString();
    LastFormConfig = set.value("LastFormConfig", LastFormConfig).toString();
    DeviceIP = set.value("DeviceIP", DeviceIP).toString();
    set.endGroup();

    set.beginGroup("VisibleConfig");
    FullScreen = set.value("FullScreen", FullScreen).toBool();
    VisibleIpcSearch = set.value("VisibleIpcSearch", VisibleIpcSearch).toBool();
    VisiblePollPlus = set.value("VisiblePollPlus", VisiblePollPlus).toBool();
    VisiblePtzStep = set.value("VisiblePtzStep", VisiblePtzStep).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig1");
    ScaleMode = set.value("ScaleMode", ScaleMode).toInt();
    VideoMode = set.value("VideoMode", VideoMode).toInt();
    BorderWidth = set.value("BorderWidth", BorderWidth).toInt();
    BannerPosition = set.value("BannerPosition", BannerPosition).toInt();
    SoundValue = set.value("SoundValue", SoundValue).toInt();
    SoundMuted = set.value("SoundMuted", SoundMuted).toBool();
    SharedData = set.value("SharedData", SharedData).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig2");    
    Hardware = set.value("Hardware", Hardware).toString();
    Transport = set.value("Transport", Transport).toString();
    DecodeType = set.value("DecodeType", DecodeType).toInt();
    Caching = set.value("Caching", Caching).toInt();
    AudioLevel = set.value("AudioLevel", AudioLevel).toBool();
    PlayAudio = set.value("PlayAudio", PlayAudio).toBool();
    CountKbps = set.value("CountKbps", CountKbps).toBool();
    EncodeType = set.value("EncodeType", EncodeType).toInt();
    EncodeVideoRatio = set.value("EncodeVideoRatio", EncodeVideoRatio).toFloat();
    ReadTimeout = set.value("ReadTimeout", ReadTimeout).toInt();
    ConnectTimeout = set.value("ConnectTimeout", ConnectTimeout).toInt();
    set.endGroup();

    set.beginGroup("VideoConfig3");
    VideoCount = set.value("VideoCount", VideoCount).toInt();
    VideoType = set.value("VideoType", VideoType).toString();
    VideoMax = set.value("VideoMax", VideoMax).toBool();
    VideoDrag = set.value("VideoDrag", VideoDrag).toBool();
    SaveVideo = set.value("SaveVideo", SaveVideo).toBool();    
    VideoSmart = set.value("VideoSmart", VideoSmart).toBool();
    OnvifEvent = set.value("OnvifEvent", OnvifEvent).toBool();
    OnvifNtp = set.value("OnvifNtp", OnvifNtp).toBool();
    set.endGroup();

    set.beginGroup("VideoConfig4");
    CheckOffline = set.value("CheckOffline", CheckOffline).toBool();
    TreeBig = set.value("TreeBig", TreeBig).toBool();
    TreeRtsp = set.value("TreeRtsp", TreeRtsp).toBool();
    RtspType = set.value("RtspType", RtspType).toInt();
    AlarmSaveTime = set.value("AlarmSaveTime", AlarmSaveTime).toInt();    
    OpenIpcPolicy = set.value("OpenIpcPolicy", OpenIpcPolicy).toInt();
    OpenNvrPolicy = set.value("OpenNvrPolicy", OpenNvrPolicy).toInt();
    OpenMaxCount = set.value("OpenMaxCount", OpenMaxCount).toInt();
    TableDataPolicy = set.value("TableDataPolicy", TableDataPolicy).toInt();
    ChannelBgText = set.value("ChannelBgText", ChannelBgText).toInt();
    DefaultChName = set.value("DefaultChName", DefaultChName).toString();
    DefaultIpcName = set.value("DefaultIpcName", DefaultIpcName).toString();
    DefaultNvrName = set.value("DefaultNvrName", DefaultNvrName).toString();
    set.endGroup();

    set.beginGroup("EnableConfig1");
    EnableChannelx = set.value("EnableChannelx", EnableChannelx).toBool();
    EnableChannel1 = set.value("EnableChannel1", EnableChannel1).toBool();
    EnableChannel4 = set.value("EnableChannel4", EnableChannel4).toBool();
    EnableChannel6 = set.value("EnableChannel6", EnableChannel6).toBool();
    EnableChannel8 = set.value("EnableChannel8", EnableChannel8).toBool();
    EnableChannel9 = set.value("EnableChannel9", EnableChannel9).toBool();
    EnableChannel13 = set.value("EnableChannel13", EnableChannel13).toBool();
    EnableChannel16 = set.value("EnableChannel16", EnableChannel16).toBool();
    EnableChannel25 = set.value("EnableChannel25", EnableChannel25).toBool();
    EnableChannel36 = set.value("EnableChannel36", EnableChannel36).toBool();
    EnableChannel64 = set.value("EnableChannel64", EnableChannel64).toBool();
    set.endGroup();

    set.beginGroup("EnableConfig2");
    EnableMain1 = set.value("EnableMain1", EnableMain1).toBool();
    EnableWeather = set.value("EnableWeather", EnableWeather).toBool();
    EnableVideoPanelTool = set.value("EnableVideoPanelTool", EnableVideoPanelTool).toBool();
    EnableMapImage = set.value("EnableMapImage", EnableMapImage).toBool();
    EnableVideoPlayWeb = set.value("EnableVideoPlayWeb", EnableVideoPlayWeb).toBool();
    EnableVideoPlayNvr = set.value("EnableVideoPlayNvr", EnableVideoPlayNvr).toBool();
    EnableVideoPlayImage = set.value("EnableVideoPlayImage", EnableVideoPlayImage).toBool();
    EnableVideoUpload = set.value("EnableVideoUpload", EnableVideoUpload).toBool();
    EnableConfigPoll = set.value("EnableConfigPoll", EnableConfigPoll).toBool();
    EnableConfigUser = set.value("EnableConfigUser", EnableConfigUser).toBool();
    EnableConfigRecord = set.value("EnableConfigRecord", EnableConfigRecord).toBool();
    EnableConfigOther = set.value("EnableConfigOther", EnableConfigOther).toBool();
    EnableTitleMenu = set.value("EnableTitleMenu", EnableTitleMenu).toBool();
    EnableModelMenu = set.value("EnableModelMenu", EnableModelMenu).toBool();
    set.endGroup();

    set.beginGroup("PollConfig");
    AutoPoll = set.value("AutoPoll", AutoPoll).toBool();
    Polling = set.value("Polling", Polling).toBool();
    PollInterval = set.value("PollInterval", PollInterval).toInt();
    PollType = set.value("PollType", PollType).toInt();
    PollRtsp = set.value("PollRtsp", PollRtsp).toInt();    
    PollGroupNames = set.value("PollGroupNames", PollGroupNames).toString();
    PollGroupLast = set.value("PollGroupLast", PollGroupLast).toString();
    set.endGroup();

    set.beginGroup("PlusConfig");
    PlusType = set.value("PlusType", PlusType).toInt();
    PlusNumber = set.value("PlusNumber", PlusNumber).toInt();
    PlusNvr = set.value("PlusNvr", PlusNvr).toBool();
    PlusAddr = set.value("PlusAddr", PlusAddr).toString();
    PlusMain = set.value("PlusMain", PlusMain).toString();
    PlusSub = set.value("PlusSub", PlusSub).toString();
    set.endGroup();

    set.beginGroup("NvrConfig");
    NvrCompanyType = set.value("NvrCompanyType", NvrCompanyType).toInt();
    NvrVideoType = set.value("NvrVideoType", NvrVideoType).toInt();    
    NvrUserName = set.value("NvrUserName", NvrUserName).toString();
    NvrUserPwd = set.value("NvrUserPwd", NvrUserPwd).toString();
    NvrDeviceIP = set.value("NvrDeviceIP", NvrDeviceIP).toString();
    NvrSyncPosition = set.value("NvrSyncPosition", NvrSyncPosition).toBool();
    NvrAutoCh = set.value("NvrAutoCh", NvrAutoCh).toBool();
    NvrStreamType = set.value("NvrStreamType", NvrStreamType).toInt();
    NvrDateTimeStart = set.value("NvrDateTimeStart", NvrDateTimeStart).toString();
    NvrDateTimeEnd = set.value("NvrDateTimeEnd", NvrDateTimeEnd).toString();
    set.endGroup();

    set.beginGroup("SearchConfig");
    SearchUserName = set.value("SearchUserName", SearchUserName).toString();
    SearchUserPwd = set.value("SearchUserPwd", SearchUserPwd).toString();
    SearchLocalIP = set.value("SearchLocalIP", SearchLocalIP).toString();
    SearchDeviceIP = set.value("SearchDeviceIP", SearchDeviceIP).toString();
    SearchFilter = set.value("SearchFilter", SearchFilter).toString();
    SearchFilters = set.value("SearchFilters", SearchFilters).toString();
    SearchInterval = set.value("SearchInterval", SearchInterval).toInt();
    SearchTimeout = set.value("SearchTimeout", SearchTimeout).toInt();
    SearchClear = set.value("SearchClear", SearchClear).toBool();
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    LocalAutoInfo = set.value("LocalAutoInfo", LocalAutoInfo).toBool();
    LocalConnMode = set.value("LocalConnMode", LocalConnMode).toInt();
    LocalDbType = set.value("LocalDbType", LocalDbType).toString();
    LocalDbName = set.value("LocalDbName", LocalDbName).toString();
    LocalHostName = set.value("LocalHostName", LocalHostName).toString();
    LocalHostPort = set.value("LocalHostPort", LocalHostPort).toInt();
    LocalUserName = set.value("LocalUserName", LocalUserName).toString();
    LocalUserPwd = set.value("LocalUserPwd", LocalUserPwd).toString();
    set.endGroup();

    set.beginGroup("MapConfig");
    MapVersionKey = set.value("MapVersionKey", MapVersionKey).toString();
    MapCenter = set.value("MapCenter", MapCenter).toString();
    MapZoom = set.value("MapZoom", MapZoom).toInt();
    set.endGroup();

    set.beginGroup("RouteConfig");
    RouteStartAddr = set.value("RouteStartAddr", RouteStartAddr).toString();
    RouteEndAddr = set.value("RouteEndAddr", RouteEndAddr).toString();
    RouteType = set.value("RouteType", RouteType).toInt();
    RoutePointCount = set.value("RoutePointCount", RoutePointCount).toInt();
    RouteDeviceName = set.value("RouteDeviceName", RouteDeviceName).toString();
    RouteMoveInterval = set.value("RouteMoveInterval", RouteMoveInterval).toInt();
    RouteMoveMode = set.value("RouteMoveMode", RouteMoveMode).toInt();
    set.endGroup();

    set.beginGroup("UploadConfig");
    UploadHost = set.value("UploadHost", UploadHost).toString();
    UploadPort = set.value("UploadPort", UploadPort).toInt();
    set.endGroup();

    set.beginGroup("ColorConfig");
    ColorTextNormal = set.value("ColorTextNormal", ColorTextNormal).toString();
    ColorTextWarn = set.value("ColorTextWarn", ColorTextWarn).toString();
    ColorTextAlarm = set.value("ColorTextAlarm", ColorTextAlarm).toString();
    ColorIconAlarm = set.value("ColorIconAlarm", ColorIconAlarm).toString();
    ColorMsgWait = set.value("ColorMsgWait", ColorMsgWait).toString();
    ColorMsgSolved = set.value("ColorMsgSolved", ColorMsgSolved).toString();
    set.endGroup();

    set.beginGroup("ComConfig");
    PortNameA = set.value("PortNameA", PortNameA).toString();
    PortNameB = set.value("PortNameB", PortNameB).toString();
    PortNameC = set.value("PortNameC", PortNameC).toString();
    PortNameD = set.value("PortNameD", PortNameD).toString();
    BaudRateA = set.value("BaudRateA", BaudRateA).toInt();
    BaudRateB = set.value("BaudRateB", BaudRateB).toInt();
    BaudRateC = set.value("BaudRateC", BaudRateC).toInt();
    BaudRateD = set.value("BaudRateD", BaudRateD).toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    TcpServerHost = set.value("TcpServerHost", TcpServerHost).toString();
    TcpServerPort = set.value("TcpServerPort", TcpServerPort).toInt();
    TcpListenPort = set.value("TcpListenPort", TcpListenPort).toInt();
    UdpListenPort = set.value("UdpListenPort", UdpListenPort).toInt();
    set.endGroup();

    set.beginGroup("WeatherConfig");
    WeatherHttp = set.value("WeatherHttp", WeatherHttp).toString();
    WeatherCity = set.value("WeatherCity", WeatherCity).toString();
    WeatherInterval = set.value("WeatherInterval", WeatherInterval).toInt();
    WeatherStyle = set.value("WeatherStyle", WeatherStyle).toInt();
    set.endGroup();

    set.beginGroup("DemoConfig");
    IndexDemo = set.value("IndexDemo", IndexDemo).toInt();
    VideoImageUrl = set.value("VideoImageUrl", VideoImageUrl).toString();
    VideoWidgetUrl = set.value("VideoWidgetUrl", VideoWidgetUrl).toString();
    VideoSaveUrl = set.value("VideoSaveUrl", VideoSaveUrl).toString();
    VideoPlusUrl1 = set.value("VideoPlusUrl1", VideoPlusUrl1).toString();
    VideoPlusUrl2 = set.value("VideoPlusUrl2", VideoPlusUrl2).toString();
    VideoPlayUrl1 = set.value("VideoPlayUrl1", VideoPlayUrl1).toString();
    VideoPlayUrl2 = set.value("VideoPlayUrl2", VideoPlayUrl2).toString();
    VideoPlayUrl3 = set.value("VideoPlayUrl3", VideoPlayUrl3).toString();
    VideoPlayUrl4 = set.value("VideoPlayUrl4", VideoPlayUrl4).toString();
    VideoAudioUrl = set.value("VideoAudioUrl", VideoAudioUrl).toString();
    VideoDragUrl = set.value("VideoDragUrl", VideoDragUrl).toString();
    VideoChangeUrl1 = set.value("VideoChangeUrl1", VideoChangeUrl1).toString();
    VideoChangeUrl2 = set.value("VideoChangeUrl2", VideoChangeUrl2).toString();
    VideoDrawUrl = set.value("VideoDrawUrl", VideoDrawUrl).toString();
    VideoOsdUrl = set.value("VideoOsdUrl", VideoOsdUrl).toString();
    set.endGroup();

    checkConfig();
}

void AppConfig::checkConfig()
{
    //过滤通道数量
    AppConfig::VideoCount = (AppConfig::VideoCount < 16 ? 16 : AppConfig::VideoCount);
    if (AppConfig::VideoCount <= 16) {
        AppConfig::EnableChannel25 = false;
        AppConfig::EnableChannel36 = false;
        AppConfig::EnableChannel64 = false;
    } else if (AppConfig::VideoCount <= 25) {
        AppConfig::EnableChannel36 = false;
        AppConfig::EnableChannel64 = false;
    } else if (AppConfig::VideoCount <= 36) {
        AppConfig::EnableChannel64 = false;
    }

    //配置文件不存在或者有问题则重新生成
    if (!QtHelper::checkIniFile(ConfigFile)) {
        //首次启动将窗体位置移到中间
        QRect deskRect = QtHelper::checkCenterRect(FormGeometry);
        //小分辨率重新调整
        if (QtHelper::deskWidth() <= 1366) {
            FormMax = true;
            FormGeometry = deskRect;
        }

        //重新设置默认选择目录
        AppConfig::SelectDirName = QtHelper::appPath() + "/db";
        writeConfig();
        return;
    }

    //加密过的密码在这里解密
    NvrUserPwd = QtHelper::getXorEncryptDecrypt(NvrUserPwd, 100);
    SearchUserPwd = QtHelper::getXorEncryptDecrypt(SearchUserPwd, 100);
    LocalUserPwd = QtHelper::getXorEncryptDecrypt(LocalUserPwd, 100);
}

void AppConfig::writeConfig()
{
    QSettings set(ConfigFile, QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    set.setIniCodec("utf-8");
#endif

    set.beginGroup("AppConfig1");
    set.setValue("OpenGLType", OpenGLType);
    set.setValue("IndexStart", IndexStart);
    set.setValue("WorkMode", WorkMode);
    set.setValue("NavStyle", NavStyle);
    set.setValue("StyleName", StyleName);
    set.setValue("LogoImage", LogoImage);
    set.setValue("CompanyHttp", CompanyHttp);
    set.setValue("CompanyTel", CompanyTel);
    set.setValue("Copyright", Copyright);
    set.setValue("TitleCn", TitleCn);
    set.setValue("TitleEn", TitleEn);
    set.setValue("SynTimeInterval", SynTimeInterval);
    set.endGroup();

    set.beginGroup("AppConfig2");
    set.setValue("RecordsPerpage", RecordsPerpage);
    set.setValue("PageButtonCount", PageButtonCount);
    set.setValue("MsgListCount", MsgListCount);
    set.setValue("MsgTableCount", MsgTableCount);
    set.setValue("TimeHideCursor", TimeHideCursor);
    set.setValue("TimeAutoFull", TimeAutoFull);
    set.setValue("AlarmSoundCount", AlarmSoundCount);
    set.setValue("WindowOpacity", WindowOpacity);
    set.setValue("TipInterval", TipInterval);
    set.endGroup();

    set.beginGroup("AppConfig3");
    set.setValue("AutoRun", AutoRun);
    set.setValue("AutoLogin", AutoLogin);
    set.setValue("AutoPwd", AutoPwd);
    set.setValue("SynTime", SynTime);
    set.setValue("SaveLog", SaveLog);
    set.setValue("SaveRunTime", SaveRunTime);
    set.setValue("RightInfo", RightInfo);
    set.setValue("SaveUrl", SaveUrl);
    set.endGroup();

    set.beginGroup("AppConfig4");
    set.setValue("FormMax", FormMax);
    set.setValue("FormFull", FormFull);
    set.setValue("FormGeometry", FormGeometry);
    set.setValue("PollGeometry", PollGeometry);
    set.setValue("SelectDirName", SelectDirName);
    set.setValue("ExpandItem", ExpandItem);    
    set.endGroup();

    set.beginGroup("AppConfig5");
    set.setValue("LastLoginer", LastLoginer);
    set.setValue("LastFormMain", LastFormMain);
    set.setValue("LastFormVideo", LastFormVideo);
    set.setValue("LastFormMap", LastFormMap);
    set.setValue("LastFormData", LastFormData);
    set.setValue("LastFormConfig", LastFormConfig);
    set.setValue("DeviceIP", DeviceIP);
    set.endGroup();

    set.beginGroup("VisibleConfig");
    set.setValue("FullScreen", FullScreen);
    set.setValue("VisibleIpcSearch", VisibleIpcSearch);
    set.setValue("VisiblePollPlus", VisiblePollPlus);
    set.setValue("VisiblePtzStep", VisiblePtzStep);
    set.endGroup();

    set.beginGroup("VideoConfig1");
    set.setValue("ScaleMode", ScaleMode);
    set.setValue("VideoMode", VideoMode);
    set.setValue("BorderWidth", BorderWidth);
    set.setValue("BannerPosition", BannerPosition);
    set.setValue("SoundValue", SoundValue);
    set.setValue("SoundMuted", SoundMuted);
    set.setValue("SharedData", SharedData);
    set.endGroup();

    set.beginGroup("VideoConfig2");

    set.setValue("Hardware", Hardware);
    set.setValue("Transport", Transport);
    set.setValue("DecodeType", DecodeType);
    set.setValue("Caching", Caching);
    set.setValue("AudioLevel", AudioLevel);
    set.setValue("PlayAudio", PlayAudio);
    set.setValue("CountKbps", CountKbps);
    set.setValue("EncodeType", EncodeType);
    set.setValue("EncodeVideoRatio", (double)EncodeVideoRatio);
    set.setValue("ReadTimeout", ReadTimeout);
    set.setValue("ConnectTimeout", ConnectTimeout);
    set.endGroup();

    set.beginGroup("VideoConfig3");
    set.setValue("VideoCount", VideoCount);
    set.setValue("VideoType", VideoType);
    set.setValue("VideoMax", VideoMax);
    set.setValue("VideoDrag", VideoDrag);
    set.setValue("SaveVideo", SaveVideo);    
    set.setValue("VideoSmart", VideoSmart);
    set.setValue("OnvifEvent", OnvifEvent);
    set.setValue("OnvifNtp", OnvifNtp);
    set.endGroup();

    set.beginGroup("VideoConfig4");
    set.setValue("CheckOffline", CheckOffline);
    set.setValue("TreeBig", TreeBig);
    set.setValue("TreeRtsp", TreeRtsp);
    set.setValue("RtspType", RtspType);
    set.setValue("AlarmSaveTime", AlarmSaveTime);    
    set.setValue("OpenIpcPolicy", OpenIpcPolicy);
    set.setValue("OpenNvrPolicy", OpenNvrPolicy);
    set.setValue("OpenMaxCount", OpenMaxCount);
    set.setValue("TableDataPolicy", TableDataPolicy);
    set.setValue("ChannelBgText", ChannelBgText);
    set.setValue("DefaultChName", DefaultChName);
    set.setValue("DefaultIpcName", DefaultIpcName);
    set.setValue("DefaultNvrName", DefaultNvrName);
    set.endGroup();

    set.beginGroup("EnableConfig1");
    set.setValue("EnableChannelx", EnableChannelx);
    set.setValue("EnableChannel1", EnableChannel1);
    set.setValue("EnableChannel4", EnableChannel4);
    set.setValue("EnableChannel6", EnableChannel6);
    set.setValue("EnableChannel8", EnableChannel8);
    set.setValue("EnableChannel9", EnableChannel9);
    set.setValue("EnableChannel13", EnableChannel13);
    set.setValue("EnableChannel16", EnableChannel16);
    set.setValue("EnableChannel25", EnableChannel25);
    set.setValue("EnableChannel36", EnableChannel36);
    set.setValue("EnableChannel64", EnableChannel64);
    set.endGroup();

    set.beginGroup("EnableConfig2");
    set.setValue("EnableMain1", EnableMain1);
    set.setValue("EnableWeather", EnableWeather);
    set.setValue("EnableVideoPanelTool", EnableVideoPanelTool);
    set.setValue("EnableMapImage", EnableMapImage);
    set.setValue("EnableVideoPlayWeb", EnableVideoPlayWeb);
    set.setValue("EnableVideoPlayNvr", EnableVideoPlayNvr);
    set.setValue("EnableVideoPlayImage", EnableVideoPlayImage);
    set.setValue("EnableVideoUpload", EnableVideoUpload);
    set.setValue("EnableConfigPoll", EnableConfigPoll);
    set.setValue("EnableConfigUser", EnableConfigUser);
    set.setValue("EnableConfigRecord", EnableConfigRecord);
    set.setValue("EnableConfigOther", EnableConfigOther);
    set.setValue("EnableTitleMenu", EnableTitleMenu);
    set.setValue("EnableModelMenu", EnableModelMenu);
    set.endGroup();

    set.beginGroup("PollConfig");
    set.setValue("AutoPoll", AutoPoll);
    set.setValue("Polling", Polling);
    set.setValue("PollInterval", PollInterval);
    set.setValue("PollType", PollType);
    set.setValue("PollRtsp", PollRtsp);
    set.setValue("PollGroupNames", PollGroupNames);
    set.setValue("PollGroupLast", PollGroupLast);
    set.endGroup();

    set.beginGroup("PlusConfig");
    set.setValue("PlusType", PlusType);
    set.setValue("PlusNumber", PlusNumber);
    set.setValue("PlusNvr", PlusNvr);
    set.setValue("PlusAddr", PlusAddr);
    set.setValue("PlusMain", PlusMain);
    set.setValue("PlusSub", PlusSub);
    set.endGroup();

    set.beginGroup("NvrConfig");
    set.setValue("NvrCompanyType", NvrCompanyType);
    set.setValue("NvrVideoType", NvrVideoType);    
    set.setValue("NvrUserName", NvrUserName);
    set.setValue("NvrUserPwd", QtHelper::getXorEncryptDecrypt(NvrUserPwd, 100));
    set.setValue("NvrDeviceIP", NvrDeviceIP);
    set.setValue("NvrSyncPosition", NvrSyncPosition);
    set.setValue("NvrAutoCh", NvrAutoCh);
    set.setValue("NvrStreamType", NvrStreamType);
    set.setValue("NvrDateTimeStart", NvrDateTimeStart);
    set.setValue("NvrDateTimeEnd", NvrDateTimeEnd);
    set.endGroup();

    set.beginGroup("SearchConfig");
    set.setValue("SearchUserName", SearchUserName);
    set.setValue("SearchUserPwd", QtHelper::getXorEncryptDecrypt(SearchUserPwd, 100));
    set.setValue("SearchLocalIP", SearchLocalIP);
    set.setValue("SearchDeviceIP", SearchDeviceIP);
    set.setValue("SearchFilter", SearchFilter);
    set.setValue("SearchFilters", SearchFilters);
    set.setValue("SearchInterval", SearchInterval);
    set.setValue("SearchTimeout", SearchTimeout);
    set.setValue("SearchClear", SearchClear);
    set.endGroup();

    set.beginGroup("LocalDbConfig");
    set.setValue("LocalAutoInfo", LocalAutoInfo);
    set.setValue("LocalConnMode", LocalConnMode);
    set.setValue("LocalDbType", LocalDbType);
    set.setValue("LocalDbName", LocalDbName);
    set.setValue("LocalHostName", LocalHostName);
    set.setValue("LocalHostPort", LocalHostPort);
    set.setValue("LocalUserName", LocalUserName);
    set.setValue("LocalUserPwd", QtHelper::getXorEncryptDecrypt(LocalUserPwd, 100));
    set.endGroup();

    set.beginGroup("MapConfig");
    set.setValue("MapVersionKey", MapVersionKey);
    set.setValue("MapCenter", MapCenter);
    set.setValue("MapZoom", MapZoom);
    set.endGroup();

    set.beginGroup("RouteConfig");
    set.setValue("RouteStartAddr", RouteStartAddr);
    set.setValue("RouteEndAddr", RouteEndAddr);
    set.setValue("RouteType", RouteType);
    set.setValue("RoutePointCount", RoutePointCount);
    set.setValue("RouteDeviceName", RouteDeviceName);
    set.setValue("RouteMoveInterval", RouteMoveInterval);
    set.setValue("RouteMoveMode", RouteMoveMode);
    set.endGroup();

    set.beginGroup("UploadConfig");
    set.setValue("UploadHost", UploadHost);
    set.setValue("UploadPort", UploadPort);
    set.endGroup();

    set.beginGroup("ColorConfig");
    set.setValue("ColorTextNormal", ColorTextNormal);
    set.setValue("ColorTextWarn", ColorTextWarn);
    set.setValue("ColorTextAlarm", ColorTextAlarm);
    set.setValue("ColorIconAlarm", ColorIconAlarm);
    set.setValue("ColorMsgWait", ColorMsgWait);
    set.setValue("ColorMsgSolved", ColorMsgSolved);
    set.endGroup();

    set.beginGroup("ComConfig");
    set.setValue("PortNameA", PortNameA);
    set.setValue("PortNameB", PortNameB);
    set.setValue("PortNameC", PortNameC);
    set.setValue("PortNameD", PortNameD);
    set.setValue("BaudRateA", BaudRateA);
    set.setValue("BaudRateB", BaudRateB);
    set.setValue("BaudRateC", BaudRateC);
    set.setValue("BaudRateD", BaudRateD);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("TcpServerHost", TcpServerHost);
    set.setValue("TcpServerPort", TcpServerPort);
    set.setValue("TcpListenPort", TcpListenPort);
    set.setValue("UdpListenPort", UdpListenPort);
    set.endGroup();

    set.beginGroup("WeatherConfig");
    set.setValue("WeatherHttp", WeatherHttp);
    set.setValue("WeatherCity", WeatherCity);
    set.setValue("WeatherInterval", WeatherInterval);
    set.setValue("WeatherStyle", WeatherStyle);
    set.endGroup();

    set.beginGroup("DemoConfig");
    set.setValue("IndexDemo", IndexDemo);
    set.setValue("VideoImageUrl", VideoImageUrl);
    set.setValue("VideoWidgetUrl", VideoWidgetUrl);
    set.setValue("VideoSaveUrl", VideoSaveUrl);
    set.setValue("VideoPlusUrl1", VideoPlusUrl1);
    set.setValue("VideoPlusUrl2", VideoPlusUrl2);
    set.setValue("VideoPlayUrl1", VideoPlayUrl1);
    set.setValue("VideoPlayUrl2", VideoPlayUrl2);
    set.setValue("VideoPlayUrl3", VideoPlayUrl3);
    set.setValue("VideoPlayUrl4", VideoPlayUrl4);
    set.setValue("VideoAudioUrl", VideoAudioUrl);
    set.setValue("VideoDragUrl", VideoDragUrl);
    set.setValue("VideoChangeUrl1", VideoChangeUrl1);
    set.setValue("VideoChangeUrl2", VideoChangeUrl2);
    set.setValue("VideoDrawUrl", VideoDrawUrl);
    set.setValue("VideoOsdUrl", VideoOsdUrl);
    set.endGroup();
}

QByteArray AppConfig::readLayout(const QString &file)
{
    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("MainWindow");
    QByteArray data = set.value("State").toByteArray();
    set.endGroup();
    return data;
}

void AppConfig::writeLayout(const QString &file, const QByteArray &data)
{
    QSettings set(file, QSettings::IniFormat);
    set.beginGroup("MainWindow");
    set.setValue("State", data);
    set.endGroup();
}
