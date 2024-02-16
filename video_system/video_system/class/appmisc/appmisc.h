#ifndef APPMISC_H
#define APPMISC_H

#include "singleton.h"

class frmMain;
class frmLogin;
class frmTVWallWidget;
class AppMisc : public QObject
{
    Q_OBJECT SINGLETON_DECL(AppMisc)

public:
    explicit AppMisc(QObject* parent = 0);
public:
    frmMain* mainWnd();
    frmLogin* loginWnd();
    frmTVWallWidget* tvwallWidget();
    void setTVWallWidget(frmTVWallWidget* widget);

    bool hasMergeScreen();

private:
    frmMain* m_mainWnd = nullptr;
    frmLogin* m_loginWnd = nullptr;
    frmTVWallWidget* m_tvwallWidget = nullptr;
};

#if defined(theMainWnd)
#undef theMainWnd
#endif
#define theMainWnd (AppMisc::Instance()->mainWnd())

#if defined(theLoginWnd)
#undef theLoginWnd
#endif
#define theLoginWnd (AppMisc::instance()->loginWnd())

#endif // APPMISC_H
