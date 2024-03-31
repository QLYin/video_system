#include "appmisc.h"
#include "frmmain.h"
#include "frmlogin.h"
#include "ui/frmtvwall/frmtvwallwidget.h"

SINGLETON_IMPL(AppMisc)
AppMisc::AppMisc(QObject *parent) : QObject(parent)
{
	m_mainWnd = new frmMain;
	m_loginWnd = new frmLogin;

}

frmMain* AppMisc::mainWnd()
{
	return m_mainWnd;
}

frmLogin* AppMisc::loginWnd()
{
	return m_loginWnd;
}

frmTVWallWidget* AppMisc::tvwallWidget()
{
	return m_tvwallWidget;
}

void AppMisc::setTVWallWidget(frmTVWallWidget* widget)
{
	m_tvwallWidget = widget;
}

bool AppMisc::hasMergeScreen()
{
	if (m_tvwallWidget)
	{
		return m_tvwallWidget->hasMergeScreen();
	}
	return false;
}

QVariant AppMisc::get(const QString& key)
{
	return m_appmisMap[key];
}

void AppMisc::set(const QString& key, QVariant value)
{
	m_appmisMap[key] = value;
}