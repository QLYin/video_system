#include "frmdemohelper.h"
#include "ui_frmdemohelper.h"
#include "qthelper.h"

frmDemoHelper::frmDemoHelper(QWidget *parent) : QWidget(parent), ui(new Ui::frmDemoHelper)
{
    ui->setupUi(this);
#if 0
    //更改设备上下线图标
    DeviceHelper::setVideoIcon("https://hls01open.ys7.com/openlive/6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8", false);
    //更改设备上下线图标
    DeviceHelper::setVideoIcon("https://hls01open.ys7.com/openlive/6e0b2be040a943489ef0b9bb344b96b8.hd.m3u8", true);
#endif
}

frmDemoHelper::~frmDemoHelper()
{
    delete ui;
}

void frmDemoHelper::on_btnShowInfo_clicked()
{
    QtHelper::showMessageBoxInfo("测试弹出信息框, 自动关闭!", 3);
}

void frmDemoHelper::on_btnShowError_clicked()
{
    QtHelper::showMessageBoxError("测试弹出错误框, 不自动关闭!");
}

void frmDemoHelper::on_btnShowQuestion_clicked()
{
    if (QtHelper::showMessageBoxQuestion("确定要删除记录吗? 删除后不可恢复!") == QMessageBox::Yes) {
        QtHelper::showMessageBoxInfo("恭喜你删除记录成功!", 3);
    }
}

void frmDemoHelper::on_btnShowInput_clicked()
{
    QString result = QtHelper::showInputBox("请输入姓名:");
    if (!result.isEmpty()) {
        QtHelper::showMessageBoxInfo(QString("您输入的姓名是 : %1").arg(result));
    }
}

void frmDemoHelper::on_btnShowDate_clicked()
{
    QString dateStart, dateEnd;
    if (QtHelper::showDateSelect(dateStart, dateEnd) == QMessageBox::Ok) {
        QString msg = QString("你选择的日期范围是 %1 到 %2").arg(dateStart).arg(dateEnd);
        QtHelper::showMessageBoxInfo(msg, 3);
    }
}

void frmDemoHelper::on_btnShowTip_clicked()
{
    static int index = 0;
    if (index == 0) {
        //默认文本居中,没有关闭倒计时
        QtHelper::showTipBox(AppConfig::TitleCn, "专业各种自定义控件和UI界面定制", AppConfig::FullScreen);
    } else if (index == 1) {
        //左对齐写法
        QtHelper::showTipBox(AppConfig::TitleCn, "主营业务如下：\n1：UI界面定制\n2：输入法定制\n3：自定义控件定制\n4：各种软硬件系统开发", AppConfig::FullScreen, false);
    } else if (index == 2) {
        //自动倒计时关闭写法
        QtHelper::showTipBox(AppConfig::TitleCn, "主营业务如下：\n1：UI界面定制\n2：输入法定制\n3：自定义控件定制\n4：各种软硬件系统开发", AppConfig::FullScreen, false, 5);
    } else if (index == 3) {
        //全屏位置
        QtHelper::showTipBox(AppConfig::TitleCn, "主营业务如下：\n1：UI界面定制\n2：输入法定制\n3：自定义控件定制\n4：各种软硬件系统开发", AppConfig::FullScreen, true, 15);
    }

    index++;
    if (index == 3) {
        index = 0;
    }
}

void frmDemoHelper::on_btnHideTip_clicked()
{
    QtHelper::hideTipBox();
}

void frmDemoHelper::on_btnShowSplash_clicked()
{
    QtHelper::showSplashInfo("正在努力关闭系统请稍等...", 50, 15, 5);
}

void frmDemoHelper::on_btnShowAbout_clicked()
{
    QtHelper::showAboutInfo(AboutInfo(), 5);
}

