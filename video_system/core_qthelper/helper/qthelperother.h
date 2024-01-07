#ifndef QTHELPEROTHER_H
#define QTHELPEROTHER_H

#include <QObject>
#include <QColor>
#include <QElapsedTimer>
class QTextEdit;
class QTableView;
class QAbstractButton;
class QDateTimeEdit;

class QtHelperOther
{
public:
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
    static QList<QColor> colors;
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
    static bool replaceCRLF;
    static QVector<int> msgTypes;
    static QVector<QString> msgKeys;
    static QVector<QColor> msgColors;
    static void resetMsgMap();
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

#endif // QTHELPEROTHER_H
