#include "appmisc.h"
#include "frmmain.h"
#include "frmlogin.h"

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