#include "commonstyle.h"
#include "qthelper.h"

void CommonStyle::addQtControlStyle(QStringList &list)
{
    //默认Qt对话框样式
    list << QString("QMessageBox{background:%1;}").arg("#FFFFFF");
    list << QString("QMessageBox>QLabel{color:%1;}").arg("#000000");
    list << QString("QDialogButtonBox>QPushButton{min-width:%1px;}").arg(50);

    //悬浮窗体背景色
    list << QString("QDockWidget{background:%1;}").arg(GlobalConfig::BorderColor);
    //分隔条背景颜色
    list << QString("QSplitter{qproperty-handleWidth:1px;}QSplitter::handle{background:%1;}").arg(GlobalConfig::BorderColor);
    //停靠窗体标题栏样式
    list << QString("CustomTitleBar{background:%1;border-bottom:1px solid %2;}").arg(GlobalConfig::NormalColorStart).arg(GlobalConfig::BorderColor);
    //停靠窗体分隔条样式,他娘的根本不是QSplitter,做梦也想不到这样设置,搞了很久
    list << QString("QMainWindow::separator{width:%1px;height:%1px;margin:%2px;padding:%2px;background:%3;}").arg(2).arg(0).arg(GlobalConfig::BorderColor);

    //选项卡居中
    list << QString("QTabBar,QTabWidget::tab-bar{alignment:center;}");
    //选项卡 不显示切换按钮 自动拉伸-居然自带了卧槽之前还自己来计算
    list << QString("QTabBar{qproperty-usesScrollButtons:false;qproperty-documentMode:true;qproperty-expanding:true;}");

    //提示信息背景透明度
    list << QString("QToolTip{opacity:230;}");
    //树状控件
    list << QString("QTreeView{padding:5px 0px 5px 5px;}");
    //日历控件
    list << QString("QCalendarWidget{qproperty-gridVisible:true;}");

    //表格奇数偶数行不同颜色
    list << QString("QTableView,QTreeView{qproperty-alternatingRowColors:false;}");
    //单元格光标在最前面
    list << QString("QGroupBox>QLineEdit,QFrame>QLineEdit{qproperty-cursorPosition:0;}");
    //下拉框按照最小尺寸拉伸
    list << QString("QComboBox{qproperty-sizeAdjustPolicy:AdjustToMinimumContentsLengthWithIcon}");

    //密码框显示密码按钮
    list << QString("XLineEdit{qproperty-showPwdButton:true;}");
}

void CommonStyle::addCustomControlStyle(QStringList &list, int borderWidth)
{
    //硬盘使用空间控件
    list << QString("DeviceSizeTable{qproperty-bgColor:%1;}").arg(GlobalConfig::PanelColor);
    list << QString("DeviceSizeTable{qproperty-chunkColor1:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("DeviceSizeTable{qproperty-chunkColor2:%1;}").arg(GlobalConfig::HighColor);
    list << QString("DeviceSizeTable{qproperty-textColor1:%1;}").arg(GlobalConfig::TextColor);
    list << QString("DeviceSizeTable{qproperty-textColor2:%1;}").arg(GlobalConfig::PanelColor);

    //视频播放控件
    list << QString("AbstractVideoWidget{qproperty-borderWidth:%1;}").arg(borderWidth);
    list << QString("AbstractVideoWidget{qproperty-bgTextSize:%1;}").arg(GlobalConfig::FontSize + 10);
    list << QString("AbstractVideoWidget{qproperty-borderColor:%1;}").arg(GlobalConfig::BorderColor);
    list << QString("AbstractVideoWidget{qproperty-focusColor:%1;}").arg(GlobalConfig::HighColor);
    list << QString("AbstractVideoWidget{qproperty-textColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("AbstractVideoWidget{qproperty-borderColor:%1;}").arg(GlobalConfig::BorderColor);

    //背景颜色可以搞个透明度更美观
    list << QString("AbstractVideoWidget{qproperty-bannerBgAlpha:%1;}").arg(200);
    list << QString("AbstractVideoWidget{qproperty-bannerBgColor:%1;}").arg(GlobalConfig::BorderColor);
    list << QString("AbstractVideoWidget{qproperty-bannerTextColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("AbstractVideoWidget{qproperty-bannerPressColor:%1;}").arg(GlobalConfig::HighColor);

    //视频回放控件
    list << QString("VideoPlayback{qproperty-bgColor:%1;qproperty-videoBgColor:%1;}").arg(GlobalConfig::PanelColor);
    list << QString("VideoPlayback{qproperty-textColor:%1;qproperty-videoTextColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("VideoPlayback{qproperty-videoChColor:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("VideoPlayback{qproperty-videoDataColor:%1;}").arg(GlobalConfig::HighColor);

    //自定义面板标题控件
    list << QString("NavTitle{qproperty-bgColor:%1;}").arg(GlobalConfig::NormalColorStart);
    list << QString("NavTitle{qproperty-textColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("NavTitle{qproperty-borderColor:%3;}").arg(GlobalConfig::BorderColor);
    list << QString("NavTitle{qproperty-iconNormalColor:%1;}").arg(GlobalConfig::TextColor);
    list << QString("NavTitle{qproperty-iconHoverColor:%1;}").arg(GlobalConfig::HighColor);
    list << QString("NavTitle{qproperty-iconPressColor:%1;}").arg(GlobalConfig::BorderColor);
}

void CommonStyle::addNavPageStyle(QStringList &list, int pageButtonCount)
{
    //分页导航
    list << QString("NavPage{qproperty-pageButtonCount:%1;qproperty-showLabInfo:%2;}")
         .arg(pageButtonCount).arg("true");
    list << QString("NavPage{qproperty-fontSize:%1;qproperty-borderWidth:%2;qproperty-borderRadius:%3;qproperty-borderColor:%4;}")
         .arg(GlobalConfig::FontSize + 5).arg(0).arg(5).arg(GlobalConfig::BorderColor);
    list << QString("NavPage{qproperty-normalBgColor:%1;qproperty-normalTextColor:%2;}")
         .arg(GlobalConfig::NormalColorEnd).arg(GlobalConfig::TextColor);
    list << QString("NavPage{qproperty-hoverBgColor:%1;qproperty-hoverTextColor:%2;}")
         .arg(GlobalConfig::DarkColorStart).arg(GlobalConfig::TextColor);
    list << QString("NavPage{qproperty-pressedBgColor:%1;qproperty-pressedTextColor:%2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::TextColor);
    list << QString("NavPage{qproperty-checkedBgColor:%1;qproperty-checkedTextColor:%2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::TextColor);
}

void CommonStyle::addNavBtnStyle(QStringList &list, int topBtnRadius, int leftBtnRadius)
{
    //顶部导航按钮,可以自行修改圆角角度,采用弱属性机制[flag=\"btnNavTop\"],也可采用对象名#widgetBtn
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton{font-size:%1px;border-radius:%2px;}")
         .arg(GlobalConfig::FontSize + 3).arg(topBtnRadius);
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton{background:%1;border:2px solid %2;}")
         .arg("transparent").arg("transparent");
    //悬停和选中可以分开不同颜色
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton:hover{background:%1;border:2px solid %2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::BorderColor);
    list << QString("QWidget[flag=\"btnNavTop\"]>QAbstractButton:checked{background:%1;border:2px solid %2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::BorderColor);

    //左侧导航按钮,可以自行修改圆角角度,采用弱属性机制[flag=\"btnNavLeft\"],也可采用对象名#widgetLeft
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton{font-size:%1px;border-radius:%2px;}")
         .arg(GlobalConfig::FontSize + 3).arg(leftBtnRadius);
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton{background:%1;border:2px solid %2;}")
         .arg("transparent").arg("transparent");
    //悬停和选中可以分开不同颜色
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton:hover{background:%1;border:2px solid %2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::BorderColor);
    list << QString("QWidget[flag=\"btnNavLeft\"]>QAbstractButton:checked{background:%1;border:2px solid %2;}")
         .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::BorderColor);
}

void CommonStyle::addSwitchButtonStyle(QStringList &list, const QString &styleName, int btnWidth, int btnHeight)
{
    //尺寸大小
    list << QString("SwitchButton{min-width:%1px;max-width:%1px;min-height:%2px;max-height:%2px;}").arg(btnWidth).arg(btnHeight);

    //加深的样式颜色需要反着来效果更明显
    bool dark = GlobalStyle::isDark1(styleName);
    if (dark) {
        list << QString("SwitchButton{qproperty-bgColorOn:%1;qproperty-bgColorOff:%2;}")
             .arg(GlobalConfig::DarkColorEnd).arg(GlobalConfig::NormalColorEnd);
    } else {
        list << QString("SwitchButton{qproperty-bgColorOn:%1;qproperty-bgColorOff:%2;}")
             .arg(GlobalConfig::NormalColorEnd).arg(GlobalConfig::DarkColorEnd);
    }

    //禁用状态设置颜色透明度区分
    QColor color(GlobalConfig::TextColor);
    color.setAlpha(100);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QString textColor = color.name(QColor::HexArgb);
#else
    QString textColor = GlobalConfig::PanelColor;
#endif
    list << QString("SwitchButton{qproperty-textColorOn:%1;qproperty-textColorOff:%2;}")
         .arg(GlobalConfig::TextColor).arg(textColor);
    list << QString("SwitchButton{qproperty-sliderColorOn:%1;qproperty-sliderColorOff:%2;}")
         .arg(GlobalConfig::PanelColor).arg(GlobalConfig::PanelColor);
}

void CommonStyle::addDarkStyle(QStringList &list, const QString &styleName)
{
    //加深的样式需要设置不一样的颜色
    bool dark = GlobalStyle::isDark2(styleName);
    if (dark) {
        list << QString();
        list << QString("XSlider{qproperty-normalColor:%1;qproperty-grooveColor:%2;qproperty-borderColor:%2;}")
             .arg(GlobalConfig::BorderColor).arg(GlobalConfig::DarkColorEnd);
        list << QString("XSlider{qproperty-handleColor:%1;qproperty-textColor:%2;}")
             .arg(GlobalConfig::TextColor).arg(GlobalConfig::BorderColor);
        list << QString("CustomPlot{qproperty-bgColor:%1;qproperty-textColor:%2;qproperty-gridColor:%2;}")
             .arg(GlobalConfig::BorderColor).arg(GlobalConfig::TextColor);
    } else {
        list << QString("XSlider{qproperty-normalColor:%1;qproperty-grooveColor:%2;qproperty-borderColor:%2;}")
             .arg(GlobalConfig::NormalColorStart).arg(GlobalConfig::BorderColor);
        list << QString("XSlider{qproperty-handleColor:%1;qproperty-textColor:%2;}")
             .arg(GlobalConfig::PanelColor).arg(GlobalConfig::TextColor);
        list << QString("CustomPlot{qproperty-bgColor:%1;qproperty-textColor:%2;qproperty-gridColor:%2;}")
             .arg(GlobalConfig::NormalColorStart).arg(GlobalConfig::TextColor);
    }

    //重新设置单元格等item选中和悬停颜色 默认悬停-DarkColorEnd 选中-NormalColorEnd
    //也可以设置两种颜色为一种颜色
    if (dark) {
        GlobalConfig::HoverBgColor = GlobalConfig::NormalColorEnd;
        GlobalConfig::SelectBgColor = GlobalConfig::DarkColorEnd;
    }

    //哪个最后设置则 选中+悬停 状态就用哪个颜色
    GlobalConfig::HoverCoverSelected = false;
    if (GlobalConfig::HoverCoverSelected) {
        list << QString("QTableView::item:selected,QListView::item:selected,QTreeView::item:selected{background:%1;}").arg(GlobalConfig::SelectBgColor);
        list << QString("QTableView::item:hover,QListView::item:hover,QTreeView::item:hover{background:%1;}").arg(GlobalConfig::HoverBgColor);
    } else {
        list << QString("QTableView::item:hover,QListView::item:hover,QTreeView::item:hover{background:%1;}").arg(GlobalConfig::HoverBgColor);
        list << QString("QTableView::item:selected,QListView::item:selected,QTreeView::item:selected{background:%1;}").arg(GlobalConfig::SelectBgColor);
    }

    //设备面板
    list << QString("PanelItem{qproperty-titleColor:%1;}").arg(GlobalConfig::PanelColor);
    list << QString("PanelItem{qproperty-borderColor:%1;}").arg(GlobalConfig::HighColor);
    list << QString("PanelItem{qproperty-titleDisableColor:%1;}").arg(GlobalConfig::DarkColorEnd);
    list << QString("PanelItem{qproperty-borderDisableColor:%1;}").arg(GlobalConfig::BorderColor);

    //进度条
#ifdef __arm__
    int sliderHeight = 16;
#else
    int sliderHeight = 12;
#endif
    list << QString("XSlider{qproperty-sliderHeight:%1;qproperty-showText:%2;}").arg(sliderHeight).arg(false);
    list << QString("#sliderPtzStep,#sliderFps{qproperty-sliderHeight:%1;qproperty-showText:%2;}").arg(15).arg(true);
}

void CommonStyle::addGaugeCloudStyle(QStringList &list, const QString &styleName)
{
    //是否显示四个斜角
    //list << QString("GaugeCloud{qproperty-showAngle:false;}");

    //根据不同的样式设置不同的风格
    if (styleName.contains("blackvideo")) {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Black;}");
    } else if (styleName.contains("blackblue") || styleName.contains("darkblue")) {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Blue;}");
    } else if (styleName.contains("otherpurple")) {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Purple;}");
    } else if (styleName.contains("flatgray")) {
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_White;}");
    } else {
        //特殊颜色处理
        QString colorStart = GlobalConfig::DarkColorStart;
        QString colorEnd = GlobalConfig::DarkColorEnd;
        if (GlobalStyle::isDark3(styleName)) {
            colorStart = GlobalConfig::NormalColorStart;
            colorEnd = GlobalConfig::NormalColorEnd;
        }

        QString centerHoverColor = GlobalConfig::NormalColorEnd;
        if (styleName.contains("normalgray") || styleName.contains("lightblue")) {
            centerHoverColor = GlobalConfig::DarkColorStart;
        }

        //自定义风格
        list << QString("GaugeCloud{qproperty-cloudStyle:CloudStyle_Custom;}");

        //渐变边框颜色
        list << QString("GaugeCloud{qproperty-borderColor1:%1;}").arg(colorStart);
        list << QString("GaugeCloud{qproperty-borderColor2:%1;}").arg(colorEnd);
        list << QString("GaugeCloud{qproperty-borderColor3:%1;}").arg(colorEnd);
        list << QString("GaugeCloud{qproperty-borderColor4:%1;}").arg(colorStart);

        //背景颜色/文字颜色/鼠标进入颜色/鼠标按下颜色
        list << QString("GaugeCloud{qproperty-bgColor:%1;}").arg(GlobalConfig::PanelColor);
        list << QString("GaugeCloud{qproperty-textColor:%1;}").arg(GlobalConfig::TextColor);
        list << QString("GaugeCloud{qproperty-enterColor:%1;}").arg("#47CAF6");
        list << QString("GaugeCloud{qproperty-pressColor:%1;}").arg(GlobalConfig::HighColor);

        //中间边框颜色/背景颜色/悬停颜色
        list << QString("GaugeCloud{qproperty-centerBorderColor:%1;}").arg(GlobalConfig::BorderColor);
        list << QString("GaugeCloud{qproperty-centerBgColor:%1;}").arg(GlobalConfig::PanelColor);
        list << QString("GaugeCloud{qproperty-centerHoverColor:%1;}").arg(centerHoverColor);
    }
}

void CommonStyle::addFormStyle(QStringList &list, const QString &styleName, int btnMinWidth)
{
    //中英文标题字体加粗
    list << QString("#labTitleCn{font-size:%1px;}").arg(GlobalConfig::FontSize + 13);
    list << QString("#labTitleEn{font-size:%1px;}").arg(GlobalConfig::FontSize + 2);

    //停靠窗体自定义标题栏字体放大
    list << QString("#dockTitle{font:%1px;min-height:%2px;}").arg(GlobalConfig::FontSize + 3).arg(20);
    list << QString("QLabel[flag=\"title\"]{border:none;padding:5px;}");

    //添加删除保存清空等一排按钮最小宽度
    list << QString("QWidget[flag=\"navbtn\"] QPushButton{min-width:%1px;}").arg(btnMinWidth);

    //登录登出窗体软件标题样式
    QString textColor = "#FFFFFF";
    if (styleName.contains("darkblue") || styleName.contains("blackblue") || styleName.contains("purple")) {
        textColor = GlobalConfig::TextColor;
    }
    list << QString("#frmLogin>#labName,#frmLogout>#labName{font:22px;color:%1;}").arg(textColor);

    //委托中的按钮样式
    list << QString("#DbDelegate_pushButton{border-radius:0px;margin:1px;padding:1px;}");

    //主背景
    list << QString("QWidget#widgetMain{background:%1;}").arg(GlobalConfig::BorderColor);
    //右上角菜单
    list << QString("QWidget#widgetMenu>QPushButton{border-radius:0px;padding:0px;margin:1px 1px 2px 1px;}");
}
