#include "frmconfigdb.h"
#include "ui_frmconfigdb.h"
#include "qthelper.h"
#include "dbconnthread.h"

void frmConfigDb::initDbInfo(DbInfo &dbInfo, const QString &connName)
{
    dbInfo.connMode = AppConfig::LocalConnMode;
    dbInfo.connName = connName;
    dbInfo.dbName = AppConfig::LocalDbName;
    dbInfo.hostName = AppConfig::LocalHostName;
    dbInfo.hostPort = AppConfig::LocalHostPort;
    dbInfo.userName = AppConfig::LocalUserName;
    dbInfo.userPwd = AppConfig::LocalUserPwd;
}

frmConfigDb::frmConfigDb(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigDb)
{
    ui->setupUi(this);
    this->initForm();
    this->initButton();
    this->initConfig();
}

frmConfigDb::~frmConfigDb()
{
    delete ui;
}

void frmConfigDb::setBtnInRight(bool btnInRight)
{
    this->btnInRight = btnInRight;
    this->initButton();
}

void frmConfigDb::setConnName(const QString &connName)
{
    this->connName = connName;
}

void frmConfigDb::setConnFlag(const QString &connFlag)
{
    this->connFlag = connFlag;
}

void frmConfigDb::initForm()
{
    btnInRight = false;
    connName = "testdb";
    connFlag = QtHelper::appName();
}

void frmConfigDb::initButton()
{
    //将按钮移动到右侧
    if (btnInRight) {
        ui->btnConnect->setMinimumWidth(110);
        ui->btnConnect->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        ui->btnInit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(ui->btnConnect);
        layout->addWidget(ui->btnInit);
        ui->gridLayout->addLayout(layout, 0, 4, 3, 3);
    }
}

void frmConfigDb::initConfig()
{
    ui->cboxConnMode->addItems(DbHelper::getConnMode());
    ui->cboxConnMode->setCurrentIndex(AppConfig::LocalConnMode);
    connect(ui->cboxConnMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxDbType->addItems(DbHelper::getDbType());
    ui->cboxDbType->setCurrentIndex(ui->cboxDbType->findText(AppConfig::LocalDbType));
    connect(ui->cboxDbType, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtDbName->setText(AppConfig::LocalDbName);
    connect(ui->txtDbName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QString hostInfo = QString("%1:%2").arg(AppConfig::LocalHostName).arg(AppConfig::LocalHostPort);
    ui->txtHostInfo->setText(hostInfo);
    connect(ui->txtHostInfo, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserName->setText(AppConfig::LocalUserName);
    connect(ui->txtUserName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtUserPwd->setText(AppConfig::LocalUserPwd);
    connect(ui->txtUserPwd, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));
}

void frmConfigDb::saveConfig()
{
    QStringList hostInfo = ui->txtHostInfo->text().split(":");
    if (hostInfo.count() != 2) {
        return;
    }

    AppConfig::LocalConnMode = ui->cboxConnMode->currentIndex();
    AppConfig::LocalDbType = ui->cboxDbType->currentText();
    AppConfig::LocalDbName = ui->txtDbName->text();
    AppConfig::LocalHostName = hostInfo.at(0);
    AppConfig::LocalHostPort = hostInfo.at(1).toInt();
    AppConfig::LocalUserName = ui->txtUserName->text();
    AppConfig::LocalUserPwd = ui->txtUserPwd->text();
    AppConfig::writeConfig();
}

void frmConfigDb::on_btnConnect_clicked()
{
    {
        //初始化数据库连接信息结构体数据
        DbInfo dbInfo;
        initDbInfo(dbInfo, connName);

        QString dbType = AppConfig::LocalDbType.toUpper();
        if (dbType == "SQLITE") {
            dbInfo.dbName = DbHelper::getDbDefaultFile(connFlag);
            if (QFile(dbInfo.dbName).size() <= 4) {
                QtHelper::showMessageBoxError("数据库文件不存在!", 5);
                return;
            }
        }

        QSqlDatabase database;
        if (DbHelper::initDatabase(true, dbType, database, dbInfo)) {
            if (database.open()) {
                database.close();
                QtHelper::showMessageBoxInfo("打开数据库成功!", 3);
            } else {
                QString error = database.lastError().text();
                QtHelper::showMessageBoxError("打开数据库失败!\n" + error, 3);
            }
        } else {
            QString error = database.lastError().text();
            QtHelper::showMessageBoxError("连接数据库失败!\n" + error, 3);
        }
    }

    QSqlDatabase::removeDatabase(connName);
}

void frmConfigDb::on_btnInit_clicked()
{
    if (QtHelper::showMessageBoxQuestion("确定要初始化数据库吗? 会全部清空数据并且不可还原!") != QMessageBox::Yes) {
        return;
    }

    QString sqlName = QString("%1/db/%2.sql").arg(QtHelper::appPath()).arg(connFlag);
    QFile file(sqlName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QtHelper::showMessageBoxError("数据库脚本文件打开失败!", 3);
        return;
    }

    QElapsedTimer time;
    time.start();
    {
        //初始化数据库连接信息结构体数据
        DbInfo dbInfo;
        initDbInfo(dbInfo, connName);

        QString dbType = AppConfig::LocalDbType.toUpper();
        if (dbType == "SQLITE") {
            dbInfo.dbName = DbHelper::getDbDefaultFile(connFlag);
            //如果文件存在则先删除原来的数据库文件,貌似win上不行
            QFile file(dbInfo.dbName);
            if (file.exists()) {
                bool ok = file.remove();
                if (!ok) {
                    qDebug() << TIMEMS << "remove error" << dbInfo.dbName;
                }

                //清空所有表
                QStringList tables = QSqlDatabase::database().tables();
                foreach (QString table, tables) {
                    DbHelper::clearTable(table, dbType);
                    qDebug() << TIMEMS << "clearTable" << table;
                }

                //关闭默认数据库连接
                QSqlDatabase::database().close();
            }
        }

        //初始化数据库连接并打开数据库
        QSqlDatabase database;
        if (!DbHelper::initDatabase(true, dbType, database, dbInfo)) {
            QString error = database.lastError().text();
            QtHelper::showMessageBoxError("连接数据库失败!\n" + error, 3);
            return;
        }
        if (!database.open()) {
            QString error = database.lastError().text();
            QtHelper::showMessageBoxError("打开数据库失败!\n" + error, 3);
            return;
        }

        QSqlQuery query(QSqlDatabase::database(connName));

        //第一步:删除原有数据库
        QString sql = QString("DROP DATABASE %1;").arg(dbInfo.dbName);
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query.exec(sql);

        //第二步:新建数据库
        sql = QString("CREATE DATABASE %1;").arg(dbInfo.dbName);
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query.exec(sql);

        //第三步:切换到新建的数据库库并执行建表语句
        database.close();
        if (!DbHelper::initDatabase(false, dbType, database, dbInfo)) {
            QString error = database.lastError().text();
            QtHelper::showMessageBoxError("连接数据库失败!\n" + error, 3);
            return;
        }
        if (!database.open()) {
            QString error = database.lastError().text();
            QtHelper::showMessageBoxError("打开数据库失败!\n" + error, 3);
            return;
        }

        //将执行出错的sql语句输出到文件方便查看
        QString fileName2 = QString("%1/db/error.sql").arg(QtHelper::appPath());
        QFile file2(fileName2);

        QSqlQuery query2(QSqlDatabase::database(connName));

        sql = "BEGIN;";
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query2.exec(sql);

        while (!file.atEnd()) {
            sql = QString::fromUtf8(file.readLine());
            sql.replace("\n", "");

            //有些数据库不支持的语句跳过去
            if (DbHelper::existNoSupportSql(sql)) {
                continue;
            }

            //重新纠正sql语句
            DbHelper::checkSql(dbType, sql);

            if (!query2.exec(sql)) {
                //打印及输出错误信息
                QString error = query2.lastError().text();
                qDebug() << TIMEMS << "sql:" << sql << error;

                //没打开则先打开
                if (!file2.isOpen()) {
                    file2.open(QFile::WriteOnly | QFile::Append);
                }

                QString msg = QString("时间[%1]  语句: %2  错误: %3\n").arg(DATETIME).arg(sql).arg(error);
                file2.write(msg.toUtf8());
            }
        }

        sql = "COMMIT;";
        qDebug() << TIMEMS << "sql:" << sql << "result:" << query2.exec(sql);
        database.close();

        //sqlite数据库的话再执行下压缩减少体积
        if (dbType == "SQLITE") {
            DbHelper::execSql("VACUUM;");
        }
    }

    QSqlDatabase::removeDatabase(connName);
    double ms = time.elapsed();
    QString info = QString("数据库脚本执行成功, 总共用时 %1 秒!\n记得重新启动程序!").arg(QString::number(ms / 1000, 'f', 1));
    QtHelper::showMessageBoxInfo(info, 3);
}

void frmConfigDb::on_cboxDbType_activated(int)
{
    //自动切换默认端口号和其他信息
    QString hostPort, userName, userPwd;
    QString dbType = ui->cboxDbType->currentText().toUpper();
    DbHelper::getDbDefaultInfo(dbType, hostPort, userName, userPwd);
    if (!hostPort.isEmpty() && AppConfig::LocalAutoInfo) {
        QStringList hostInfo = ui->txtHostInfo->text().split(":");
        ui->txtHostInfo->setText(hostInfo.at(0) + ":" + hostPort);
        ui->txtUserName->setText(userName);
        ui->txtUserPwd->setText(userPwd);
    }
}
