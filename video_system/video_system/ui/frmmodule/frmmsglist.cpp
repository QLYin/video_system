#include "frmmsglist.h"
#include "ui_frmmsglist.h"
#include "frmmsglistitem.h"
#include "qthelper.h"
#include "devicehelper.h"

frmMsgList::frmMsgList(QWidget *parent) : QWidget(parent), ui(new Ui::frmMsgList)
{
    ui->setupUi(this);
    this->initForm();
    this->initPanel();
    this->initAction();
}

frmMsgList::~frmMsgList()
{
    delete ui;
}

void frmMsgList::showEvent(QShowEvent *)
{
    static bool isLoad = false;
    if (!isLoad) {
        isLoad = true;
        resizeEvent(NULL);
    }

    if (AppConfig::MsgListCount != 0) {
        msgListCount = AppConfig::MsgListCount;
    }
}

void frmMsgList::resizeEvent(QResizeEvent *)
{
    //设置的0行表示自动计算行数直到不产生滚动条
    if (AppConfig::MsgListCount == 0 && isVisible()) {
        //根据高度变化自动设置行数
        int height = ui->panelWidget->height() - 10;
        //行数=高度/每行高度
        msgListCount = height / 80;
        checkCount();
    }
}

void frmMsgList::initForm()
{
    msgListCount = 1;
    DeviceHelper::setMsgList(this);
}

void frmMsgList::initPanel()
{
    //设置边距
    ui->panelWidget->setMargin(6);
    //设置间距
    ui->panelWidget->setSpace(6);
    //设置自动横向拉伸
    ui->panelWidget->setAutoWidth(true);
    //设置列数
    ui->panelWidget->setColumnCount(1);

    QStringList listMsg;
    listMsg << "画面有变动" << "有人在移动" << "检测到人脸" << "识别到人脸" << "有可疑车辆";
    QStringList listResult;
    listResult << "已处理!" << "待处理!";

    //不需要演示数据注释下面这段即可
#if 1
    QList<QWidget *> widgets;
    for (int i = 1; i <= 20; ++i) {
        //时间递减最近的在最前面
        QString time = QTime::currentTime().addSecs(-i).toString("HH:mm:ss");
        QString msg = listMsg.at(rand() % listMsg.count());
        QString result = listResult.at(rand() % listResult.count());
        QImage image(QString(":/image/demo%1.jpg").arg(rand() % 4));
        frmMsgListItem *widget = new frmMsgListItem;
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        widget->setMsg(msg, result, image, time);
        widgets << widget;
    }

    //设置窗体集合到容器
    ui->panelWidget->setWidgets(widgets);
#endif
}

void frmMsgList::initAction()
{
    //增加右键菜单操作用于演示各种动作
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    QStringList listTexts;
    listTexts << "添加消息" << "删除消息" << "清空消息";
    foreach (QString text, listTexts) {
        QAction *action = new QAction(text, this);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(doAction()));
        this->addAction(action);
    }
}

void frmMsgList::doAction()
{
    QAction *action = (QAction *)sender();
    QString text = action->text();
    if (text == "添加消息") {
        QImage image(QString(":/image/liuyifei%1.jpg").arg(rand() % 3));
        //addMsg("检测人脸!", image);
        //测试直接调用全局函数设置
        DeviceHelper::addMsgList("检测到人脸", "待处理!", image);
    } else if (text == "删除消息") {

    } else if (text == "清空消息") {
        clearMsg();
    }
}

void frmMsgList::checkCount()
{
    //判断数量是否超过最大值超过则移除多余的
    while (ui->panelWidget->getWidgets().count() >= msgListCount) {
        QWidget *widget = ui->panelWidget->getWidgets().last();
        ui->panelWidget->removeWidget(widget);
        widget->deleteLater();
    }
}

void frmMsgList::addMsg(const QString &msg, const QString &result, const QImage &image, const QString &time)
{
    checkCount();

    //添加新的卡片消息窗体
    frmMsgListItem *widget = new frmMsgListItem;
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widget->setMsg(msg, result, image, time);
    ui->panelWidget->insertWidget(0, widget);
}

void frmMsgList::clearMsg()
{
    ui->panelWidget->clearWidgets();
}
