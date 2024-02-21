#ifndef DBHEAD_H
#define DBHEAD_H

#include <QtGui>
#include <QtSql>
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#pragma execution_character_set("utf-8")

#ifndef TIMEMS
#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#endif
#ifndef DATETIME
#define DATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#endif

//数据库类型枚举
enum DbType {
    DbType_Sqlite = 1,      //sqlite数据库
    DbType_MySql = 2,       //mysql数据库
    DbType_PostgreSQL = 3,  //postgresql数据库
    DbType_SqlServer = 4,   //sqlserver数据库
    DbType_Oracle = 5,      //oracle数据库
    DbType_Access = 6,      //access数据库
    DbType_KingBase = 10,   //国产人大金仓数据库
    DbType_Other = 255      //其他数据库
};

//数据库连接信息结构体
struct DbInfo {
    int connMode;           //连接方式 0-直连数据库 1-ODBC数据源
    int timeout;            //超时时间 0-不处理
    QString connName;       //连接名称
    QString dbName;         //数据库名

    QString hostName;       //主机地址
    int hostPort;           //通信端口
    QString userName;       //用户名称
    QString userPwd;        //用户密码

    //默认构造函数
    DbInfo() {
        connMode = 0;
        timeout = 0;
        connName = "qt_sql_default_connection";
        dbName = "test";

        hostName = "127.0.0.1";
        hostPort = 3306;
        userName = "root";
        userPwd = "root";
    }

    //重载打印输出格式
    friend QDebug operator << (QDebug debug, const DbInfo &dbInfo) {
        QStringList list;
        list << QString("连接方式: %1").arg(dbInfo.connMode == 0 ? "直连数据库" : "ODBC数据源");
        list << QString("超时时间: %1").arg(dbInfo.timeout);
        list << QString("连接名称: %1").arg(dbInfo.connName);
        list << QString("数据库名: %1").arg(dbInfo.dbName);

        list << QString("主机地址: %1").arg(dbInfo.hostName);
        list << QString("通信端口: %1").arg(dbInfo.hostPort);
        list << QString("用户名称: %1").arg(dbInfo.userName);
        list << QString("用户密码: %1").arg(dbInfo.userPwd);

#if (QT_VERSION >= QT_VERSION_CHECK(5,4,0))
        debug.noquote() << list.join("\n");
#else
        debug << list.join("\n");
#endif
        return debug;
    }
};

#endif // DBHEAD_H
