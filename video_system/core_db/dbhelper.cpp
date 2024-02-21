#include "dbhelper.h"
#include "qtcpsocket.h"
#include "qregexp.h"

void DbHelper::debugDbInfo()
{
    static bool isDebug = false;
    if (!isDebug) {
        isDebug = true;
        //打印支持的数据库,可以自行增加其他打印信息
        qDebug() << TIMEMS << "QSqlDatabase drivers" << QSqlDatabase::database().drivers();
    }
}

QStringList DbHelper::getConnMode()
{
    static QStringList list;
    if (list.count() == 0) {
        list << "直连数据库" << "ODBC数据源";
    }

    return list;
}

QStringList DbHelper::getDbType()
{
    static QStringList list;
    if (list.count() == 0) {
        list << "Sqlite" << "MySql" << "PostgreSQL" << "SqlServer" << "Oracle" << "Access" << "KingBase" << "Other";
    }

    return list;
}

QString DbHelper::getDbType(const DbType &dbType)
{
    //强制转成大写
    QString type = "OTHER";
    if (dbType == DbType_Sqlite) {
        type = "SQLITE";
    } else if (dbType == DbType_MySql) {
        type = "MYSQL";
    } else if (dbType == DbType_PostgreSQL) {
        type = "POSTGRESQL";
    } else if (dbType == DbType_SqlServer) {
        type = "SQLSERVER";
    } else if (dbType == DbType_Oracle) {
        type = "ORACLE";
    } else if (dbType == DbType_Access) {
        type = "ACCESS";
    } else if (dbType == DbType_KingBase) {
        type = "KINGBASE";
    }

    return type;
}

DbType DbHelper::getDbType(const QString &dbType)
{
    //强制转成大写
    QString flag = dbType.toUpper();
    DbType type = DbType_Other;
    if (flag == "SQLITE") {
        type = DbType_Sqlite;
    } else if (flag == "MYSQL") {
        type = DbType_MySql;
    } else if (flag == "POSTGRESQL") {
        type = DbType_PostgreSQL;
    } else if (flag == "SQLSERVER") {
        type = DbType_SqlServer;
    } else if (flag == "ORACLE") {
        type = DbType_Oracle;
    } else if (flag == "ACCESS") {
        type = DbType_Access;
    } else if (flag == "KINGBASE") {
        type = DbType_KingBase;
    }

    return type;
}

void DbHelper::getDbDefaultInfo(const QString &dbType, QString &hostPort,
                                QString &userName, QString &userPwd)
{
    //强制转成大写
    QString flag = dbType.toUpper();
    if (flag == "MYSQL") {
        hostPort = "3306";
        userName = "root";
        userPwd = "root";
    } else if (flag == "POSTGRESQL") {
        hostPort = "5432";
        userName = "postgres";
        userPwd = "123456";
    } else if (flag == "SQLSERVER") {
        hostPort = "1433";
        userName = "sa";
        userPwd = "123456";
    } else if (flag == "ORACLE") {
        hostPort = "1521";
        userName = "system";
        userPwd = "123456";
    } else if (flag == "ACCESS") {
        hostPort = "0";
        userName = "admin";
        userPwd = "123456";
    } else if (flag == "KINGBASE") {
        hostPort = "54321";
        userName = "SYSTEM";
        userPwd = "123456";
    }
}

void DbHelper::bindTable(const QString &dbType, QSqlTableModel *model, const QString &table)
{
    //postgresql全部小写,oracle全部大写,这两个数据库严格区分表名字段名的大小写卧槽
    QString flag = dbType.toUpper();
    if (flag == "POSTGRESQL" || flag == "KINGBASE") {
        model->setTable(table.toLower());
    } else if (flag == "ORACLE") {
        model->setTable(table.toUpper());
    } else {
        model->setTable(table);
    }
}

QString DbHelper::getSelect1Sql(const DbType &dbType)
{
    QString sql;
    if (dbType == DbType_Oracle) {
        sql = "select 1 from dual";
    } else {
        sql = "select 1";
    }

    return sql;
}

QString DbHelper::getSelectCountSql(const DbType &dbType, const QString &table,
                                    const QString &column, const QString &where,
                                    const QString &order, int count)
{
    QString whereSql = "where 1=1";
    if (!where.isEmpty()) {
        whereSql = whereSql + " and " + where;
    }

    QString orderSql = "";
    if (!order.isEmpty()) {
        orderSql = " order by " + order;
    }

    QString sql;
    if (dbType == DbType_SqlServer || dbType == DbType_Access) {
        sql = QString("select top %5 %1 from %2 %3%4").arg(column).arg(table).arg(whereSql).arg(orderSql).arg(count);
    } else if (dbType == DbType_Oracle) {
        sql = QString("select %1 from (select %1 from %2 %3%4) where rownum <= %5 order by rownum asc").arg(column).arg(table).arg(whereSql).arg(orderSql).arg(count);
    } else {
        sql = QString("select %1 from %2 %3%4 limit %5").arg(column).arg(table).arg(whereSql).arg(orderSql).arg(count);
    }

    return sql;
}

QString DbHelper::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
        name.replace("_armeabi-v7a", "");
    }

    return name;
}

QString DbHelper::appPath()
{
#ifdef Q_OS_ANDROID
    //return QString("/sdcard/Android/%1").arg(appName());
    return QString("/storage/emulated/0/%1").arg(appName());
#else
    return qApp->applicationDirPath();
#endif
}

QString DbHelper::getDbDefaultFile(const QString &name)
{
    QString dbName;
#ifdef Q_OS_WASM
    dbName = "/" + name + ".db";
#else
    dbName = name.isEmpty() ? appName() : name;
    dbName = QString("%1/db/%2.db").arg(appPath()).arg(dbName);
#endif
    return dbName;
}

bool DbHelper::existNoSupportSql(const QString &sql)
{
    if (sql.length() < 5 ||
        sql.startsWith("--") ||
        sql.startsWith("PRAGMA") ||
        sql.startsWith("BEGIN TRANSACTION") ||
        sql.startsWith("COMMIT TRANSACTION") ||
        sql.startsWith("DROP INDEX IF EXISTS")) {
        return true;
    }
    return false;
}

void DbHelper::checkSql(const QString &dbType, QString &sql)
{
    //表字段类型替换
    replaceColumnType(sql, "INTEGER");
    //表字段关键字替换,和对应数据库关键字或者函数冲突
    if (dbType != "MYSQL") {
        replaceColumnName(sql, "current");
        replaceColumnName(sql, "plan");
        //replaceColumnName(sql, "year");
        //replaceColumnName(sql, "month");
    }

    //替换部分数据类型
    sql.replace("REAL(11,1)", "REAL");
    //替换自增字段 自增字段容易出问题各种数据库处理方式不一致不建议使用
    sql.replace(" AUTOINCREMENT", "");

    if (dbType == "SQLSERVER" || dbType == "ACCESS") {
        //sqlserver2016以前的版本不认识 IF EXISTS 判断
        sql.replace("DROP TABLE IF EXISTS", "DROP TABLE");
    } else if (dbType == "ORACLE") {
        //不认识 IF EXISTS 判断
        sql.replace("DROP TABLE IF EXISTS", "DROP TABLE");
        //不支持末尾的分号
        sql.replace(";", "");
    }
}

void DbHelper::replaceColumnName(QString &sql, const QString &key)
{
    //不存在关键字不用处理
    if (!sql.contains(key)) {
        return;
    }

    //按照分隔符取出每个字段
    QStringList list = sql.split(",");
    QStringList temp;
    foreach (QString str, list) {
        //下划线的表示是一个其他字段
        if (str.contains(QString("%1_").arg(key)) || str.contains(QString("_%1").arg(key))) {
            temp << str;
            continue;
        }

        //加上双引号才能和系统关键字区分
        if (str.contains(key)) {
            str.replace(key, QString("\"%1\"").arg(key));
        }

        temp << str;
    }

    sql = temp.join(",");
}

void DbHelper::replaceColumnType(QString &sql, const QString &key)
{
    if (sql.contains(key)) {
        //先去掉字段类型和长度之间的空格
        sql.replace("INTEGER (", "INTEGER(");
#if 1
        QRegExp regExp(QString("%1\\(\\d+\\)").arg(key));
        int start = regExp.indexIn(sql);
        int length = regExp.matchedLength();
        if (length > 0) {
            QString result = sql.mid(start, length);
            sql.replace(result, key);
        }
#elif 1
        sql.replace(QRegExp(QString("%1\\(\\d+\\)").arg(key)), key);
#else
        QStringList listAfter;
        QStringList listBefore = sql.split(",");
        foreach (QString str, listBefore) {
            if (str.contains(key)) {
                for (int i = 1; i < 20; ++i) {
                    QString flag = QString("%1(%2)").arg(key).arg(i);
                    str.replace(flag, key);
                }
            }
            listAfter << str;
        }
        sql = listAfter.join(",");
#endif
    }
}

bool DbHelper::hostLive(const QString &hostName, int port, int timeout)
{
    if (timeout <= 0) {
        return true;
    }

    //局部的事件循环,不卡主界面
    QEventLoop eventLoop;

    //设置超时
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.setSingleShot(true);
    timer.start(timeout);

    QTcpSocket tcpSocket;
    QObject::connect(&tcpSocket, SIGNAL(connected()), &eventLoop, SLOT(quit()));
    tcpSocket.connectToHost(hostName, port);
    eventLoop.exec();
    bool ok = (tcpSocket.state() == QAbstractSocket::ConnectedState);
    return ok;
}

bool DbHelper::checkDatabase(const QString &dbType, const DbInfo &dbInfo)
{
    if (dbType == "SQLITE") {
        //判断数据库文件是否存在,不存在则尝试从资源文件复制出来
        QString sourceFile = QString(":/%1.db").arg(appName());
        QString targetFile = dbInfo.dbName;
        QFile file(dbInfo.dbName);
        if (!file.exists() || file.size() == 0) {
            file.remove();
            file.copy(sourceFile, targetFile);
            //将复制过去的文件只读属性取消
            file.setPermissions(targetFile, QFile::WriteOwner);
        }

        //执行完成以后不管成功与否再判断下文件大小
        if (QFile(dbInfo.dbName).size() <= 4) {
            //这里不用返回假因为就算文件不存在也可以通过执行sql脚本生成数据库文件
        }
    } else if (dbType == "ACCESS") {
        QFile file(dbInfo.dbName);
        if (!file.exists() || file.size() == 0) {
            return false;
        }
    } else if (dbInfo.connMode != 1) {
        //先判断数据库服务器IP地址是否存在,否则会卡很久
        if (!hostLive(dbInfo.hostName, dbInfo.hostPort, dbInfo.timeout)) {
            qDebug() << TIMEMS << "服务器连接失败, 请检查地址和端口...";
            return false;
        }
    }
    return true;
}

bool DbHelper::initDatabase(bool testConnect, const QString &dbType,
                            QSqlDatabase &database, const DbInfo &dbInfo)
{
    //先检查数据库连接对象前置条件
    if (!checkDatabase(dbType, dbInfo)) {
        return false;
    }

    //不同数据库连接字符串不一样
    QString connName = dbInfo.connName;
    if (dbInfo.connMode == 0) {
        if (dbType == "SQLITE") {
            database = QSqlDatabase::addDatabase("QSQLITE", connName);
        } else if (dbType == "MYSQL") {
            database = QSqlDatabase::addDatabase("QMYSQL", connName);
            int timeout = 0;
            if (dbInfo.timeout >= 1000) {
                timeout = dbInfo.timeout / 1000;
                QStringList list;
                list << QString("MYSQL_OPT_RECONNECT=%1").arg(timeout);
                list << QString("MYSQL_OPT_CONNECT_TIMEOUT=%1").arg(timeout);
                list << QString("MYSQL_OPT_READ_TIMEOUT=%1").arg(timeout);
                list << QString("MYSQL_OPT_WRITE_TIMEOUT=%1").arg(timeout);
                database.setConnectOptions(QString("%1").arg(list.join(";")));
            }
        } else if (dbType == "SQLSERVER") {
            database = QSqlDatabase::addDatabase("QODBC", connName);
        } else if (dbType == "POSTGRESQL") {
            database = QSqlDatabase::addDatabase("QPSQL", connName);
        } else if (dbType == "ORACLE") {
            database = QSqlDatabase::addDatabase("QOCI", connName);
        } else if (dbType == "ACCESS") {
            database = QSqlDatabase::addDatabase("QODBC", connName);
        } else if (dbType == "KINGBASE") {
            //其实kingbase就是postgresql改的 http://t.zoukankan.com/xianghuaqiang-p-13433621.html
            database = QSqlDatabase::addDatabase("QPSQL", connName);
        }

        //如果是用来测试连接则对应数据库为空
        //在测试阶段可能真正的数据库还不存在,需要测试完执行新建数据库
        if (dbType == "SQLSERVER") {
            //下面这种方式不需要设置数据源
            QStringList list;
            list << QString("DRIVER={%1}").arg("SQL SERVER");
            list << QString("SERVER=%1,%2").arg(dbInfo.hostName).arg(dbInfo.hostPort);
            if (!testConnect) {
                list << QString("DATABASE=%1").arg(dbInfo.dbName);
                list << QString("UID=%1").arg(dbInfo.userName);
                list << QString("PWD=%1").arg(dbInfo.userPwd);
            }
            database.setDatabaseName(list.join(";"));
        } else if (dbType == "ORACLE") {
            //貌似只能连接全局数据库名称 orcl
            database.setDatabaseName("orcl");
            //database.setDatabaseName(dbInfo.dbName);

            //oracle12开始连接方式改了
            //QString info = QString("%1:%2/%3").arg(dbInfo.hostName).arg(dbInfo.hostPort).arg(dbInfo.dbName);
            //database.setDatabaseName(info);
        } else if (dbType == "ACCESS") {
            QStringList list;
            list << QString("DRIVER={%1}").arg("Microsoft Access Driver (*.mdb, *.accdb)");
            list << QString("FIL={%1}").arg("MS Access");
            list << QString("DBQ=%1").arg(dbInfo.dbName);
            list << QString("UID=%1").arg(dbInfo.userName);
            list << QString("PWD=%1").arg(dbInfo.userPwd);
            database.setDatabaseName(list.join(";"));
        } else if (dbType == "KINGBASE") {
            database.setDatabaseName(testConnect ? "test" : dbInfo.dbName);
        } else {
            if (!testConnect) {
                database.setDatabaseName(dbInfo.dbName);
            }
        }
    } else {
        database = QSqlDatabase::addDatabase("QODBC", connName);
        database.setDatabaseName(dbInfo.dbName);
    }

    //设置主机地址端口和用户信息
    database.setHostName(dbInfo.hostName);
    database.setPort(dbInfo.hostPort);
    database.setUserName(dbInfo.userName);
    database.setPassword(dbInfo.userPwd);
    return true;
}

//查询数据库所有表名
//sqlite        select name from sqlite_master where type = 'table' order by name
//mysql         select table_name from information_schema.tables where table_schema = 'dbtool'
//postgresql    select tablename from pg_tables where schemaname = 'public'
//sqlserver     select name from sys.tables
//oracle        select object_name from user_objects where object_type = 'TABLE' 注意TABLE是大写

//查询数据库表所有字段
//sqlite        pragma table_info([loginfo])
//mysql         select column_name from information_schema.columns where table_name = 'loginfo' and table_schema = 'dbtool'
//postgresql    select a.attname from pg_class as c,pg_attribute as a where	c.relname = 'loginfo' and a.attrelid = c.oid and a.attnum > 0
//sqlserver     select syscolumns.name from syscolumns,systypes where syscolumns.xusertype = systypes.xusertype and syscolumns.id = object_id('loginfo')
//oracle        select column_name from user_tab_columns where table_name = 'LOGINFO' 注意表名是大写

QStringList DbHelper::getTables(const QString &date, const QString &dbType, QSqlDatabase database)
{
    QString flag = dbType.toUpper();
    QStringList tables;
    if (flag == "POSTGRESQL" || flag == "KINGBASE") {
        QString sql = QString("select tablename from pg_tables where schemaname = 'public'");
        QSqlQuery query(database);
        if (query.exec(sql)) {
            while (query.next()) {
                QString table = query.value(0).toString();
                tables << table;
            }
        }
    } else if (flag == "SQLSERVER") {
        QString sql = QString("select name from sys.tables");
        QSqlQuery query(database);
        if (query.exec(sql)) {
            while (query.next()) {
                QString table = query.value(0).toString();
                tables << table;
            }
        }
    } else if (flag == "ORACLE") {
        //oracle数据库是公共的数据库一堆表在里面,默认的表创建时间比较早
        QString sql = QString("select object_name,created from user_objects where object_type = 'TABLE' and created > to_date('%1', 'yyyy-MM-dd')").arg(date);
        QSqlQuery query(database);
        if (query.exec(sql)) {
            while (query.next()) {
                QString table = query.value(0).toString();
                if (!table.contains("$")) {
                    tables << table;
                }
            }
        }
    } else {
        tables = database.tables();
    }

    return tables;
}

bool DbHelper::execSql(const QString &sql, QSqlDatabase database)
{
    QSqlQuery query(database);
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return false;
    }
    return true;
}

void DbHelper::clearTable(const QString &tableName, QSqlDatabase database)
{
    QString sql = QString("delete from %1").arg(tableName);
    execSql(sql, database);
}

void DbHelper::clearTable(const QString &tableName, const QString &dbType, QSqlDatabase database)
{
    QString sql;
    QString flag = dbType.toUpper();
    if (flag == "SQLITE") {
        //SQLite数据库自增列信息存储在 sqlite_sequence 表,表包含两个列：name记录自增列所在的表,seq记录当前序号
        sql = QString("delete from %1").arg(tableName);
        execSql(sql, database);
        sql = QString("update sqlite_sequence set seq=0 where name='%1'").arg(tableName);
        //execSql(sql, database);
    } else {
        //用truncate而不是delete删除表,速度和效率最高,会重置自增列和释放空间
        sql = QString("truncate table %1").arg(tableName);
        execSql(sql, database);
    }
}

int DbHelper::getMaxID(const QString &tableName, const QString &columnName, QSqlDatabase database)
{
    int maxID = 0;
    QSqlQuery query(database);
    QString sql = QString("select max(%1) from %2").arg(columnName).arg(tableName);
    if (query.exec(sql)) {
        if (query.next()) {
            maxID = query.value(0).toInt();
        }
    } else {
        qDebug() << TIMEMS << query.lastError().text() << sql;
    }

    return maxID;
}

void DbHelper::getBetweenDate(QString &sql, const QString &dbType, const QString &columnName, const QString &dateTimeStart, const QString &dateTimeEnd, bool varchar)
{
    if (varchar) {
        //数据库中对应字段类型是字符串,直接比较即可 '2022-06-25 20:33:33'
        sql += QString(" and %1 >= '%2'").arg(columnName).arg(dateTimeStart);
        sql += QString(" and %1 <= '%2'").arg(columnName).arg(dateTimeEnd);
    } else {
        //数据库中对应字段类型是日期时间,需要将传入的日期时间字符串转换后(转换成日期时间或者unix时间戳)再比较
        QString flag = dbType.toUpper();
        if (flag == "SQLITE") {
            //https://blog.csdn.net/horses/article/details/119256750
            sql += QString(" and datetime(%1) >= '%2'").arg(columnName).arg(dateTimeStart);
            sql += QString(" and datetime(%1) <= '%2'").arg(columnName).arg(dateTimeEnd);
        } else if (flag == "MYSQL") {
            //https://blog.csdn.net/weixin_33072655/article/details/113110339
            sql += QString(" and unix_timestamp(%1) >= unix_timestamp('%2')").arg(columnName).arg(dateTimeStart);
            sql += QString(" and unix_timestamp(%1) <= unix_timestamp('%2')").arg(columnName).arg(dateTimeEnd);
        } else if (flag == "POSTGRESQL" || flag == "KINGBASE") {
            //https://blog.csdn.net/sky_limitless/article/details/79527665
            sql += QString(" and to_timestamp(%1) >= to_timestamp('%2')").arg(columnName).arg(dateTimeStart);
            sql += QString(" and to_timestamp(%1) <= to_timestamp('%2')").arg(columnName).arg(dateTimeEnd);
        } else if (flag == "SQLSERVER") {
            //https://blog.csdn.net/qq_37493556/article/details/113205668
            sql += QString(" and %1 >= convert(varchar(100), '%2', 20)").arg(columnName).arg(dateTimeStart);
            sql += QString(" and %1 <= convert(varchar(100), '%2', 20)").arg(columnName).arg(dateTimeEnd);
        } else if (flag == "ORACLE") {
            //https://blog.csdn.net/tiantangdizhibuxiang/article/details/81035305
            sql += QString(" and %1 >= to_date('%2', 'yyyy-MM-dd hh24:mi:ss')").arg(columnName).arg(dateTimeStart);
            sql += QString(" and %1 <= to_date('%2', 'yyyy-MM-dd hh24:mi:ss')").arg(columnName).arg(dateTimeEnd);
        } else {
            sql += QString(" and %1 >= '%2'").arg(columnName).arg(dateTimeStart);
            sql += QString(" and %1 <= '%2'").arg(columnName).arg(dateTimeEnd);
        }
    }
}

void DbHelper::bindData(const QString &columnName, const QString &orderSql,
                        const QString &tableName, QComboBox *cbox, QSqlDatabase database)
{
    QSqlQuery query(database);
    query.setForwardOnly(true);
    QString sql = QString("select %1 from %2 order by %3").arg(columnName).arg(tableName).arg(orderSql);
    if (query.exec(sql)) {
        while (query.next()) {
            QString value = query.value(0).toString();
            cbox->addItem(value);
        }
    } else {
        qDebug() << TIMEMS << query.lastError().text() << sql;
    }
}

void DbHelper::bindData(const QString &columnName, const QString &orderSql,
                        const QString &tableName, QList<QComboBox *> cboxs, QSqlDatabase database)
{
    QSqlQuery query(database);
    query.setForwardOnly(true);
    QString sql = QString("select %1 from %2 order by %3").arg(columnName).arg(tableName).arg(orderSql);
    if (query.exec(sql)) {
        while (query.next()) {
            QString value = query.value(0).toString();
            foreach (QComboBox *cbox, cboxs) {
                cbox->addItem(value);
            }
        }
    } else {
        qDebug() << TIMEMS << query.lastError().text() << sql;
    }
}
