#include "frmconfigipcsearch.h"
#include "ui_frmconfigipcsearch.h"
#include "qthelper.h"
#include "onvifhead.h"

frmConfigIpcSearch::frmConfigIpcSearch(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigIpcSearch)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initConfig();
}

frmConfigIpcSearch::~frmConfigIpcSearch()
{
    delete ui;
}

void frmConfigIpcSearch::showEvent(QShowEvent *)
{
    //根据列头宽度自动调整全选框位置
    int width = ui->tableWidget->verticalHeader()->width();
    //ui->tableWidget->verticalHeader()->setFixedWidth(width);
    ckAll->move(width + 9, -3);
}

void frmConfigIpcSearch::initForm()
{
    ui->frameRight->setFixedWidth(AppData::RightWidth);

    //实例化onvif搜索并绑定信号槽
    onvifSearch = new OnvifSearch(this);
    connect(onvifSearch, SIGNAL(receiveError(QString)), this, SLOT(receiveError(QString)));
    connect(onvifSearch, SIGNAL(receiveDevice(OnvifDeviceInfo)), this, SLOT(receiveDevice(OnvifDeviceInfo)));

    //定时器排队获取所有设备onvif信息
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(getMedia()));
    timer->setInterval(300);
}

void frmConfigIpcSearch::initData()
{
    QList<QString> columnNames;
    columnNames << "" << "地址" << "用户名称" << "用户密码" << "厂家" << "设备地址" << "配置文件" << "视频文件" << "主码流" << "子码流";
    QList<int> columnWidths;
    columnWidths << 30 << 100 << 80 << 80 << 80 << 350 << 150 << 150 << 350 << 350;
    ui->tableWidget->setStyleSheet("QCheckBox{padding:0px 0px 0px 7px;}");

    //设置列数和列宽
    int columnCount = columnWidths.count();
    ui->tableWidget->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidget->setColumnWidth(i, columnWidths.at(i));
    }

    //设置行数列名等
    ui->tableWidget->setRowCount(255);
    ui->tableWidget->setHorizontalHeaderLabels(columnNames);
    ui->tableWidget->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    QtHelper::initTableView(ui->tableWidget, AppData::RowHeight, true, false);

    ui->tableWidget->setColumnHidden(1, true);
    ui->tableWidget->setColumnHidden(2, true);
    ui->tableWidget->setColumnHidden(3, true);

    //增加一个全选按钮
    ckAll = new QCheckBox(this);
    connect(ckAll, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    ckAll->setChecked(true);
}

void frmConfigIpcSearch::initConfig()
{
    ui->txtSearchUserName->setText(AppConfig::SearchUserName);
    connect(ui->txtSearchUserName, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtSearchUserPwd->setText(AppConfig::SearchUserPwd);
    connect(ui->txtSearchUserPwd, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    //不在网卡IP地址列表中则取第一个
    QStringList ips = QtHelper::getLocalIPs();
    QString ip = AppConfig::SearchLocalIP;
    if (ips.length() > 0) {
        ip = ips.contains(ip) ? ip : ips.first();
    }
    ui->cboxSearchLocalIP->addItems(ips);
    ui->cboxSearchLocalIP->lineEdit()->setText(ip);
    connect(ui->cboxSearchLocalIP->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    //支持两种格式,一种是直接IP地址一种是onvif地址
    ui->cboxSearchDeviceIP->addItem("192.168.1.128");
    ui->cboxSearchDeviceIP->addItem("http://192.168.1.128/onvif/device_service");
    ui->cboxSearchDeviceIP->lineEdit()->setText(AppConfig::SearchDeviceIP);
    connect(ui->cboxSearchDeviceIP->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->cboxSearchFilter->addItems(AppConfig::SearchFilters.split("|"));
    ui->cboxSearchFilter->lineEdit()->setText(AppConfig::SearchFilter);
    connect(ui->cboxSearchFilter->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList searchInterval, searchIntervalx;
    searchInterval << "数据1" << "数据2" << "数据3" << "数据4";
    searchInterval << "0.3 秒" << "0.5 秒" << "1.0 秒" << "3.0 秒" << "5.0 秒";
    searchIntervalx << "1" << "2" << "3" << "4" << "300" << "500" << "1000" << "3000" << "5000";
    int count = searchInterval.count();
    for (int i = 0; i < count; ++i) {
        ui->cboxSearchInterval->addItem(searchInterval.at(i), searchIntervalx.at(i));
    }

    ui->cboxSearchInterval->setCurrentIndex(ui->cboxSearchInterval->findData(AppConfig::SearchInterval));
    connect(ui->cboxSearchInterval, SIGNAL(activated(int)), this, SLOT(saveConfig()));

    ui->cboxSearchTimeout->addItem("1.0 秒", 1000);
    ui->cboxSearchTimeout->addItem("1.5 秒", 1500);
    ui->cboxSearchTimeout->addItem("3.0 秒", 3000);
    ui->cboxSearchTimeout->addItem("5.0 秒", 5000);
    ui->cboxSearchTimeout->addItem("8.0 秒", 8000);
    ui->cboxSearchTimeout->setCurrentIndex(ui->cboxSearchTimeout->findData(AppConfig::SearchTimeout));
    connect(ui->cboxSearchTimeout, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    OnvifRequest::timeout = AppConfig::SearchTimeout;

    ui->cboxSearchClear->setCurrentIndex(AppConfig::SearchClear ? 0 : 1);
    connect(ui->cboxSearchClear, SIGNAL(activated(int)), this, SLOT(saveConfig()));
}

void frmConfigIpcSearch::saveConfig()
{
    AppConfig::SearchUserName = ui->txtSearchUserName->text().trimmed();
    AppConfig::SearchUserPwd = ui->txtSearchUserPwd->text().trimmed();
    AppConfig::SearchLocalIP = ui->cboxSearchLocalIP->lineEdit()->text().trimmed();
    AppConfig::SearchDeviceIP = ui->cboxSearchDeviceIP->lineEdit()->text().trimmed();
    AppConfig::SearchFilter = ui->cboxSearchFilter->lineEdit()->text().trimmed();
    AppConfig::SearchInterval = ui->cboxSearchInterval->itemData(ui->cboxSearchInterval->currentIndex()).toInt();
    AppConfig::SearchTimeout = ui->cboxSearchTimeout->itemData(ui->cboxSearchTimeout->currentIndex()).toInt();
    OnvifRequest::timeout = AppConfig::SearchTimeout;
    AppConfig::SearchClear = (ui->cboxSearchClear->currentIndex() == 0);
    AppConfig::writeConfig();
}

void frmConfigIpcSearch::clear()
{
    if (AppConfig::SearchClear) {
        qDeleteAll(devices);
        devices.clear();
        ui->tableWidget->clearContents();
    }
}

void frmConfigIpcSearch::stateChanged(int arg1)
{
    for (int row = 0; row < devices.count(); row++) {
        QCheckBox *itemCk = (QCheckBox *)ui->tableWidget->cellWidget(row, 0);
        if (itemCk) {
            itemCk->setChecked(arg1 != 0);
        }
    }
}

void frmConfigIpcSearch::disableRow(const QString &addr, int row)
{
    //过滤已经添加设备,不同背景显示以便区分
    //目前发现最新版的样式表可能冲突了,下面代码不生效
    QColor textColor = GlobalConfig::TextColor;
    QColor bgColor = GlobalConfig::DarkColorEnd;
    if (DbData::IpcInfo_OnvifAddr.contains(addr)) {
        int columnCount = ui->tableWidget->columnCount();
        for (int i = 0; i < columnCount; ++i) {
            QTableWidgetItem *item = ui->tableWidget->item(row, i);
            item->setFlags(Qt::NoItemFlags);
            item->setForeground(textColor);
            item->setBackground(bgColor);
        }
    }
}

void frmConfigIpcSearch::receiveError(const QString &data)
{
    QtHelper::showMessageBoxError(data);
}

void frmConfigIpcSearch::receiveDevice(const OnvifDeviceInfo &deviceInfo)
{
    //如果已经存在则不添加
    foreach (OnvifDevice *device, devices) {
        if (device->getOnvifAddr() == deviceInfo.onvifAddr) {
            return;
        }
    }

    //qDebug() << deviceInfo;
    QString addr = deviceInfo.onvifAddr;
    QString ip = deviceInfo.deviceIp;

    QTableWidgetItem *itemName = new QTableWidgetItem(deviceInfo.name);
    QTableWidgetItem *itemAddr = new QTableWidgetItem(deviceInfo.onvifAddr);

    //复选框,如果勾选了全选则自动选中当前设备
    QCheckBox *itemCk = new QCheckBox(this);
    itemCk->setChecked(ckAll->isChecked());

    //取IP地址转成整型作为排序依据
    quint32 ipAddr = QtHelper::ipv4StringToInt(ip);
    QTableWidgetItem *itemIPAddr = new QTableWidgetItem;
    itemIPAddr->setData(Qt::DisplayRole, ipAddr);

    //没有值的先要放个空的 QTableWidgetItem 防止下面判断失败
    int row = devices.count();
    ui->tableWidget->setCellWidget(row, 0, itemCk);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 1, itemIPAddr);
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 4, itemName);
    ui->tableWidget->setItem(row, 5, itemAddr);
    ui->tableWidget->setItem(row, 6, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 7, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 8, new QTableWidgetItem);
    ui->tableWidget->setItem(row, 9, new QTableWidgetItem);

    //重新排序
    ui->tableWidget->sortByColumn(1, Qt::AscendingOrder);

    //实例化设备对象添加到设备队列
    OnvifDevice *device = new OnvifDevice(this);
    //设置onvif地址
    device->setOnvifAddr(addr);
    //将当前广播搜索返回的设备信息一起打包发给设备类
    device->setDeviceInfo(deviceInfo);
    devices << device;

    //测试下来发现很多设备的时候后面还有陆陆续续回来的
    ui->labCount->setText(QString("共搜索到 %1 个设备").arg(devices.count()));
}

void frmConfigIpcSearch::searchFinsh()
{
    ui->frameRight->setEnabled(true);
    ui->labCount->setText(QString("共搜索到 %1 个设备").arg(devices.count()));
}

void frmConfigIpcSearch::searchDevice(bool one, int interval)
{
    if (interval <= 0) {
        //计算延时时间
        interval = AppConfig::SearchInterval * 4 + 1000;
        //限定最小结束时间 有时候设备很多需要一定时间排队处理数据
        interval = interval < 3000 ? 3000 : interval;
        //单播搜索必须自动清空
        if (one) {
            AppConfig::SearchClear = true;
            interval = 1000;
        }
    }

    //清空数据
    clear();

    //设置搜索过滤条件和间隔
    QString localIP = ui->cboxSearchLocalIP->currentText();
    QString deviceIP = ui->cboxSearchDeviceIP->currentText();
    onvifSearch->setSearchFilter(AppConfig::SearchFilter);
    onvifSearch->setSearchInterval(AppConfig::SearchInterval);
    onvifSearch->search(localIP, one ? deviceIP : "");

    //禁用后延时恢复
    ui->frameRight->setEnabled(false);
    ui->labCount->setText(QString("正在搜索, 请稍等 %1 秒...").arg(interval / 1000));
    QTimer::singleShot(interval, this, SLOT(searchFinsh()));
}

void frmConfigIpcSearch::on_btnSearchAll_clicked()
{
    searchDevice(false);
}

void frmConfigIpcSearch::on_btnSearchOne_clicked()
{
    searchDevice(true);
}

void frmConfigIpcSearch::addUser(QString &rtspAddr, const QString &userName, const QString &userPwd)
{
    QString userInfo = QString("rtsp://%1:%2@").arg(userName).arg(userPwd);
    rtspAddr.replace("rtsp://", userInfo);
}

void frmConfigIpcSearch::getMedia()
{
    if (listRow.count() == 0) {
        ui->frameRight->setEnabled(true);
        timer->stop();
        return;
    }

    //指定行获取详细信息
    int row = listRow.takeFirst();
    OnvifDevice *device = listDevice.takeFirst();
    getMedia(row, device);
}

void frmConfigIpcSearch::getMedia(int row, OnvifDevice *device)
{
    //先设置用户名和密码
    QString userName = ui->txtSearchUserName->text().trimmed();
    QString userPwd = ui->txtSearchUserPwd->text().trimmed();
    device->setUserInfo(userName, userPwd);

    //先获取日期时间(鉴权的时候需要带上设备的日期时间)
    device->getDateTime();

    //厂家为空则先获取设备信息(有时候单播后没有拿到设备信息需要主动重新获取)
    QString manufacturer = ui->tableWidget->item(row, 4)->text();
    if (manufacturer.isEmpty()) {
        OnvifDeviceInfo info = device->getDeviceInfo();
        ui->tableWidget->item(row, 4)->setText(info.manufacturer);
    }

    //如果没有获取到地址则重新发送另外一种请求数据
    device->getServices();
    if (device->getMediaUrl().isEmpty() || device->getPtzUrl().isEmpty()) {
        device->getCapabilities();
    }

    //获取配置文件profile
    QList<OnvifProfileInfo> profiles = device->getProfiles();
    int countProfile = profiles.count();
    if (countProfile == 0) {
        QString ip = OnvifHelper::getUrlIP(device->getOnvifAddr());
        QtHelper::showMessageBoxError(QString("设备 %1 没有配置文件, 请仔细检查!").arg(ip), 3);
        return;
    }

    QString profileToken, rtspMain, rtspSub;
    //如果有第二个则为子码流
    if (countProfile > 1) {
        profileToken = profiles.at(1).token;
        rtspSub = device->getStreamUri(profileToken);
        addUser(rtspSub, userName, userPwd);
    }

    //为什么将第一个放在后面处理,因为默认profileToken取第一个更合理
    if (countProfile > 0) {
        profileToken = profiles.at(0).token;
        rtspMain = device->getStreamUri(profileToken);
        addUser(rtspMain, userName, userPwd);
    }

    //获取视频文件
    QList<OnvifVideoSource> videoSources = device->getVideoSources();
    QString videoSource = videoSources.first().token;

#if 1
    //如果有多个视频源则一般是NVR(也有部分相机同时带两路视频流)
    int countVideo = videoSources.count();
    if (countVideo > 1) {
        //取出每个通道的profile和码流地址,不同厂家不同格式,可以根据实际需求进行调整
        QStringList listProfileToken, listVideoSource, listRtspMain, listRtspSub;
        QString company = ui->tableWidget->item(row, 4)->text().toUpper();
        if (company == "DAHUA") {
            //大华格式: profile 通道1 MediaProfile00000 - 00001 - 00002
            //大华格式: profile 通道2 MediaProfile00100 - 00101 - 00102
            //大华格式: videosource 00000 - 00100 - 00200
            //有部分设备是一个通道对应2个配置/需要计算下改成自适应
            int count = 0;
            for (int i = 0; i < countProfile; i++) {
                QString token = profiles.at(i).token;
                if (token.startsWith("MediaProfile000")) {
                    count++;
                }
            }

            for (int i = 0; i < countProfile; i = i + count) {
                listProfileToken << profiles.at(i).token;
                QString rtspMain = device->getStreamUri(profiles.at(i).token);
                QString rtspSub = device->getStreamUri(profiles.at(i + 1).token);
                addUser(rtspMain, userName, userPwd);
                addUser(rtspSub, userName, userPwd);
                listRtspMain << rtspMain;
                listRtspSub << rtspSub;
            }
        } else if (company == "NETWORK") {
            //海康格式: profile 通道1 ProfileToken001 - 017
            //海康格式: profile 通道2 ProfileToken002 - 018
            //海康格式: videosource VideoSourceToken001 - 002 - 003
            //狗日的海康NVR厂家居然不是叫 HIKVISION 而是叫 NETWORK
            for (int i = 0; i < countVideo; ++i) {
                listProfileToken << profiles.at(i).token;
                QString rtspMain = device->getStreamUri(profiles.at(i).token);
                QString rtspSub = device->getStreamUri(profiles.at(i + countVideo).token);
                addUser(rtspMain, userName, userPwd);
                addUser(rtspSub, userName, userPwd);
                listRtspMain << rtspMain;
                listRtspSub << rtspSub;
            }
        } else {
            //每个通道只有一种码流(有些摄像机设备会有多个镜头多个通道码流)
            for (int i = 0; i < countVideo; ++i) {
                listProfileToken << profiles.at(i).token;
                QString rtspMain = device->getStreamUri(profiles.at(i).token);
                rtspSub = rtspMain;
                addUser(rtspMain, userName, userPwd);
                addUser(rtspSub, userName, userPwd);
                listRtspMain << rtspMain;
                listRtspSub << rtspSub;
            }
        }

        foreach (OnvifVideoSource videoSource, videoSources) {
            listVideoSource << videoSource.token;
        }

        profileToken = listProfileToken.join("|");
        videoSource = listVideoSource.join("|");
        rtspMain = listRtspMain.join("|");
        rtspSub = listRtspSub.join("|");
    }
#endif

    //返回的数据添加到表格中
    //现在临时用用户信息补全明文存储在数据库,后期考虑改成其它方式组合
    QTableWidgetItem *itemUserName = new QTableWidgetItem(userName);
    QTableWidgetItem *itemUserPwd = new QTableWidgetItem(userPwd);
    QTableWidgetItem *itemProfileToken = new QTableWidgetItem(profileToken);
    QTableWidgetItem *itemVideoSource = new QTableWidgetItem(videoSource);
    QTableWidgetItem *itemRtspMain = new QTableWidgetItem(rtspMain);
    QTableWidgetItem *itemRtspSub = new QTableWidgetItem(rtspSub);

    ui->tableWidget->setItem(row, 2, itemUserName);
    ui->tableWidget->setItem(row, 3, itemUserPwd);
    ui->tableWidget->setItem(row, 6, itemProfileToken);
    ui->tableWidget->setItem(row, 7, itemVideoSource);
    ui->tableWidget->setItem(row, 8, itemRtspMain);
    ui->tableWidget->setItem(row, 9, itemRtspSub);

    //判断已经添加过的禁用行
    disableRow(device->getOnvifAddr(), row);
}

void frmConfigIpcSearch::on_btnMediaAll_clicked()
{
    //清空队列
    qDeleteAll(listDevice);
    listRow.clear();
    listDevice.clear();

    int count = devices.count();
    foreach (OnvifDevice *device, devices) {
        //找到当前设备所在行
        QString url = device->getOnvifAddr();
        int row = -1;
        for (int i = 0; i < count; ++i) {
            QString addr = ui->tableWidget->item(i, 5)->text();
            if (url == addr) {
                row = i;
                break;
            }
        }

        if (row < 0) {
            continue;
        }

        //如果当前行已经存在码流地址则不需要继续,加快下一次获取所有的速度,跳过已经获取过的
        QString rtspAddr = ui->tableWidget->item(row, 8)->text();
        if (!rtspAddr.isEmpty()) {
            continue;
        }

        //qDebug() << TIMEMS << row << url;
        //getMedia(row, device);
        //存入队列排队处理,在很多设备的时候全部这里执行并发量太大Qt限制了并发请求5个
        listRow << row;
        listDevice << device;
    }

    ui->frameRight->setEnabled(false);
    timer->start();
}

void frmConfigIpcSearch::on_btnMediaOne_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QtHelper::showMessageBoxError("请先选中设备!", 3);
        return;
    }

    QCheckBox *itemCk = (QCheckBox *)ui->tableWidget->cellWidget(row, 0);
    if (!itemCk) {
        QtHelper::showMessageBoxError("设备地址不能为空!", 3);
        return;
    }

    QString addr = ui->tableWidget->item(row, 5)->text();
    foreach (OnvifDevice *device, devices) {
        if (device->getOnvifAddr() == addr) {
            getMedia(row, device);
            break;
        }
    }
}

void frmConfigIpcSearch::addDevice(int row, bool one)
{
    //先判断是否存在复选框,不存在则说明该行是空的
    QCheckBox *itemCk = (QCheckBox *)ui->tableWidget->cellWidget(row, 0);
    if (!itemCk) {
        return;
    }

    //判断节点是否存在
    QTableWidgetItem *item = ui->tableWidget->item(row, 8);
    if (!item) {
        return;
    }

    //判断主码流地址是否为空
    QString rtspMain = item->text();
    if (rtspMain.isEmpty()) {
        return;
    }

    //判断是否勾选 添加当前则不需要勾选只需要选中就行
    if (!one && !itemCk->isChecked()) {
        return;
    }

    QString userName = ui->tableWidget->item(row, 2)->text();
    QString userPwd = ui->tableWidget->item(row, 3)->text();
    QString ipcType = ui->tableWidget->item(row, 4)->text();
    QString onvifAddr = ui->tableWidget->item(row, 5)->text();
    QString profileToken = ui->tableWidget->item(row, 6)->text();
    QString videoSource = ui->tableWidget->item(row, 7)->text();
    QString rtspSub = ui->tableWidget->item(row, 9)->text();

    //如果有多个则分别插入
    QStringList listProfileToken = profileToken.split("|");
    QStringList listVideoSource = videoSource.split("|");
    QStringList listRtspMain = rtspMain.split("|");
    QStringList listRtspSub = rtspSub.split("|");

    int count = listProfileToken.count();
    for (int i = 0; i < count; ++i) {
        QStringList deviceInfo;
        deviceInfo << userName << userPwd;
        //带上NVR前缀方便判断 NVR_Ch1_Dahua
        if (count > 1) {
            deviceInfo << QString("NVR_Ch%1_%2").arg(i + 1).arg(ipcType);
        } else {
            deviceInfo << ipcType;
        }

        deviceInfo << onvifAddr;
        deviceInfo << listProfileToken.at(i) << listVideoSource.at(i);
        deviceInfo << listRtspMain.at(i) << listRtspSub.at(i);
        deviceInfos << deviceInfo;
    }
}

void frmConfigIpcSearch::on_btnAddAll_clicked()
{
    deviceInfos.clear();
    int count = devices.count();
    for (int row = 0; row < count; ++row) {
        addDevice(row, false);
    }

    emit addDevices(deviceInfos);
}

void frmConfigIpcSearch::on_btnAddOne_clicked()
{
    int row = ui->tableWidget->currentRow();
    if (row < 0) {
        QtHelper::showMessageBoxError("请先选中要添加的设备!", 3);
        return;
    }

    deviceInfos.clear();
    addDevice(row, true);
    emit addDevices(deviceInfos);
    //移到下一个选中
    ui->tableWidget->setCurrentCell(row + 1, 0);
}
