#ifndef FRMCONFIGDB_H
#define FRMCONFIGDB_H

#include <QWidget>
#include "dbhead.h"

namespace Ui {
class frmConfigDb;
}

class frmConfigDb : public QWidget
{
    Q_OBJECT

public:
    static void initDbInfo(DbInfo &dbInfo, const QString &connName);
    explicit frmConfigDb(QWidget *parent = 0);
    ~frmConfigDb();

private:
    Ui::frmConfigDb *ui;

    //按钮在右侧默认假
    bool btnInRight;
    //连接名称默认testdb
    QString connName;
    //连接标识默认为数据库名称
    QString connFlag;

public:
    //设置按钮在右侧
    void setBtnInRight(bool btnInRight);
    //设置连接名称
    void setConnName(const QString &connName);
    //设置连接标识
    void setConnFlag(const QString &connFlag);

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮排列
    void initButton();

    //加载数据库配置
    void initConfig();
    //保存数据库配置
    void saveConfig();

private slots:    
    void on_btnConnect_clicked();
    void on_btnInit_clicked();
    void on_cboxDbType_activated(int);
};

#endif // FRMCONFIGDB_H
