﻿#ifndef DBHELPER_H
#define DBHELPER_H

#include "dbhead.h"

class DbHelper
{
public:
    //打印数据库信息
    static void debugDbInfo();

    //获取数据库连接方式集合
    static QStringList getConnMode();
    //获取数据库类型字符串集合
    static QStringList getDbType();

    //数据库类型和类型字符串互相转换
    static QString getDbType(const DbType &dbType);
    static DbType getDbType(const QString &dbType);

    //根据数据库类型获取默认端口+用户信息
    static void getDbDefaultInfo(const QString &dbType, QString &hostPort,
                                 QString &userName, QString &userPwd);

    //绑定数据库表到模型
    static void bindTable(const QString &dbType, QSqlTableModel *model, const QString &table);

    //获取检测数据库连接语句
    static QString getSelect1Sql(const DbType &dbType);
    //获取查询记录数语句
    static QString getSelectCountSql(const DbType &dbType, const QString &table,
                                     const QString &column, const QString &where,
                                     const QString &order, int count);

    //程序文件名称+当前所在路径
    static QString appName();
    static QString appPath();
    //获取通用sqlite数据库文件
    static QString getDbDefaultFile(const QString &name = QString());

    //检查是否是不支持的sql语句
    static bool existNoSupportSql(const QString &sql);
    //重新纠正sql语句以便支持Qt直接执行
    static void checkSql(const QString &dbType, QString &sql);

    //表字段名和关键字冲突需要替换
    static void replaceColumnName(QString &sql, const QString &key);
    //表字段类型替换 比如不支持指定int类型长度,需要将sql语句中的 INTEGER(1) INTEGER(11) 等全部转成 INTEGER
    static void replaceColumnType(QString &sql, const QString &key);

    //检查数据库连接对象前置条件 比如sqlite要文件存在 其他数据库网络要通
    static bool hostLive(const QString &hostName, int port, int timeout = 1000);
    static bool checkDatabase(const QString &dbType, const DbInfo &dbInfo);

    //初始化数据库对象
    static bool initDatabase(bool testConnect, const QString &dbType,
                             QSqlDatabase &database, const DbInfo &dbInfo);
    //获取所有数据库用户表
    static QStringList getTables(const QString &date, const QString &dbType,
                                 QSqlDatabase database = QSqlDatabase::database());

    //执行sql语句
    static bool execSql(const QString &sql,
                        QSqlDatabase database = QSqlDatabase::database());
    //清空表数据并重置自增ID
    static void clearTable(const QString &tableName,
                           QSqlDatabase database = QSqlDatabase::database());
    static void clearTable(const QString &tableName, const QString &dbType,
                           QSqlDatabase database = QSqlDatabase::database());

    //取出最大编号 用程序方式插入记录 数据库的自增字段不通用
    static int getMaxID(const QString &tableName, const QString &columnName,
                        QSqlDatabase database = QSqlDatabase::database());
    //通用日期范围sql语句
    static void getBetweenDate(QString &sql, const QString &dbType, const QString &columnName,
                               const QString &dateTimeStart, const QString &dateTimeEnd,
                               bool varchar = true);

    //绑定数据到下拉框
    static void bindData(const QString &columnName, const QString &orderSql,
                         const QString &tableName, QComboBox *cbox,
                         QSqlDatabase database = QSqlDatabase::database());
    static void bindData(const QString &columnName, const QString &orderSql,
                         const QString &tableName, QList<QComboBox *> cboxs,
                         QSqlDatabase database = QSqlDatabase::database());
};

#endif // DBHELPER_H
