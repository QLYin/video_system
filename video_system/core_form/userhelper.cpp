#include "userhelper.h"
#include "qthelper.h"

QString UserHelper::CurrentUserName = "admin";
QString UserHelper::CurrentUserPwd = "admin";
QString UserHelper::CurrentUserType = QString::fromUtf8("管理员");
QList<bool> UserHelper::UserPermission = QList<bool>() << true << true << true << true << true << true << true;
QStringList UserHelper::PermissionName = QStringList() << QString::fromUtf8("系统设置|删除记录|模块A|模块B|模块C|模块D|模块E").split("|");

int UserHelper::UserInfo_Count = 0;
QList<QString> UserHelper::UserInfo_UserName = QList<QString>();
QList<QString> UserHelper::UserInfo_UserPwd = QList<QString>();
QList<QString> UserHelper::UserInfo_UserType = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission1 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission2 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission3 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission4 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission5 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission6 = QList<QString>();
QList<QString> UserHelper::UserInfo_Permission7 = QList<QString>();

void UserHelper::loadUserInfo()
{
    UserHelper::UserInfo_Count = 0;
    UserHelper::UserInfo_UserName.clear();
    UserHelper::UserInfo_UserPwd.clear();
    UserHelper::UserInfo_UserType.clear();
    UserHelper::UserInfo_Permission1.clear();
    UserHelper::UserInfo_Permission2.clear();
    UserHelper::UserInfo_Permission3.clear();
    UserHelper::UserInfo_Permission4.clear();
    UserHelper::UserInfo_Permission5.clear();
    UserHelper::UserInfo_Permission6.clear();
    UserHelper::UserInfo_Permission7.clear();

    QString column = "Permission1,Permission2,Permission3,Permission4,Permission5,Permission6,Permission7";
    QString sql = QString("select UserName,UserPwd,UserType,%1 from UserInfo").arg(column);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    while (query.next()) {
        QString userName = query.value(0).toString();
        QString userPwd = query.value(1).toString();
        QString userType = query.value(2).toString();
        QString permission1 = query.value(3).toString();
        QString permission2 = query.value(4).toString();
        QString permission3 = query.value(5).toString();
        QString permission4 = query.value(6).toString();
        QString permission5 = query.value(7).toString();
        QString permission6 = query.value(8).toString();
        QString permission7 = query.value(9).toString();

        UserHelper::UserInfo_Count++;
        UserHelper::UserInfo_UserName << userName;
        UserHelper::UserInfo_UserPwd << userPwd;
        UserHelper::UserInfo_UserType << userType;
        UserHelper::UserInfo_Permission1 << permission1;
        UserHelper::UserInfo_Permission2 << permission2;
        UserHelper::UserInfo_Permission3 << permission3;
        UserHelper::UserInfo_Permission4 << permission4;
        UserHelper::UserInfo_Permission5 << permission5;
        UserHelper::UserInfo_Permission6 << permission6;
        UserHelper::UserInfo_Permission7 << permission7;
    }
}

void UserHelper::clearUserInfo()
{
    QString sql = "delete from UserInfo where UserName!='admin'";
    DbHelper::execSql(sql);
}

void UserHelper::updateUserInfo(const QString &userName, const QString &userPwd)
{
    QString sql = QString("update UserInfo set UserPwd='%1' where UserName='%2'").arg(userPwd).arg(userName);
    DbHelper::execSql(sql);
}

void UserHelper::getUserInfo()
{
    UserHelper::getUserInfo(UserHelper::CurrentUserName, UserHelper::CurrentUserPwd, UserHelper::CurrentUserType, UserHelper::UserPermission);
}

void UserHelper::getUserInfo(const QString &userName, QString &userPwd, QString &userType, QList<bool> &permission)
{
    QString column = "Permission1,Permission2,Permission3,Permission4,Permission5,Permission6,Permission7";
    QString sql = QString("select UserPwd,UserType,%1 from UserInfo where UserName='%2'").arg(column).arg(userName);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return;
    }

    if (query.next()) {
        userPwd = query.value(0).toString();
        userType = query.value(1).toString();
        for (int i = 0; i < 7; ++i) {
            permission[i] = (query.value(i + 2).toString() == "启用");
        }
    }

    //qDebug() << TIMEMS << UserHelper::CurrentUserName << UserHelper::CurrentUserPwd << UserHelper::CurrentUserType << UserHelper::UserPermission;
}

bool UserHelper::checkPermission(const QString &text)
{
    //从权限模块名称找到当前模块是否需要授权
    //可能对应按钮的文本有空格要去掉再比较 比如权限文本设定的是 用户管理 而实际按钮是 用 户 管 理
    QString flag = text;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    flag.remove(QRegularExpression("\\s"));
#else
    flag.remove(QRegExp("\\s"));
#endif
    int index = UserHelper::PermissionName.indexOf(flag);
    if (index >= 0) {
        if (!UserHelper::UserPermission.at(index)) {
            QtHelper::showMessageBoxError(QString("当前用户没有 [%1] 权限!").arg(text), 3);
            return false;
        }
    }

    return true;
}
