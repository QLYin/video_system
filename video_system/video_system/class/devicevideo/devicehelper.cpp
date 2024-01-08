#include "devicehelper.h"
#include "qthelper.h"
#include "videourl.h"
#include "playwav.h"
#include "dbquery.h"
#include "devicemap.h"
#include "devicebutton.h"
#include "videomanage.h"
#include "videohelper.h"
#include "urlhelper.h"
#include "frmvideowidgetpreview.h"
#include "frmvideowidgetslider.h"
#include "frmmsglist.h"

namespace {
QLabel *labMap = NULL;              //显示背景图和存放设备控件
QTableWidget *deviceTable = NULL;   //显示临时消息
QTreeWidget *deviceTree = NULL;     //设备树状列表
QListWidget *deviceMap = NULL;      //地图缩略图
frmMsgList *deviceMsg = NULL;       //图文警情列表
QList<DeviceButton *> btns;         //设备按钮集合
QTreeWidget *deviceTreeTVWall = NULL;     //设备树状列表 -- 电视墙
}

void DeviceHelper::setLabel(QLabel *label)
{
    labMap = label;
}

void DeviceHelper::setTableWidget(QTableWidget *tableWidget)
{
    deviceTable = tableWidget;
    QtHelper::initTableView(deviceTable, AppData::RowHeight, false, false);
}

void DeviceHelper::setTreeWidget(QTreeWidget *treeWidget)
{
    deviceTree = treeWidget;
}

void DeviceHelper::setTVWallTreeWidget(QTreeWidget* treeWidget)
{
    deviceTreeTVWall = treeWidget;
}

void DeviceHelper::setListWidget(QListWidget *listWidget)
{
    deviceMap = listWidget;
}

void DeviceHelper::setMsgList(frmMsgList *msgList)
{
    deviceMsg = msgList;
}

void DeviceHelper::initDeviceTable()
{
    if (!deviceTable) {
        return;
    }

    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "时间" << "消息";
    columnWidths << 60 << 80;
    deviceTable->setRowCount(AppConfig::MsgTableCount);

    //设置标题+列宽
    int columnCount = columnNames.count();
    deviceTable->setColumnCount(columnCount);
    deviceTable->setHorizontalHeaderLabels(columnNames);
    for (int i = 0; i < columnCount; ++i) {
        deviceTable->setColumnWidth(i, columnWidths.at(i));
    }
}

void DeviceHelper::addMsg(const QString &msg, quint8 type, bool log)
{
    if (!deviceTable) {
        return;
    }

    //移除末尾一行,将当前消息插入到第一行
    deviceTable->removeRow(deviceTable->rowCount() - 1);
    deviceTable->insertRow(0);

    //根据不同的类型设置不同的文字颜色
    //如果只想采用系统的配色自动切换则注释下面的判断即可
    QColor textColor = GlobalConfig::TextColor;
    if (type == 0) {
        //可选跟随系统还是设定的颜色
        textColor = GlobalConfig::TextColor;
        //textColor = AppConfig::ColorTextNormal;
    } else if (type == 1) {
        textColor = AppConfig::ColorTextWarn;
    } else if (type == 2) {
        textColor = AppConfig::ColorTextAlarm;
    }

    QTableWidgetItem *itemTime = new QTableWidgetItem(TIME);
    itemTime->setTextAlignment(Qt::AlignCenter);
    itemTime->setForeground(textColor);
    deviceTable->setItem(0, 0, itemTime);

    QTableWidgetItem *itemMsg = new QTableWidgetItem(msg);
    //itemMsg->setTextAlignment(Qt::AlignCenter);
    itemMsg->setForeground(textColor);
    deviceTable->setItem(0, 1, itemMsg);

    //插入打印的信息到用户操作记录
    if (log) {
        DbQuery::addUserLog(msg);
    }
}

void DeviceHelper::clearMsg()
{
    if (!deviceTree) {
        return;
    }

    deviceTable->clearContents();
}

void DeviceHelper::addMsgList(const QString &msg, const QString &result, const QImage &image, const QString &time)
{
    if (!deviceMsg) {
        return;
    }

    deviceMsg->addMsg(msg, result, image, time);
}

void DeviceHelper::clearMsgList()
{
    if (!deviceMsg) {
        return;
    }

    deviceMsg->clearMsg();
}

void DeviceHelper::initDeviceTree()
{
    QList<QTreeWidget*> deviceTrees;
    deviceTrees.push_back(deviceTree);
    deviceTrees.push_back(deviceTreeTVWall);
    for (auto& tree : deviceTrees)
    {
        initDeviceTree(tree);
    }
}

void DeviceHelper::initDeviceTree(QTreeWidget *treeWidget)
{
    if (!treeWidget) {
        return;
    }

    treeWidget->clear();
    treeWidget->setAnimated(false);
    treeWidget->setHeaderHidden(true);
    //设置节点双击禁用打开
    //treeWidget->setItemsExpandable(false);
    //设置树状节点可拖曳
    treeWidget->setDragEnabled(true);

    //有些年纪大的老板说字体要放大
    if (AppConfig::TreeBig) {
        QFont font = treeWidget->font();
        font.setPixelSize(20);
        treeWidget->setFont(font);
        //通过设置节点左侧距离来调整前面branch图标大小
        treeWidget->setIndentation(30);
        treeWidget->setIconSize(QSize(25, 25));
    }

    //展开节点的索引集合
    QStringList listExpandItem = AppConfig::ExpandItem.split(",");

    //从数据库中加载,先加载NVR,再遍历NVR下面的IPC
    QStringList nvrNames;
    for (int i = 0; i < DbData::NvrInfo_Count; ++i) {
        //添加录像机节点
        QString nvrName = DbData::NvrInfo_NvrName.at(i);
        QString nvrIP = DbData::NvrInfo_NvrIP.at(i);
        //过滤一样的名字的只添加一次
        if (nvrNames.contains(nvrName)) {
            continue;
        }

        nvrNames << nvrName;
        QTreeWidgetItem *itemNvr = new QTreeWidgetItem(treeWidget);
        itemNvr->setText(0, nvrName);
        itemNvr->setData(0, Qt::UserRole, nvrIP);

        //循环添加录像机下的摄像机节点
        for (int j = 0; j < DbData::IpcInfo_Count; j++) {
            if (DbData::IpcInfo_NvrName.at(j) == nvrName) {
                QString ipcName = DbData::IpcInfo_IpcName.at(j);
                QString rtspMain = DbData::IpcInfo_RtspMain.at(j);
                QString rtspSub = DbData::IpcInfo_RtspSub.at(j);
                QString rtspAddr = DbData::getRtspAddr(j);

                //添加摄像机节点
                QTreeWidgetItem *itemIpc = new QTreeWidgetItem(itemNvr);
                itemIpc->setText(0, ipcName);
                itemIpc->setData(0, Qt::UserRole, rtspAddr);

                //主码流子码流节点
                if (AppConfig::TreeRtsp) {
                    QTreeWidgetItem *itemRtspMain = new QTreeWidgetItem(itemIpc);
                    itemRtspMain->setText(0, "主码流");
                    itemRtspMain->setData(0, Qt::UserRole, rtspMain);

                    QTreeWidgetItem *itemRtspSub = new QTreeWidgetItem(itemIpc);
                    itemRtspSub->setText(0, "子码流");
                    itemRtspSub->setData(0, Qt::UserRole, rtspSub);
                }
            }
        }

        //根据展开节点的记录来展开对应节点
        if (listExpandItem.contains(QString::number(i))) {
            treeWidget->expandItem(itemNvr);
        }
    }
}

void DeviceHelper::initVideoIcon()
{
    if (!deviceTree && !deviceTreeTVWall) {
        return;
    }

    QList<QTreeWidget*> deviceTrees;
    deviceTrees.push_back(deviceTree);
    deviceTrees.push_back(deviceTreeTVWall);

    //不同的节点显示不同的图标
    for (auto& tree : deviceTrees)
    {      
        initVideoIcon(tree);
    }
}

void DeviceHelper::initVideoIcon(QTreeWidget* treeWidget)
{
    //图标的大小和宽高
    int size = AppConfig::TreeBig ? 22 : 18;
    int width = size + 2;
    int height = size + 2;

    //可以自行查找对照表改成不同的图标
    QPixmap iconNvr = IconHelper::getPixmap(GlobalConfig::TextColor, 0xea23, size, width, height);
    //如果没有启动离线检测则默认在线
    QString color = AppConfig::CheckOffline ? AppConfig::ColorIconAlarm : GlobalConfig::TextColor;
    QPixmap iconIpc = IconHelper::getPixmap(color, 0xea07, size, width, height);
    QPixmap iconSub = IconHelper::getPixmap(color, 0xe9ff, size, width, height);

    QTreeWidgetItemIterator it(treeWidget);
    while (*it) {
        QTreeWidgetItem* item = (*it);
        QString text = item->text(0);
        if (item->parent() == 0) {
            item->setIcon(0, iconNvr);
        }
        else if (text == "主码流" || text == "子码流") {
            item->setIcon(0, iconSub);
        }
        else {
            item->setIcon(0, iconIpc);
        }
        ++it;
    }
}

void DeviceHelper::setVideoIcon(const QString &url, bool online)
{
    setVideoIcon2(UrlHelper::getUrlIP(url), online);
}

void DeviceHelper::setVideoIcon2(const QString &ip, bool online)
{
    if (!deviceTree) {
        return;
    }

    QList<QTreeWidget*> deviceTrees;
    deviceTrees.push_back(deviceTree);
    deviceTrees.push_back(deviceTreeTVWall);

    //图标的大小和宽高
    int size = AppConfig::TreeBig ? 22 : 18;
    int width = size + 2;
    int height = size + 2;

    //摄像头图标在线离线
    QPixmap iconIpc = IconHelper::getPixmap(GlobalConfig::TextColor, 0xea07, size, width, height);
    QPixmap iconSub = IconHelper::getPixmap(GlobalConfig::TextColor, 0xe9ff, size, width, height);
    QPixmap iconIpcx = IconHelper::getPixmap(AppConfig::ColorIconAlarm, 0xea07, size, width, height);
    QPixmap iconSubx = IconHelper::getPixmap(AppConfig::ColorIconAlarm, 0xe9ff, size, width, height);

    for (auto& tree : deviceTrees)
    {
        QTreeWidgetItemIterator it(tree);
        while (*it) {
            QTreeWidgetItem* item = (*it);
            QString url = item->data(0, Qt::UserRole).toString();
#if 1
            //限定对摄像机节点处理就行
            if (item->childCount() > 0 && item->parent()) {
                //如果不是带IP的地址则会自动用原地址判断
                if (UrlHelper::getUrlIP(url) == ip) {
                    item->setIcon(0, online ? iconIpc : iconIpcx);
                    //对下面的码流节点同样处理
                    item->child(0)->setIcon(0, online ? iconSub : iconSubx);
                    item->child(1)->setIcon(0, online ? iconSub : iconSubx);
                    //这里还要继续因为可能是同一台NVR他的IP地址是一样的
                    //break;
                }
            }
#else
            if (UrlHelper::getUrlIP(url) == ip) {
                //item->setDisabled(!online);
                if (online) {
                    item->setIcon(0, item->childCount() > 0 ? iconIpc : iconSub);
                }
                else {
                    item->setIcon(0, item->childCount() > 0 ? iconIpcx : iconSubx);
                }
            }
#endif
            ++it;
}
    }
}

void DeviceHelper::initDeviceMap()
{
    initDeviceMap(deviceMap);
}

void DeviceHelper::initDeviceMap(QListWidget *listWidget)
{
    if (!deviceMap) {
        return;
    }

    listWidget->clear();
    //图标模式
    listWidget->setViewMode(QListView::IconMode);
    //自适应大小
    listWidget->setResizeMode(QListView::Adjust);
    //不允许拖动元素
    listWidget->setMovement(QListView::Static);
    //设置图标大小
    listWidget->setIconSize(QSize(AppData::RightWidth - 25, AppData::RightWidth - 20));

    int count = AppData::MapNames.count();
    for (int i = 0; i < count; ++i) {
        QString imageName = AppData::MapNames.at(i);
        QPixmap pix = DeviceMap::Instance()->getMapPix(imageName);
        QString itemName = QtHelper::cutString(imageName, 12, 6, 6, true);
        QListWidgetItem *listItem = new QListWidgetItem(listWidget, i);
        listItem->setIcon(QIcon(pix));
        listItem->setText(itemName);
        listItem->setData(Qt::UserRole, imageName);
        listItem->setTextAlignment(Qt::AlignCenter);
    }
}

void DeviceHelper::initDeviceMapCurrent(const QString &imgName)
{
    initDeviceMapCurrent(labMap, imgName);
}

void DeviceHelper::initDeviceMapCurrent(QLabel *label, const QString &imgName)
{
    if (!labMap) {
        return;
    }

    if (label == labMap) {
        AppData::CurrentImage = imgName;
    }

    //通过设置弱属性记住图片名
    label->setProperty("image", imgName);
    label->setPixmap(DeviceMap::Instance()->getMapPix(imgName));

    //当前图片对应的设备按钮可见
    QList<DeviceButton *> btns = label->findChildren<DeviceButton *>();
    foreach (DeviceButton *btn, btns) {
        btn->setVisible(btn->property("ipcImage").toString() == imgName);
    }
}

void DeviceHelper::initDeviceButton()
{
    initDeviceButton(labMap);
}

void DeviceHelper::initDeviceButton(QLabel *label)
{
    if (!labMap) {
        return;
    }

    //先清空删除原来的
    qDeleteAll(btns);
    btns.clear();

    for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
        //地图为空的不需要
        if (DbData::IpcInfo_IpcImage.at(i).contains("无")) {
            continue;
        }

        DeviceButton *btn = new DeviceButton(label);
        QObject::connect(btn, SIGNAL(doubleClicked()), AppEvent::Instance(), SLOT(slot_doubleClicked()));
        btn->setButtonColor(DeviceButton::ButtonColor_Green);
        btn->setCanMove(true);
#if 1
        btn->setButtonStyle(DeviceButton::ButtonStyle_Msg2);
        btn->resize(AppData::DeviceWidth, AppData::DeviceHeight);
        btn->setText(QString("%1").arg(DbData::IpcInfo_IpcName.at(i)));
#else
        btn->setButtonStyle(DeviceButton::ButtonStyle_Custom);
        btn->resize(30, 30);
        btn->setText("");
#endif
        //设置弱属性来存储数据,后期有需要还可以自行增加其他
        btn->setProperty("ipcID", DbData::IpcInfo_IpcID.at(i));
        btn->setProperty("rtspMain", DbData::IpcInfo_RtspMain.at(i));
        btn->setProperty("rtspSub", DbData::IpcInfo_RtspSub.at(i));
        btn->setProperty("ipcImage", DbData::IpcInfo_IpcImage.at(i));

        //设置位置
        int x = DbData::IpcInfo_IpcX.at(i);
        int y = DbData::IpcInfo_IpcY.at(i);
        btn->move(x, y);
        btns << btn;
    }
}

void DeviceHelper::saveDeviceButtonPosition()
{
    //开启数据库事务加快速度
    QSqlDatabase::database().transaction();

    //逐个保存位置
    foreach (DeviceButton *btn, btns) {
        DbQuery::updatePosition(btn);
    }

    //提交失败回滚事务
    if (!QSqlDatabase::database().commit()) {
        QSqlDatabase::database().rollback();
    } else {
        //重新加载
        DbQuery::loadIpcInfo();
        changeDeviceButtonPosition();
    }
}

void DeviceHelper::changeDeviceButtonPosition()
{
    //改变现有设备的位置
    foreach (DeviceButton *btn, btns) {
        int ipcID = btn->property("ipcID").toInt();
        int index = DbData::IpcInfo_IpcID.indexOf(ipcID);
        if (index >= 0) {
            int x = DbData::IpcInfo_IpcX.at(index);
            int y = DbData::IpcInfo_IpcY.at(index);
            btn->move(x, y);
        }
    }
}

int DeviceHelper::initUrls()
{
    int count = 0;
    if (AppConfig::SaveUrl) {
        QString fileName = QtHelper::appPath() + "/config/url.txt";
        VideoUrl::readUrls(fileName, AppData::VideoUrls);

        //地址要求在摄像机表中存在并且处于开启状态
        for (int i = 0; i < AppData::VideoUrls.count(); ++i) {
            QString url = AppData::VideoUrls.at(i);
            if (url.isEmpty()) {
                continue;
            }

            //这里可以增加只对rtsp等开头的地址过滤(也可以注释掉不启用这个机制)
            count++;
            if (!url.startsWith("rtsp")) {
                continue;
            }

            int index1 = DbData::IpcInfo_RtspMain.indexOf(url);
            int index2 = DbData::IpcInfo_RtspSub.indexOf(url);
            if (index1 < 0 && index2 < 0) {
                AppData::VideoUrls[i] = "";
            }
            if (index1 >= 0 && DbData::IpcInfo_IpcEnable.at(index1) == "禁用") {
                AppData::VideoUrls[i] = "";
            }
            if (index2 >= 0 && DbData::IpcInfo_IpcEnable.at(index2) == "禁用") {
                AppData::VideoUrls[i] = "";
            }
        }
    }

    return count;
}

void DeviceHelper::saveUrls()
{
    if (AppConfig::SaveUrl) {
        QString fileName = QtHelper::appPath() + "/config/url.txt";
        VideoUrl::writeUrls(fileName, AppData::VideoUrls);
    }

    //将url地址设置到视频管理类中
    VideoManage::Instance()->setVideoUrls(AppData::VideoUrls);
    //重新启动管理线程(有可能外面停止了线程)
    VideoManage::Instance()->start();
}

void DeviceHelper::clearUrls()
{
    QString fileName = QtHelper::appPath() + "/config/url.txt";
    QFile::remove(fileName);
}

void DeviceHelper::changeUrls()
{
    //当摄像机管理中修改删除清空设备后要重新设置下主界面视频通道
    int count = AppData::VideoUrls.count();
    for (int i = 0; i < count; ++i) {
        QString url = AppData::VideoUrls.at(i);
        if (url.isEmpty()) {
            continue;
        }

        bool exist = false;
        QString rtspMain, rtspSub;
        for (int j = 0; j < DbData::IpcInfo_Count; ++j) {
            rtspMain = DbData::IpcInfo_RtspMain.at(j);
            rtspSub = DbData::IpcInfo_RtspSub.at(j);
            if (rtspMain == url || rtspSub == url) {
                exist = true;
                break;
            }
        }

        //对应地址不在则清空地址并关闭视频
        if (!exist) {
            AppData::VideoUrls[i] = "";
            VideoWidget *videoWidget = VideoManage::Instance()->getVideoWidget(url, url);
            if (videoWidget && videoWidget->getIsRunning()) {
                videoWidget->stop();
            }
        }
    }

    DeviceHelper::saveUrls();
}

void DeviceHelper::getDeviceInfo(QStringList &names, QStringList &addrs, QStringList &points)
{
    names.clear();
    addrs.clear();
    points.clear();

#if 0
    //随机产生数据
    for (int i = 1; i <= 20; ++i) {
        names << QString("摄像头%1").arg(i);
        addrs << QString("测试地址%1").arg(i);
        points << QString("121.%1,31.%2").arg(rand() % 1000000).arg(rand() % 1000000);
    }
#elif 0
    //写死数据
    names << "摄像头1" << "摄像头2" << "摄像头3" << "摄像头4" << "摄像头5";
    addrs << "安波路533弄1号楼" << "北新路8号" << "康桥镇康桥路1200号(御青路)" << "闵行区诸翟镇纪翟南路" << "浦东新区广兰路1080号(紫薇路口)";
    points << "121.534942,31.307706" << "121.572075,31.188825" << "121.57987,31.155795" << "121.292628,31.215278" << "121.626992,31.211056";
#else
    //从数据库加载
    names = DbData::IpcInfo_IpcName;
    //纠正经纬度坐标,在数据库中 可能是 | 分隔符,方便导出数据
    foreach (QString ipcPosition, DbData::IpcInfo_IpcPosition) {
        addrs << "摄像机默认位置";
        points << ipcPosition.replace("|", ",");
    }
#endif
}

bool DeviceHelper::checkOnline(const QString &url)
{
    if (!url.startsWith("rtsp")) {
        return true;
    }

    QString ip = UrlHelper::getUrlIP(url);
    int port = UrlHelper::getUrlPort(url);
    int timeout = QtHelper::getRandValue(1000, 2000);
    return QtHelper::hostLive(ip, port, timeout);
}

void DeviceHelper::showVideo(const QString &url, const QString &flag, int recordTime, const QString &fileName)
{
    if (url.isEmpty()) {
        return;
    }

    //提前检测下是否在线
    bool online = false;
    if (AppConfig::CheckOffline) {
        for (int i = 0; i < DbData::IpcInfo_Count; ++i) {
            if (DbData::IpcInfo_RtspMain.at(i) == url || DbData::IpcInfo_RtspSub.at(i) == url) {
                online = DbData::IpcInfo_IpcOnline.at(i);
                break;
            }
        }
    } else {
        online = checkOnline(url);
    }

    if (online) {
        frmVideoWidgetPreview *video = new frmVideoWidgetPreview;
        video->open(url, flag, recordTime, fileName);
    } else {
        QtHelper::showMessageBoxError("设备不在线, 请重新检查!");
    }
}

void DeviceHelper::showAlarmVideo(const QString &url)
{
    //创建视频播放控件
    VideoWidget *videoWidget = new VideoWidget;
    DeviceHelper::initVideoWidget(videoWidget, 100, false, true);

    //放入到视频播放窗体(带播放进度条)
    frmVideoWidgetSlider *widget = new frmVideoWidgetSlider(videoWidget);
    widget->resize(850, 600);
    widget->setWindowTitle("报警视频回放");
    widget->layout()->setContentsMargins(6, 6, 6, 6);
    widget->setWindowFlags(widget->windowFlags() | Qt::WindowStaysOnTopHint);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->show();

    //设置图片拉伸策略并播放
    videoWidget->setScaleMode(ScaleMode_Auto);
    videoWidget->open(url);
}

void DeviceHelper::showAlarmImage(const QString &url)
{
    QLabel *labImage = new QLabel;
    labImage->setWindowTitle("报警图片预览");
    labImage->setAlignment(Qt::AlignCenter);
    labImage->setWindowFlags(labImage->windowFlags() | Qt::WindowStaysOnTopHint);
    labImage->setAttribute(Qt::WA_DeleteOnClose);

    //自适应图片分辨率并限制最大分辨率
    QImage image(url);
    QSize size = image.size();
    if (size.width() > 1280 || size.height() > 720) {
        size.scale(1280, 720, Qt::KeepAspectRatio);
        image = image.scaled(size);
    }

    labImage->resize(image.size());
    labImage->setPixmap(QPixmap::fromImage(image));
    labImage->show();
}

void DeviceHelper::initVideoWidget(VideoWidget *videoWidget, int soundValue, bool soundMuted, bool bannerEnable, int readTimeout)
{
    //设置窗体参数
    WidgetPara widgetPara = videoWidget->getWidgetPara();
    widgetPara.borderWidth = AppConfig::BorderWidth * 1;
    widgetPara.bgImage = QImage(QString("%1/config/bg_novideo.png").arg(QtHelper::appPath()));
    widgetPara.videoFlag = videoWidget->objectName();
    widgetPara.scaleMode = (ScaleMode)AppConfig::ScaleMode;
    widgetPara.videoMode = (VideoMode)AppConfig::VideoMode;
    widgetPara.bannerPosition = (BannerPosition)AppConfig::BannerPosition;
    widgetPara.soundValue = soundValue;
    widgetPara.soundMuted = soundMuted;
    widgetPara.sharedData = AppConfig::SharedData;
    widgetPara.bannerEnable = bannerEnable;

    //设置解码参数
    VideoPara videoPara = videoWidget->getVideoPara();
    videoPara.videoCore = VideoHelper::getVideoCore();
    videoPara.hardware = AppConfig::Hardware;
    videoPara.transport = AppConfig::Transport;
    videoPara.decodeType = (DecodeType)AppConfig::DecodeType;
    videoPara.audioLevel = AppConfig::AudioLevel;
    videoPara.playAudio = AppConfig::PlayAudio;
    videoPara.countKbps = AppConfig::CountKbps;
    videoPara.encodeType = (EncodeType)AppConfig::EncodeType;
    videoPara.encodeVideoRatio = AppConfig::EncodeVideoRatio;
    videoPara.readTimeout = readTimeout;
    videoPara.playRepeat = (readTimeout > 0);
    videoPara.connectTimeout = AppConfig::ConnectTimeout;

    //初始化参数
    VideoHelper::initPara(widgetPara, videoPara);
    videoWidget->setWidgetPara(widgetPara);
    //如果视频控件正在播放则不设置解码参数
    if (!videoWidget->getIsRunning()) {
        videoWidget->setVideoPara(videoPara);
    }
}

void DeviceHelper::playAlarm(const QString &soundName, bool stop)
{
    if (AppConfig::AlarmSoundCount > 0) {
        playSound(soundName, AppConfig::AlarmSoundCount, stop);
    }
}

void DeviceHelper::playSound(const QString &soundName, int playCount, bool stop)
{
    //名称为空或者设置无则不播放
    if (soundName.isEmpty() || soundName.contains("无")) {
        return;
    }

    //设置了先停止
    if (stop) {
        stopSound();
    }

    //重新设置文件全路径
    QString fileName = QString("%1/sound/%2").arg(QtHelper::appPath()).arg(soundName);
    PlayWav::play(fileName, playCount);
}

void DeviceHelper::stopSound()
{
    PlayWav::stop();
}
