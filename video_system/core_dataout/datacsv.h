#ifndef DATACSV_H
#define DATACSV_H

#include "datahead.h"

class DataCsv
{
public:
    //是否开启字符串转换(保证字符前面的0不被优化)
    static bool CsvString;
    //保存文件对应的 分隔符/拓展名/过滤条件
    static QString CsvFilter;
    static QString CsvExtension;
    static QString CsvSpliter;

    //获取指定表名字段数据内容
    static QStringList getContent(const QString &tableName,
                                  const QString &columnNames,
                                  const QString &whereSql = QString(),
                                  const QString &title = QString(),
                                  const QString &spliter = DataCsv::CsvSpliter);

    //初始化文本流
    static void initTextStream(const QString &fileName, QTextStream *stream);
    //判断是否有BOM rewrite=true 表示移除BOM后重写文件
    static bool existBom(const QString &fileName, bool rewrite = false);
    //检查文件编码 0=ANSI 1=UTF-16LE 2=UTF-16BE 3=UTF-8 4=UTF-8BOM
    static int findCode(const QString &fileName, QString &flag);

    //下面的参数 getContent中的title和inputData中的existTitle必须一样 有标题字段则existTitle=true
    //导入数据文件 字段名为空则表示所有字段
    static bool inputData(int columnCount,
                          const QString &columnNames,
                          const QString &tableName,                          
                          QString &fileName,
                          const QString &defaultDir = QCoreApplication::applicationDirPath(),
                          bool existTitle = false);
    //导入数据
    static bool inputData(int columnCount,
                          const QString &columnNames,
                          const QString &tableName,
                          QString &fileName,
                          bool existTitle = false);

    //导出数据文件
    static bool outputData(const QString &defaultName,
                           const QStringList &content,
                           QString &fileName,
                           const QString &defaultDir = QCoreApplication::applicationDirPath());
    //导出数据
    static bool outputData(QString &fileName, const QStringList &content);
};

#endif // DATACSV_H
