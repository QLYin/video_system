#ifndef USERHELPER_H
#define USERHELPER_H

#include "head.h"

class UserHelper
{
public:
    //当前用户及权限
    static QString CurrentUserName;     //当前用户名
    static QString CurrentUserPwd;      //当前用户密码
    static QString CurrentUserType;     //当前用户类型(值班员/管理员)
    static QList<bool> UserPermission;  //当前用户权限集合
    static QStringList PermissionName;  //权限模块名称集合

    //用户信息链表
    static int UserInfo_Count;
    static QList<QString> UserInfo_UserName;
    static QList<QString> UserInfo_UserPwd;
    static QList<QString> UserInfo_UserType;
    static QList<QString> UserInfo_Permission1;
    static QList<QString> UserInfo_Permission2;
    static QList<QString> UserInfo_Permission3;
    static QList<QString> UserInfo_Permission4;
    static QList<QString> UserInfo_Permission5;
    static QList<QString> UserInfo_Permission6;
    static QList<QString> UserInfo_Permission7;

    //载入用户信息
    static void loadUserInfo();
    //清空用户信息
    static void clearUserInfo();
    //更新密码
    static void updateUserInfo(const QString &userName, const QString &userPwd);
    //取出用户的密码及类型和权限
    static void getUserInfo();
    static void getUserInfo(const QString &userName,  QString &userPwd, QString &userType, QList<bool> &permission);

    //校验权限
    static bool checkPermission(const QString &text);
};

#endif // USERHELPER_H
