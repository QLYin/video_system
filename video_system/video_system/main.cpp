#include "frmdemo.h"
#include "frmmain.h"
#include "frmlogin.h"
#include "class/appmisc/appmisc.h"

#include "qthelper.h"
#include "dbquery.h"
#include "appinit.h"

QString appDataFolder(void)
{
    static QString s_appFolder;

    if (s_appFolder.isEmpty())
    {
        QString appName("video_system");
        if (appName.isEmpty())
        {
            appName = qApp->applicationName();
        }

        QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        dir.cdUp();

        s_appFolder = dir.absolutePath() + QDir::separator() + appName;
        dir.mkpath(s_appFolder);
    }

    return s_appFolder;
}

QString initLoggerFile()
{
    static QString s_loggerFile;
    if (s_loggerFile.isEmpty())
    {
        // 获取当前日期和时间
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
        // 构建日志文件名
        QString logFileName = QString("log_%1.txt").arg(timestamp);

        // 定义日志文件路径
        QString logFileDir = appDataFolder() + QDir::separator() + QStringLiteral("logs");
        QDir directory(logFileDir);
        if (!directory.exists()) {
            directory.mkpath(logFileDir);
        }
        s_loggerFile = logFileDir + QDir::separator() + logFileName;
    }

    return s_loggerFile;
}

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // 打开日志文件
    QFile logFile(initLoggerFile());
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        // 创建文本流
        QTextStream textStream(&logFile);

        // 获取当前时间
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        // 根据消息类型写入日志文件
        switch (type) {
        case QtDebugMsg:
            textStream << "[" << timestamp << "] [Debug] " << msg << endl;
            break;
        case QtInfoMsg:
            textStream << "[" << timestamp << "] [Info] " << msg << endl;
            break;
        case QtWarningMsg:
            textStream << "[" << timestamp << "] [Warning] " << msg << endl;
            break;
        case QtCriticalMsg:
            textStream << "[" << timestamp << "] [Critical] " << msg << endl;
            break;
        case QtFatalMsg:
            textStream << "[" << timestamp << "] [Fatal] " << msg << endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int openGLType = QtHelper::getIniValue(argv, "OpenGLType", "config/").toInt();
    QtHelper::initOpenGL(openGLType);

    QtHelper::initMain(false, false);
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/main.ico"));

    if (!a.applicationFilePath().contains("release")) // 外发的理论上不带release目录
    {
        // 安装自定义消息处理程序
        qInstallMessageHandler(customMessageHandler);
    }

    //强制指定启动窗体方便测试
    AppConfig::IndexStart = 0;
    AppInit::Instance()->start();

    //根据不同的启动窗体索引来启动 0-主程序 1-演示示例
    QWidget *w;
    if (AppConfig::IndexStart == 1) {
        w = new frmDemo;
        w->setWindowTitle("监控系统组件示例 (QQ: 517216493  WX: feiyangqingyun)");
    } else {
        //开启了自动登录则直接启动主窗体/否则启动登录界面
        AppConfig::AutoLogin ? (w = AppMisc::Instance()->mainWnd()) : (w = AppMisc::Instance()->loginWnd());
    }

    w->show();
    w->activateWindow();
    return a.exec();
}
