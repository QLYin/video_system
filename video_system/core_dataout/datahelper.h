#ifndef DATAHELPER_H
#define DATAHELPER_H

#include "datahead.h"

class DataHelper
{
public:
    //获取当前鼠标所在屏幕索引+区域尺寸+缩放系数
    static int getScreenIndex();
    static qreal getScreenRatio(bool devicePixel = false);

    //校验列是否满足规则,可以自行拓展其他规则
    static bool checkColumn(const QString &sourceValue, const QString &checkType, const QString &checkValue);
    //获取默认最大数量
    static void getMaxCount(quint8 type, int &maxCount, int &warnCount);

    //找出对齐样式字符串 type=0 表示列名 =1表示内容
    static QString getAlignStyle(const DataContent &dataContent, quint8 type, int column);
    //校验结构体数据
    static bool checkDataContent(DataContent &dataContent);

    //通用数据导出+打印函数
    static void init();
    static void dataout(const DataContent &dataContent);
    //下面这个函数为了兼容早期的项目
    static void dataout(const QString &fileName,
                        const QString &sheetName,
                        const QString &title,
                        const QList<QString> &columnNames,
                        const QList<int> &columnWidths,
                        const QStringList &content,
                        bool landscape = false);

    //直接传入控件打印 type: 0=导出到csv 1=导出到xls 2=导出到pdf 3=打印
    static QString dataout(QTableView *table, QStandardItemModel *model,
                           quint8 type, const QString &file = QString(),
                           const QString &title = QString(), const QString &sheet = QString());
    static QString dataout(QTableWidget *table, quint8 type, const QString &file = QString(),
                           const QString &title = QString(), const QString &sheet = QString());
    static QString dataout(quint8 type, const QString &file, const QString &title,
                           const QString &sheet, const QStringList &content,
                           const QStringList &columnNames, const QList<int> &columnWidths);
};

#endif // DATAHELPER_H
