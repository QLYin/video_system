#ifndef DATAXLS_H
#define DATAXLS_H

#include "datahead.h"

class DataXls : public QThread
{
    Q_OBJECT
public:
    //全局导出数据类
    static DataXls *dataXls;
    //导出数据到xls
    static void saveXls(const DataContent &dataContent);
    static void saveXls(const QString &fileName,
                        const QString &sheetName,
                        const QString &title,
                        const QList<QString> &columnNames,
                        const QList<int> &columnWidths,
                        const QStringList &content);

    explicit DataXls(QObject *parent = 0);
    ~DataXls();

protected:
    void run();

private:
    QMutex mutex;               //共享锁
    volatile bool stopped;      //标志位用于停止线程
    QElapsedTimer time;         //计算用时对象

    int columnCount;            //列数
    QFile file;                 //文件对象
    QTextStream stream;         //数据流对象
    DataContent dataContent;    //数据内容

public:
    //获取执行所用时间
    QString getUseTime();
    //获取数据类型字符串
    QString getDataType(const QString &data);

public slots:
    //输出头部信息
    void appendHead();
    //输出文档信息
    void appendDocument();
    //输出样式信息
    void appendStyle();
    void appendStyle(const QString &id, const QString &align,
                     bool border, int fontSize = 10, int fontBold = 0,
                     const QString &textColor = "#000000", const QString &bgColor = QString());
    //输出表结构
    void appendTable();
    //输出字段名称
    void appendColumn();
    //输出结尾信息
    void appendEnd();

    //输出当前队列内容
    void appendContent(const QStringList &listContent);
    //核心处理循环输出内容
    void takeContent();

public slots:
    //设置数据内容
    void setDataContent(const DataContent &dataContent);

    //初始化
    void init();
    //打开文件准备输出数据
    void open();
    //关闭文件
    void close();

    //暂停线程
    void stop();
    //追加数据
    void append(const QString &content, const QString &subTitle1 = "", const QString &subTitle2 = "");

signals:
    //处理完一批次内容
    void appendFinshed(int count, int mesc);
};

#endif // DATAXLS_H
