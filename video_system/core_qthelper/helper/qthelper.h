#ifndef QTHELPER_H
#define QTHELPER_H

#include "globalhead.h"
#include "globalconfig.h"
#include "qtabout.h"

class QtHelper
{
public:
    //获取所有屏幕区域/当前鼠标所在屏幕索引/区域尺寸/缩放系数
    static QList<QRect> getScreenRects(bool available = true);
    static int getScreenIndex();
    static QRect getScreenRect(bool available = true);
    static qreal getScreenRatio(int index = -1, bool devicePixel = false);

    //矫正当前鼠标所在屏幕居中尺寸
    static QRect checkCenterRect(QRect &rect, bool available = true);

    //获取桌面宽度高度
    static int deskWidth();
    static int deskHeight();
    static QSize deskSize();

    //居中显示窗体
    //定义标志位指定是以桌面为参照还是主程序界面为参照
    static QWidget *centerBaseForm;
    static void setFormInCenter(QWidget *form);
    static void showForm(QWidget *form);

    //程序文件名称及所在路径
    static QString appName();
    static QString appPath();

    //程序最前面获取应用程序路径和名称
    static void getCurrentInfo(char *argv[], QString &path, QString &name);
    //程序最前面读取配置文件节点的值
    static QString getIniValue(const QString &fileName, const QString &key);
    static QString getIniValue(char *argv[], const QString &key, const QString &dir = QString());

    //获取编译器字符串
    static QString getCompilerString();
    //获取uuid
    static QString getUuid();

    //校验目录(不存在则新建)
    static void checkPath(const QString &path);
    static void checkPaths(const QString &paths);
    //通用延时函数(支持Qt4 Qt5 Qt6)
    static void sleep(int msec, bool exec = true);
    //检查程序是否已经运行
    static void checkRun();

    //设置Qt自带样式
    static void setStyle();
    //设置编码
    static void setCode(bool utf8 = true);
    //设置字体
    static QFont addFont(const QString &fontFile, const QString &fontName);
    static void setFont(const QString &fontFile = ":/font/DroidSansFallback.ttf",
                        const QString &fontName = GlobalConfig::FontName,
                        int fontSize = GlobalConfig::FontSize);
    //设置翻译文件
    static void setTranslator();
    static void setTranslator(const QString &qmFile);

    //动态设置权限
    static bool checkPermission(const QString &permission);
    //申请安卓权限
    static void initAndroidPermission();

    //一次性设置所有包括编码字体翻译等
    static void initAll(bool utf8 = true);
    //初始化main函数最前面执行的一段代码
    static void initMain(bool desktopSettingsAware = false, bool use96Dpi = true, bool logCritical = true);
    //初始化opengl类型(1=AA_UseDesktopOpenGL 2=AA_UseOpenGLES 3=AA_UseSoftwareOpenGL)
    static void initOpenGL(quint8 type = 0, bool checkCardEnable = false, bool checkVirtualSystem = false);

    //执行命令行返回执行结果
    static QString doCmd(const QString &program, const QStringList &arguments, int timeout = 1000);
    //获取显卡是否被禁用
    static bool isVideoCardEnable();
    //获取是否在虚拟机环境
    static bool isVirtualSystem();

    //16进制字符串转10进制
    static int strHexToDecimal(const QString &strHex);
    //10进制字符串转10进制
    static int strDecimalToDecimal(const QString &strDecimal);
    //2进制字符串转10进制
    static int strBinToDecimal(const QString &strBin);

    //16进制字符串转2进制字符串
    static QString strHexToStrBin(const QString &strHex);
    //10进制转2进制字符串一个字节
    static QString decimalToStrBin1(int decimal);
    //10进制转2进制字符串两个字节
    static QString decimalToStrBin2(int decimal);
    //10进制转16进制字符串,补零.
    static QString decimalToStrHex(int decimal);

    //int和字节数组互转
    static QByteArray intToByte(int data, bool reverse = false);
    static int byteToInt(const QByteArray &data, bool reverse = false);

    //ushort和字节数组互转
    static QByteArray ushortToByte(int data, bool reverse = false);
    static int byteToShort(const QByteArray &data, bool reverse = false);

    //字符串补全
    static QString getValue(quint8 value);
    //字符串去空格 -1=移除左侧空格 0=移除所有空格 1=移除右侧空格 2=移除首尾空格 3=首尾清除中间留一个空格
    static QString trimmed(const QString &text, int type);

    //异或加密-只支持字符,如果是中文需要将其转换base64编码
    static QString getXorEncryptDecrypt(const QString &value, char key);
    //异或校验
    static quint8 getOrCode(const QByteArray &data);

    //公司专用-计算校验码
    static quint8 getCheckCode(const QByteArray &data);
    //公司专用-加上桢头和校验码完整数据
    static void getFullData(QByteArray &buffer);

    //CRC校验
    static QByteArray getCrcCode(const QByteArray &data);

    //字节数组与Ascii字符串互转
    static QString byteArrayToAsciiStr(const QByteArray &data);
    static QByteArray asciiStrToByteArray(const QString &data);

    //16进制字符串与字节数组互转
    static QByteArray hexStrToByteArray(const QString &data);
    static QString byteArrayToHexStr(const QByteArray &data);

    //选择文件对话框
    static QString getOpenFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //保存文件对话框
    static QString getSaveFileName(const QString &filter = QString(),
                                   const QString &dirName = QString(),
                                   const QString &fileName = QString(),
                                   bool native = false, int width = 900, int height = 600);
    //选择目录对话框
    static QString getExistingDirectory(const QString &dirName = QString(),
                                        bool native = false, int width = 900, int height = 600);

    //获取文件名,含拓展名
    static QString getFileNameWithExtension(const QString &fileName);
    //获取选择文件夹中的文件
    static QStringList getFolderFileNames(const QStringList &filter);

    //复制文件
    static bool copyFile(const QString &sourceFile, const QString &targetFile);
    //删除文件夹下所有文件
    static void deleteDirectory(const QString &path);

    //从字符串获取IP地址
    static QString getIP(const QString &url);
    //判断是否是IP地址
    static bool isIP(const QString &ip);
    //判断是否是MAC地址
    static bool isMac(const QString &mac);
    //判断是否是合法的电话号码
    static bool isTel(const QString &tel);
    //判断是否是合法的邮箱地址
    static bool isEmail(const QString &email);

    //IP地址字符串与整型转换
    static QString ipv4IntToString(quint32 ip);
    static quint32 ipv4StringToInt(const QString &ip);

    //判断主机地址及端口是否在线
    static bool hostLive(const QString &hostName, int port, int timeout = 1000);
    //下载网络文件
    static bool download(const QString &url, const QString &fileName, int timeout = 1000);
    //获取网页所有源代码
    static QByteArray getHtml(const QString &url, int timeout = 1000);

    //获取本机公网IP地址
    static QString getNetIP(const QString &html);
    //获取本机IP
    static QString getLocalIP();
    //获取本机IP地址集合
    static QStringList getLocalIPs();
    //Url地址转为IP地址
    static QString urlToIP(const QString &url);

    //设置边框阴影
    static void setFormShadow(QWidget *widget, QLayout *layout, const QString &color, int margin, int radius);
    //立即更新所有阴影边框颜色比如换肤的时候需要用到
    static void setFormShadow(const QString &color);

    //设置无边框窗体
    static void setFramelessForm(QWidget *widgetMain,
                                 bool tool = false, bool top = false,
                                 bool menu = true, bool x11 = false);
    static void setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                 QLabel *labIco, QPushButton *btnClose,
                                 bool tool = true, bool top = true,
                                 bool menu = false, bool x11 = true);

    //弹出自定义窗体
    static int showFormExec(QWidget *widget, const QString &title, bool windowModal = true);
    //通用弹出框
    static int showMessageBox(const QString &text, int type = 0, int timeout = 0, bool exec = false);
    //弹出消息框
    static void showMessageBoxInfo(const QString &text, int timeout = 0, bool exec = false);
    //弹出错误框
    static void showMessageBoxError(const QString &text, int timeout = 0, bool exec = false);
    //弹出询问框
    static int showMessageBoxQuestion(const QString &text);

    //弹出右下角信息框
    static void showTipBox(const QString &title, const QString &tip, bool fullScreen = false,
                           bool center = true, int timeout = 0);
    //隐藏右下角信息框
    static void hideTipBox();

    //弹出输入框
    static QString showInputBox(const QString &title, int type = 0, int timeout = 0,
                                const QString &placeholderText = QString(), bool pwd = false,
                                const QString &defaultValue = QString());
    //弹出日期选择框
    static int showDateSelect(QString &dateStart, QString &dateEnd, const QString &format = "yyyy-MM-dd");

    //弹出关于对话框
    static void showAboutInfo(const AboutInfo &info, int timeout = 0, bool exec = false);
    //弹出中间提示框
    static void showSplashInfo(const QString &text, int fontSizeMain, int fontSizeSub, int timeout = 0, bool exec = false);
    //隐藏中间提示框
    static void hideSplashInfo();

    //获取等比缩放过的图片
    static QPixmap getPixmap(QWidget *widget, const QPixmap &pixmap, bool scale = false);
    static void setPixmap(QLabel *label, const QString &file, bool scale = false);
    //设置logo图片支持 资源文件+本地图片+图形字体+svg自动变色 等多种形式
    static void setLogo(QLabel *label, const QString &file, int width, int height, int offset = 20,
                        const QString &oldColor = QString(), const QString &newColor = QString());

    //传入图片尺寸和窗体区域及边框大小返回居中区域(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static QRect getCenterRect(const QSize &imageSize, const QRect &widgetRect, int borderWidth = 2, int scaleMode = 0);
    //传入图片尺寸和窗体尺寸及缩放策略返回合适尺寸(scaleMode: 0-自动调整 1-等比缩放 2-拉伸填充)
    static void getScaledImage(QImage &image, const QSize &widgetSize, int scaleMode = 0, bool fast = true);

    //初始化数据库
    static void initDb(const QString &dbName);
    //初始化文件,不存在则拷贝
    static void initFile(const QString &sourceName, const QString &targetName);
    //检查ini配置文件
    static bool checkIniFile(const QString &iniFile);

    //设置图标到按钮
    static void setIconBtn(QAbstractButton *btn, const QString &png, int icon);

    //写入消息到额外的的消息日志文件
    static void writeInfo(const QString &text, bool needWrite = false, const QString &filePath = "log");
    static void writeError(const QString &text, bool needWrite = false, const QString &filePath = "log");

    //设置系统时间
    static void setSystemDateTime(const QString &year, const QString &month, const QString &day,
                                  const QString &hour, const QString &min, const QString &sec);
    //设置开机自启动
    static void runWithSystem(bool autoRun = true);
    static void runWithSystem(const QString &fileName, const QString &filePath, bool autoRun = true);
    //启动运行程序(已经在运行则不启动)
    static void runBin(const QString &path, const QString &name);

    //获取内置颜色集合
    static QList<QColor> getColorList();
    static QStringList getColorNames();
    //随机获取颜色集合中的颜色
    static QColor getRandColor();

    //初始化随机数种子
    static void initRand();
    //获取随机小数
    static float getRandFloat(float min, float max);
    //获取随机数,指定最小值和最大值
    static double getRandValue(int min, int max, bool contansMin = false, bool contansMax = false);
    //获取范围值随机经纬度集合
    static QStringList getRandPoint(int count, float mainLng, float mainLat, float dotLng, float dotLat);
    //根据旧的范围值和值计算新的范围值对应的值
    static int getRangeValue(int oldMin, int oldMax, int oldValue, int newMin, int newMax);

    //初始化日历
    static void initDataTimeEdit(QDateTimeEdit *dateTimeEdit, qint64 days);
    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);
    //打开文件带提示框
    static void openFile(const QString &fileName, const QString &msg);
    //导出和打印数据提示框
    static bool checkRowCount(int rowCount, int maxCount, int warnCount);

    //插入消息
    static QString appendMsg(QTextEdit *textEdit, int type, const QString &data,
                             int maxCount, int &currentCount,
                             bool clear = false, bool pause = false);
    //首尾截断字符串显示
    static QString cutString(const QString &text, int len, int left, int right, bool file, const QString &mid = "...");

    //毫秒数转时间 00:00
    static QString getTimeString(qint64 time);
    //用时时间转秒数
    static QString getTimeString(QElapsedTimer timer);
    //文件大小转 KB MB GB TB
    static QString getSizeString(quint64 size);    
};

#endif // QTHELPER_H
