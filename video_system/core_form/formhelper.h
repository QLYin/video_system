#ifndef FORMHELPER_H
#define FORMHELPER_H

#include "head.h"

class FormHelper
{
public:
    //系统重启
    static void reboot();

    //获取默认文件夹
    static QString getDefaultDir();
    //保存最后选择的文件夹
    static void saveSelectDirName(const QString &fileName);

    //打开及保存文件
    static QString getOpenFileName(const QString &defaultName, const QString &filter);
    static QString getSaveFileName(const QString &defaultName, const QString &filter);

    //导入导出数据
    static bool inputData(QSqlTableModel *model, const QStringList &columnNames,
                          const QString &tableName, const QString &flag);
    static bool outputData(const QString &orderColumn, const QStringList &columnNames,
                           const QString &tableName, const QString &flag);
    //自动备份数据
    static bool outputData(const QString &tableName, const QString &flag);

    //校验导出的数据是否超过限制
    static bool checkRowCount(quint8 type, int rowCount);
    //导出数据到xls/pdf及打印数据 type 0-打印 1-导出pdf 2-导出xls
    static void dataout(quint8 type, const QList<QString> &columnNames, const QList<int> &columnWidths,
                        const QString &name, const QString &table,
                        const QString &columns, const QString &where,                        
                        bool landscape = false, bool nameWithDate = false,
                        const QStringList &listContent = QStringList());

    //自动设置控件XY坐标位置
    static void checkPosition(int &x, int &y, int width, int height);
};

#endif // FORMHELPER_H
