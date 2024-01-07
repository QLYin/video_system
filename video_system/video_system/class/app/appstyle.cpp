#include "appstyle.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "commonstyle.h"

QString AppStyle::styleName = ":/qss/blackvideo.css";
void AppStyle::addCustomStyle(QStringList &list)
{
    //画面分割栏
    list << QString("QWidget#frmVideoPanelTool>QPushButton{background:transparent;border:none;}");
    list << QString("QWidget#frmVideoPanelTool>QPushButton{color:%1;}").arg(GlobalConfig::TextColor);
    list << QString("QWidget#frmVideoPanelTool>QPushButton:hover{color:%1;}").arg(AppConfig::ColorIconAlarm);
    list << QString("QWidget#frmVideoPanelTool>QPushButton:pressed{color:%1;}").arg(AppConfig::ColorIconAlarm);
    list << QString("QWidget#frmVideoPanelTool>QPushButton:checked{color:%1;}").arg(AppConfig::ColorMsgSolved);

    //云台圆形按钮
    list << QString("#widgetPtz>QPushButton{border-width:2px;border-radius:%1px;min-width:%2px;max-width:%2px;min-height:%2px;max-height:%2px;}").arg(17).arg(20);

    //标签加粗居中背景色突出等样式
    list << QString("QLabel#labTip{font-weight:bold;font-size:%1px;min-height:%2px;}").arg(GlobalConfig::FontSize + 2).arg(28);
    list << QString("QLabel#labTip{background-color:%1;qproperty-alignment:AlignCenter;}").arg(GlobalConfig::NormalColorEnd);

    //单独的视频控件样式
    list << QString("#videoWidgetLocal{qproperty-borderWidth:%1;qproperty-borderColor:%2;qproperty-bgColor:%3;}").arg(10).arg("#FF0000").arg("#0000FF");

    //去掉边框
    list << QString("QTreeView,QTabWidget::pane{border:0px;}");
    //部分控件需要单独设置边框
    list << QString("#tableWidgetMsg{border:0px;}");
    //预置位表格中的按钮去掉圆角
    list << QString("#btnPresetGoto,#btnPresetSet,#btnPresetRemove{border-radius:0px;margin:1px;padding:0px;}");

    //已播放时长和总时长标签
    list << QString("#labDuration,#labPosition{min-width:%1px;}").arg(AppData::BtnMinWidth * 0.6);
    list << QString("#frameSlider{min-height:%1px;}").arg(AppData::BtnMinWidth * 0.7);

    //播放按钮
    list << QString("#frameSlider>QPushButton,#frameSlider2>QPushButton{background:none;border:none;padding:0px 0px 0px 0px;}");
    list << QString("#frameSlider>QPushButton,#frameSlider2>QPushButton{color:%1;}").arg(GlobalConfig::TextColor);
    //list << QString("#frameSlider>QPushButton:hover{color:%1;}").arg(GlobalConfig::DarkColorEnd);

    //录像计划任务策略表格
    list << QString("TaskTableView{qproperty-bgColor:%1;}").arg(GlobalConfig::PanelColor);
    list << QString("TaskTableView{qproperty-gridColor:%1;}").arg(GlobalConfig::BorderColor);
    list << QString("TaskTableView{qproperty-headerBgColor:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("TaskTableView{qproperty-headerTextColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("TaskTableView{qproperty-normalColor:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("TaskTableView{qproperty-selectColor:%1;}").arg(GlobalConfig::HighColor);
    list << QString("TaskTableView{qproperty-shadowWidth:%1;}").arg(0);
    list << QString("TaskTableView{qproperty-shadowColor:%1;}").arg(GlobalConfig::DarkColorStart);

    //音量条控件
    list << QString("BarVolume{qproperty-radius:%1;}").arg(0);
    list << QString("BarVolume{qproperty-space:%1;}").arg(1);
    list << QString("BarVolume{qproperty-barBgColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("BarVolume{qproperty-bgColorStart:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("BarVolume{qproperty-bgColorEnd:%1;}").arg(GlobalConfig::NormalColorEnd);
}

void AppStyle::initStyle(const QString &styleName)
{
    AppStyle::styleName = styleName;
    QFile file(styleName);
    if (!file.open(QFile::ReadOnly)) {
        return;
    }

    QString qss = QLatin1String(file.readAll());
    file.close();

    //先从样式表中读取对应的颜色值到全局变量
    GlobalStyle::getQssColor(qss, GlobalConfig::TextColor, GlobalConfig::PanelColor, GlobalConfig::BorderColor, GlobalConfig::NormalColorStart,
                          GlobalConfig::NormalColorEnd, GlobalConfig::DarkColorStart, GlobalConfig::DarkColorEnd, GlobalConfig::HighColor);

    //判断主菜单和子菜单导航按钮的图标位置,合并成一个变量方便代码判断
    CommonNav::NavMainLeft = (AppConfig::NavStyle == 0 || AppConfig::NavStyle == 1);
    CommonNav::NavSubLeft = (AppConfig::NavStyle == 0 || AppConfig::NavStyle == 2);
    if (!CommonNav::NavSubLeft) {
        AppData::LeftWidth = 120;
    }

    //先添加通用样式再添加本项目自定义样式
    QStringList list;
    CommonStyle::addQtControlStyle(list);
    //组件示例中的视频控件强制边框1
    CommonStyle::addCustomControlStyle(list, AppConfig::IndexStart == 1 ? 1 : AppConfig::BorderWidth * 1);
    CommonStyle::addNavPageStyle(list, AppConfig::PageButtonCount);
    CommonStyle::addNavBtnStyle(list, CommonNav::NavMainLeft ? 20 : 5, 5);
    CommonStyle::addSwitchButtonStyle(list, styleName, AppData::SwitchBtnWidth, AppData::SwitchBtnHeight);
    CommonStyle::addDarkStyle(list, styleName);
    CommonStyle::addGaugeCloudStyle(list, styleName);
    CommonStyle::addFormStyle(list, styleName, AppData::BtnMinWidth);
    addCustomStyle(list);

    //将新增的样式加到统一样式表中
    qss += list.join("");
    GlobalStyle::setStyle(qss);

    //加载设备数列表图标
    DeviceHelper::initVideoIcon();
    //清空临时消息
    DeviceHelper::clearMsg();
}
