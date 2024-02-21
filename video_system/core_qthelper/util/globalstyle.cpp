#include "globalstyle.h"
#include "qthelper.h"
#include "qthelperform.h"

bool GlobalStyle::isDark1(const QString &styleName)
{
    QStringList listDark;
    listDark << "blackblue" << "blackvideo" << "darkblue" << "darkblack" << "otherblue" << "otherpurple";
    listDark << "newblack" << "newpurple" << "flatblack" << "flatblue";
    bool dark = false;
    foreach (QString list, listDark) {
        if (styleName.contains(list)) {
            dark = true;
            break;
        }
    }
    return dark;
}

bool GlobalStyle::isDark2(const QString &styleName)
{
    QStringList listDark;
    listDark << "blackblue" << "blackvideo" << "darkblue" << "darkblack" << "otherblue" << "otherpurple";
    listDark << "newblack" << "newpurple";
    bool dark = false;
    foreach (QString list, listDark) {
        if (styleName.contains(list)) {
            dark = true;
            break;
        }
    }
    return dark;
}

bool GlobalStyle::isDark3(const QString &styleName)
{
    QStringList listDark;
    listDark << "darkblack" << "lightblue" << "lightgray" << "normalblue" << "darkgray" << "othersilvery";
    bool dark = false;
    foreach (QString list, listDark) {
        if (styleName.contains(list)) {
            dark = true;
            break;
        }
    }
    return dark;
}

void GlobalStyle::getStyle(QStringList &styleNames, QStringList &styleFiles)
{
    static QStringList names;
    if (names.count() == 0) {
        names << "黑蓝色" << "软件黑" << "视频黑";
        names << "深黑色" << "深蓝色" << "深灰色";
        names << "扁平黑" << "扁平蓝" << "扁平灰";
        names << "浅黑色" << "浅蓝色" << "浅灰色";
        names << "普通黑" << "普通蓝" << "普通灰";
        names << "大蓝色" << "大紫色" << "大银色";
        names << "新黑色" << "新紫色";
    }

    //中文皮肤名称对应样式表文件
    static QStringList files;
    if (files.count() == 0) {
        files << ":/qss/blackblue.css" << ":/qss/blacksoft.css" << ":/qss/blackvideo.css";
        files << ":/qss/darkblack.css" << ":/qss/darkblue.css" << ":/qss/darkgray.css";
        files << ":/qss/flatblack.css" << ":/qss/flatblue.css" << ":/qss/flatgray.css";
        files << ":/qss/lightblack.css" << ":/qss/lightblue.css" << ":/qss/lightgray.css";
        files << ":/qss/normalblack.css" << ":/qss/normalblue.css" << ":/qss/normalgray.css";
        files << ":/qss/otherblue.css" << ":/qss/otherpurple.css" << ":/qss/othersilvery.css";
        files << ":/qss/newblack.css" << ":/qss/newpurple.css";
    }

    styleNames = names;
    styleFiles = files;
}

void GlobalStyle::setStyle(const QString &qss)
{
    QStringList list;
    list << qss;

    //组件窗体样式
    list << "#QtSplash>#QtFormMain {";
    list << "  border:0px solid #FF0000;";
    list << "  border-radius:5px;";
    list << "}";

    list << "#QtAbout>#QtFormMain {";
    list << "  border:0px solid #FF0000;";
    list << "  border-radius:8px;";
    list << "  border-image:url(:/image/bg_splash.jpg);";
    list << "}";

    list << "#QtAbout>#QtFormMain>QLabel,#QtAbout>#QtFormMain>QPushButton {";
    list << "  color:#FFFFFF;";
    list << "  border-radius:5px;";
    list << "}";

    //5.12版本后tabbar选项卡左右反过来的
#if (QT_VERSION >= QT_VERSION_CHECK(5,12,0))
    //左右两侧的边框偏移一个像素
    list << "QTabWidget::pane:left{left:-1px;right:0px;}";
    list << "QTabWidget::pane:right{right:-1px;left:0px;}";
    //选中和悬停的时候边缘加深2个像素
    list << "QTabBar::tab:left:selected,QTabBar::tab:left:hover{border-width:0px 0px 0px 2px;}";
    list << "QTabBar::tab:right:selected,QTabBar::tab:right:hover{border-width:0px 2px 0px 0px;}";
#endif

    //5.14版本后菜单图标有位置,有复选框的菜单节点需要偏移
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    list << "QMenu::item:checked{padding:3px 20px 3px 0px;}";
    list << "QMenu::item:unchecked{padding:3px 20px 3px 0px;}";
#endif

    //5.2开始颜色支持透明度
    QString txtReadOnlyColor = GlobalConfig::NormalColorStart.right(6);
#if (QT_VERSION >= QT_VERSION_CHECK(5,2,0))
    txtReadOnlyColor = "88" + txtReadOnlyColor;
#endif
    //增加文本框只读背景颜色
    list << QString("QLineEdit:read-only{background-color:#%1;}").arg(txtReadOnlyColor);

    //修复MDI窗体在应用了样式以后无法拉伸边框的BUG
    list << "QMdiSubWindow:disabled{border:8px solid rgba(0,0,0,0);}";
    //设置MDI区域背景颜色透明
    list << "QMdiArea{qproperty-background:transparent;}";
    //系统对话框尺寸
    //list << "QMessageBox>QLabel{min-width:100px;}";

    QtHelperForm::isCustomUI = true;
    //阴影边框和配色方案自动变化
    QtHelper::setFormShadow(GlobalConfig::HighColor);

    QString paletteColor = qss.mid(20, 7);
    qApp->setPalette(QPalette(paletteColor));
    qApp->setStyleSheet(list.join(""));
}

void GlobalStyle::setStyle(const GlobalStyle::Style &style)
{
    //取出所有的皮肤名称和对应样式文件
    QStringList styleNames, styleFiles;
    getStyle(styleNames, styleFiles);

    //取出对应索引的样式文件
    QString qssFile = (styleFiles.at((int)style));

    //设置全局样式
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        getQssColor(qss, GlobalConfig::TextColor, GlobalConfig::PanelColor, GlobalConfig::BorderColor, GlobalConfig::NormalColorStart,
                    GlobalConfig::NormalColorEnd, GlobalConfig::DarkColorStart, GlobalConfig::DarkColorEnd, GlobalConfig::HighColor);
        setStyle(qss);
        file.close();
    }
}

void GlobalStyle::setStyleFile(const QString &qssFile)
{
    QString paletteColor, textColor;
    setStyleFile(qssFile, paletteColor, textColor);
}

void GlobalStyle::setStyleFile(const QString &qssFile, QString &paletteColor, QString &textColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        paletteColor = qss.mid(20, 7);
        textColor = qss.mid(49, 7);
        getQssColor(qss, GlobalConfig::TextColor, GlobalConfig::PanelColor, GlobalConfig::BorderColor, GlobalConfig::NormalColorStart,
                    GlobalConfig::NormalColorEnd, GlobalConfig::DarkColorStart, GlobalConfig::DarkColorEnd, GlobalConfig::HighColor);
        setStyle(qss);
        file.close();
    }
}

void GlobalStyle::setStyleFile(const QString &qssFile, QString &textColor, QString &panelColor, QString &borderColor,
                            QString &normalColorStart, QString &normalColorEnd,
                            QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        getQssColor(qss, textColor, panelColor, borderColor, normalColorStart, normalColorEnd, darkColorStart, darkColorEnd, highColor);
        setStyle(qss);
        file.close();
    }
}

void GlobalStyle::getQssColor(const QString &qss, const QString &flag, QString &color)
{
    int index = qss.indexOf(flag);
    if (index >= 0) {
        color = qss.mid(index + flag.length(), 7);
    }
    //qDebug() << TIMEMS << flag << color;
}

void GlobalStyle::getQssColor(const QString &qss, QString &textColor,
                           QString &panelColor, QString &borderColor,
                           QString &normalColorStart, QString &normalColorEnd,
                           QString &darkColorStart, QString &darkColorEnd, QString &highColor)
{
    getQssColor(qss, "TextColor:", textColor);
    getQssColor(qss, "PanelColor:", panelColor);
    getQssColor(qss, "BorderColor:", borderColor);
    getQssColor(qss, "NormalColorStart:", normalColorStart);
    getQssColor(qss, "NormalColorEnd:", normalColorEnd);
    getQssColor(qss, "DarkColorStart:", darkColorStart);
    getQssColor(qss, "DarkColorEnd:", darkColorEnd);
    getQssColor(qss, "HighColor:", highColor);

    GlobalConfig::TextColor = textColor;
    GlobalConfig::PanelColor = panelColor;
    GlobalConfig::BorderColor = borderColor;
    GlobalConfig::NormalColorStart = normalColorStart;
    GlobalConfig::NormalColorEnd = normalColorEnd;
    GlobalConfig::DarkColorStart = darkColorStart;
    GlobalConfig::DarkColorEnd = darkColorEnd;
    GlobalConfig::HighColor = highColor;

    GlobalConfig::HoverBgColor = darkColorEnd;
    GlobalConfig::SelectBgColor = normalColorEnd;
}

void GlobalStyle::setLabStyle(QLabel *lab, quint8 type, const QString &bgColor, const QString &textColor)
{
    QString colorBg = bgColor;
    QString colorText = textColor;

    //如果设置了新颜色则启用新颜色
    if (bgColor.isEmpty() && textColor.isEmpty()) {
        if (type == 0) {
            colorBg = "#D64D54";
            colorText = "#FFFFFF";
        } else if (type == 1) {
            colorBg = "#17A086";
            colorText = "#FFFFFF";
        } else if (type == 2) {
            colorBg = "#47A4E9";
            colorText = "#FFFFFF";
        } else if (type == 3) {
            colorBg = "#282D30";
            colorText = "#FFFFFF";
        } else if (type == 4) {
            colorBg = "#0E99A0";
            colorText = "#FFFFFF";
        } else if (type == 5) {
            colorBg = "#A279C5";
            colorText = "#FFFFFF";
        } else if (type == 6) {
            colorBg = "#8C2957";
            colorText = "#FFFFFF";
        } else if (type == 7) {
            colorBg = "#04567E";
            colorText = "#FFFFFF";
        } else if (type == 8) {
            colorBg = "#FD8B28";
            colorText = "#FFFFFF";
        } else if (type == 9) {
            colorBg = "#5580A2";
            colorText = "#FFFFFF";
        }
    }

    QStringList qss;
    //禁用颜色
    qss << QString("QLabel::disabled{background:none;color:%1;}").arg(GlobalConfig::BorderColor);
    //正常颜色
    qss << QString("QLabel{border:none;background-color:%1;color:%2;}").arg(colorBg).arg(colorText);
    lab->setStyleSheet(qss.join(""));
}

