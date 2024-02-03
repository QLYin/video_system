#include "frmconfigipc.h"
#include "ui_frmconfigipc.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "urlhelper.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "commonkey.h"
#include "frmconfigplus.h"

frmConfigIpc::frmConfigIpc(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigIpc)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initIcon();
}

frmConfigIpc::~frmConfigIpc()
{
    delete ui;
}

void frmConfigIpc::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigIpc::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后立即应用");

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));

    if (AppConfig::TableDataPolicy == 1) {
        //设置支持多选
        ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);
        //双击进入编辑
        ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    }

    //显示面板以及关联对应的信号槽
    ui->widgetIpcSearch->setVisible(AppConfig::VisibleIpcSearch);
    connect(ui->widgetIpcSearch, SIGNAL(addDevices(QList<QStringList>)), this, SLOT(addDevices(QList<QStringList>)));

    //关联批量添加窗体信号
    connect(frmConfigPlus::Instance(), SIGNAL(addPlus(QStringList, QStringList)), this, SLOT(addPlus(QStringList, QStringList)));
}

void frmConfigIpc::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->widgetTop);
}

void frmConfigIpc::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "IpcInfo");
    //设置过滤条件
    //model->setFilter("IpcType='http' and IpcID='5'");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    columnNames << "编号" << "名称" << "录像机" << "厂家" << "设备地址" << "配置文件" << "视频文件" << "主码流地址" << "子码流地址"
                << "主码流分辨率" << "子码流分辨率" << "X坐标" << "Y坐标" << "用户姓名" << "用户密码" << "启用" << "备注";
    columnWidths << 40 << 90 << 90 << 80 << 250 << 100 << 100 << 130 << 130
                 << 150 << 90 << 45 << 45 << 80 << 80 << 40 << 60;

    //特殊分辨率重新设置列宽
    int count = columnNames.count();
    if (QtHelper::deskWidth() >= 1920) {
        for (int i = 0; i < count - 2; ++i) {
            columnWidths[i] += 30;
        }

        columnWidths[7] = 350;
        columnWidths[8] = 350;
    }

    //挨个设置列名和列宽
    for (int i = 0; i < count; ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //设置隐藏列
    ui->tableView->setColumnHidden(2, true);
    ui->tableView->setColumnHidden(3, true);
    ui->tableView->setColumnHidden(5, true);
    ui->tableView->setColumnHidden(6, true);
    ui->tableView->setColumnHidden(11, true);
    ui->tableView->setColumnHidden(12, true);
    ui->tableView->setColumnHidden(16, true);

    //录像机委托
    d_cbox_nvrName = new DbDelegate(this);
    d_cbox_nvrName->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(2, d_cbox_nvrName);
    nvrNameChanged();

    //类型委托
    DbDelegate *d_cbox_ipcType = new DbDelegate(this);
    d_cbox_ipcType->setDelegateType("QComboBox");
    d_cbox_ipcType->setDelegateValue(AppData::IpcTypes);
    //ui->tableView->setItemDelegateForColumn(3, d_cbox_ipcType);

    // 主码流分辨率委托
    d_cbox_mainResolution = new DbDelegate(this);
    d_cbox_mainResolution->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(9, d_cbox_mainResolution);
    mainResolutionChanged();

    // 子码流分辨率委托
    d_cbox_subResolution = new DbDelegate(this);
    d_cbox_subResolution->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(10, d_cbox_subResolution);
    subResolutionChanged();

    //用户密码委托
    DbDelegate *d_txt_userPwd = new DbDelegate(this);
    d_txt_userPwd->setDelegateType("QLineEdit");
    d_txt_userPwd->setDelegatePwd(true);
    d_txt_userPwd->setDelegateColumn(14);
    ui->tableView->setItemDelegateForColumn(14, d_txt_userPwd);

    //启用禁用委托
    DbDelegate *d_ckbox_ipcEnable = new DbDelegate(this);
    d_ckbox_ipcEnable->setDelegateColumn(15);
    d_ckbox_ipcEnable->setDelegateType("QCheckBox");
    d_ckbox_ipcEnable->setCheckBoxText("启用", "禁用");
    ui->tableView->setItemDelegateForColumn(15, d_ckbox_ipcEnable);
}

QSqlTableModel* frmConfigIpc::sqlModel()
{
    return model;
}

void frmConfigIpc::nvrNameChanged()
{
    QStringList nvrNames;
    foreach (QString nvrName, DbData::NvrInfo_NvrName) {
        if (!nvrNames.contains(nvrName)) {
            nvrNames << nvrName;
        }
    }

    d_cbox_nvrName->setDelegateValue(nvrNames);
}

void frmConfigIpc::mainResolutionChanged()
{
    QStringList resolutions;
    resolutions << "1200W" << "800W" << "600W" << "500W" << "400W" << "300W" << "1080P" << "960P" << "720P" << "D1" << "CIF";
    d_cbox_mainResolution->setDelegateValue(resolutions);
}

void frmConfigIpc::subResolutionChanged()
{
    QStringList resolutions;
    resolutions << "自动"  << "D1" << "CIF";
    d_cbox_subResolution->setDelegateValue(resolutions);
}

void frmConfigIpc::addDevice(const QStringList &deviceInfo)
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //获取上一行的对应列的数据
    int row = count - 1;
    int ipcID = model->index(row, 0).data().toInt() + 1;
    QString ipcName = model->index(row, 1).data().toString();
    QString nvrName = model->index(row, 2).data().toString();
    QString ipcType = model->index(row, 3).data().toString();
    QString onvifAddr = model->index(row, 4).data().toString();
    QString profileToken = model->index(row, 5).data().toString();
    QString videoSource = model->index(row, 6).data().toString();
    QString rtspMain = model->index(row, 7).data().toString();
    QString rtspSub = model->index(row, 8).data().toString();
    QString mainResolution = model->index(row, 9).data().toString();
    QString subResolution = model->index(row, 10).data().toString();
    int ipcX = model->index(row, 11).data().toInt();
    int ipcY = model->index(row, 12).data().toInt();
    QString userName = model->index(row, 13).data().toString();
    QString userPwd = model->index(row, 14).data().toString();
    QString ipcEnable = model->index(row, 15).data().toString();

    //设备名称自定义递增规则 #后面紧跟序号
    int index = ipcName.indexOf("#");
    if (index >= 0) {
        int len = ipcName.length();
        int number = ipcName.mid(index + 1, len).toInt();
        QString flag = ipcName.mid(0, index);
        ipcName = QString("%1#%2").arg(flag).arg(number + 1);
    }

    //码流地址自定义递增规则(目前限定本地文件)
    index = rtspMain.indexOf("/mp4/");
    if (index >= 0) {
        int len = rtspMain.length();
        QString end = rtspMain.mid(index + 5, len);
        QStringList list = end.split(".");
        int number = list.first().toInt();
        QString flag = rtspMain.mid(0, index);
        rtspMain = QString("%1/mp4/%2.%3").arg(flag).arg(number + 1).arg(list.last());
        rtspSub = rtspMain;
    }

    //设备在图片上的位置自动排列
    FormHelper::checkPosition(ipcX, ipcY, AppData::DeviceWidth, AppData::DeviceHeight);

    //当前为第一行时候的默认值
    if (count == 0) {
        ipcID = 1;
        ipcName = AppConfig::DefaultIpcName;
        //默认取第一台NVR
        nvrName = DbData::NvrInfo_NvrName.first();
        nvrName = nvrName.isEmpty() ? AppConfig::DefaultNvrName : nvrName;
        ipcType = "other";

        rtspMain = "rtsp://192.168.1.128:554/0";
        rtspSub = "rtsp://192.168.1.128:554/1";

        //默认值取中心点
        //ipcPosition = AppConfig::MapCenter;
        //ipcPosition.replace(",", "|");
        mainResolution = "1080P";
        subResolution = "D1";

        //ipcImage = AppData::MapNames.count() > 0 ? AppData::MapNames.first() : " -- 无 -- ";
        ipcX = 5;
        ipcY = 5;
        userName = "admin";
        userPwd = "admin";
        ipcEnable = "启用";
    }

    //如果是批量添加过来的数据
    if (deviceInfo.count() > 7) {
        userName = deviceInfo.at(0);
        userPwd = deviceInfo.at(1);
        ipcType = deviceInfo.at(2);
        onvifAddr = deviceInfo.at(3);
        profileToken = deviceInfo.at(4);
        videoSource = deviceInfo.at(5);
        rtspMain = deviceInfo.at(6);
        rtspSub = deviceInfo.at(7);

        //重新定义搜索的摄像机设备命名规则,按照摄像机#ip地址末尾数字的方式
        QString ip = UrlHelper::getUrlIP(onvifAddr);
        QStringList ips = ip.split(".");
        QString flag = ips.last();
        ipcName = QString("摄像机#%1").arg(flag);
        if (flag.isEmpty()) {
            ipcName = QString("视频文件#%1").arg(ipcID);
        }

        //NVR过来的再区分对应的通道 NVR_Ch1_Dahua
        if (ipcType.startsWith("NVR_Ch")) {
            //编号改成对应NVR地址的整型和3位通道组合(跨网段的现场一般是后两个网段变化)
            QString ch = ipcType.split("_").at(1);
            QString channel = QString("%1").arg(ch.mid(2, 3).toInt(), 3, 10, QChar('0'));
            int addr = (ips.at(3).toInt() | ips.at(2).toInt() << 8 | 0 << 16 | 0 << 24);
            QString id = QString("%1%2").arg(addr).arg(channel);
            ipcID = id.toInt();
            //ipcName = ipcName + "_" + ch;
            ipcName = QString("通道%1").arg(channel);

            //查找对应的NVR名称
            int index = DbData::NvrInfo_NvrIP.indexOf(ip);
            if (index >= 0) {
                nvrName = DbData::NvrInfo_NvrName.at(index);
            } else {
                //查找不到就主动添加这个NVR
                nvrName = QString("录像机#%1").arg(flag);
                QString nvrType = "其他";
                if (ipcType.endsWith("HIKVISION")) {
                    nvrType = "海康";
                } else if (ipcType.endsWith("Dahua")) {
                    nvrType = "大华";
                }

                DbQuery::addNvrInfo(ip, nvrName, nvrType);
                //添加完成后还要立即重新读取下不然会重复添加
                DbQuery::loadNvrInfo();
            }
        }
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), ipcID);
    model->setData(model->index(count, 1), ipcName);
    model->setData(model->index(count, 2), nvrName);
    model->setData(model->index(count, 3), ipcType);
    model->setData(model->index(count, 4), onvifAddr);
    model->setData(model->index(count, 5), profileToken);
    model->setData(model->index(count, 6), videoSource);
    model->setData(model->index(count, 7), rtspMain);
    model->setData(model->index(count, 8), rtspSub);
    model->setData(model->index(count, 9), mainResolution);
    model->setData(model->index(count, 10), subResolution);
    model->setData(model->index(count, 11), ipcX);
    model->setData(model->index(count, 12), ipcY);
    model->setData(model->index(count, 13), userName);
    model->setData(model->index(count, 14), userPwd);
    model->setData(model->index(count, 15), ipcEnable);

    m_appendIpcids.insert(ipcID);
}

void frmConfigIpc::addDevices(const QList<QStringList> &deviceInfos)
{
    //校验是否超过秘钥的数量限制
    //总数量=当前行数+将要添加的设备数量
    int count = model->rowCount() + deviceInfos.count() - 1;
    if (!CommonKey::checkCount(count)) {
        return;
    }

    count = deviceInfos.count();
    for (int i = 0; i < count; ++i) {
        QStringList deviceInfo = deviceInfos.at(i);
        QString onvifAddr = deviceInfo.at(3);
        QString rtspMain = deviceInfo.at(6);

        //过滤已经添加过的设备
        bool exist = false;
        for (int j = 0; j < DbData::IpcInfo_Count; ++j) {
            QString addr = DbData::IpcInfo_OnvifAddr.at(j);
            QString rtsp = DbData::IpcInfo_RtspMain.at(j);
            if (addr == onvifAddr && rtsp == rtspMain) {
                exist = true;
                break;
            }
        }

        if (!exist) {
            addDevice(deviceInfo);
        }
    }

    //单击保存按钮
    on_btnSave_clicked();
}

void frmConfigIpc::addPlus(const QStringList &rtspMains, const QStringList &rtspSubs)
{
    int count = rtspMains.count();
    if (count == 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        //模拟成搜索那边批量添加的形式
        QString rtspMain = rtspMains.at(i);
        QString rtspSub = rtspSubs.at(i);
        QString userName = "admin";
        QString userPwd = "12345";

        int index = rtspMain.lastIndexOf("@");
        if (index > 0) {
            QString userInfo = rtspMain.mid(0, index);
            userInfo.replace("rtsp://", "");
            QStringList list = userInfo.split(":");
            userName = list.at(0);
            userPwd = list.at(1);
        }

        QString ipcType = "other";
        QString ip = UrlHelper::getUrlIP(rtspMain);
        QString onvifAddr = "";
        QString profileToken = "";
        QString videoSource = "";
        if (AppConfig::PlusType == 1) {
            ipcType = "HIKVISION";
            onvifAddr = QString("http://%1/onvif/device_service").arg(ip);
            profileToken = "Profile_1";
            videoSource = "VideoSource_1";
        } else if (AppConfig::PlusType == 2) {
            ipcType = "Dahua";
            onvifAddr = QString("http://%1/onvif/device_service").arg(ip);
            profileToken = "Profile000";
            videoSource = "VideoSource000";
        }

        //通道递增一般是NVR
        if (AppConfig::PlusType != 0 && AppConfig::PlusNvr) {
            ipcType = QString("NVR_Ch%1_%2").arg(i + 1).arg(ipcType);
            if (AppConfig::PlusType == 1) {
                QString flag = QString("%1").arg(i + 1, 3, 10, QChar('0'));
                profileToken = "ProfileToken" + flag;
                videoSource = "VideoSourceToken" + flag;
            } else if (AppConfig::PlusType == 2) {
                QString flag = QString("%100").arg(i, 3, 10, QChar('0'));
                profileToken = "MediaProfile" + flag;
                videoSource = flag;
            }
        }

        QStringList deviceInfo;
        deviceInfo << userName << userPwd << ipcType << onvifAddr << profileToken << videoSource << rtspMain << rtspSub;
        addDevice(deviceInfo);
    }

    //单击保存按钮
    on_btnSave_clicked();
}

void frmConfigIpc::on_btnAdd_clicked()
{
    //校验是否超过秘钥的数量限制
    int count = model->rowCount();
    if (!CommonKey::checkCount(count)) {
        return;
    }

    //调用批量添加设备
    addDevice(QStringList());
    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigIpc::on_btnSave_clicked()
{
    //重新设置下焦点避免mac系统上当单元格处于编辑状态保存不成功的bug
    ui->tableView->setFocus();

    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        AppEvent::Instance()->slot_saveIpcInfo(true);
        
        QList<IpcInfo> ipcList;
        for (auto& value : m_appendIpcids)
        {
            QString key = QString::number(value); // 要查找的键值
            int column = 0; // 假设你要在第一列中查找
            QModelIndexList matches = model->match(model->index(0, column), Qt::DisplayRole, key, -1, Qt::MatchExactly);
            if (!matches.isEmpty()) {
                QModelIndex firstMatchIndex = matches.first();
                int row = firstMatchIndex.row();
                IpcInfo ipcItem;
                ipcItem.init_flag = 255;
                ipcItem.id = model->index(row, 0).data().toInt();
                ipcItem.name = model->index(row, 1).data().toString();
                ipcItem.user = model->index(row, 13).data().toString();
                ipcItem.passwd = model->index(row, 14).data().toString();
                ipcItem.ipaddr = model->index(row, 4).data().toString();
                ipcItem.ptz_enable = 0;
                ipcItem.vda_enable = 0;
                ipcItem.rtsp_url0 = model->index(row, 7).data().toString();
                ipcItem.rtsp_url1 = model->index(row, 8).data().toString();
                ipcItem.resolution0 = model->index(row, 9).data().toInt();
                ipcItem.resolution1 = model->index(row, 10).data().toInt();
                ipcList.append(ipcItem);
            }
        }
        if (!ipcList.isEmpty())
        {
            emit ipcAddSig(ipcList);
        }
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QtHelper::showMessageBoxError("保存信息失败, 请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigIpc::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要删除的行!");
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除选中的摄像机吗?\n摄像机对应的轮询信息都会被删除!") == QMessageBox::Yes) {
        //获取选中行的内容
        QStringList ids, ips, addrs;
        QItemSelectionModel *selections = ui->tableView->selectionModel();
        QModelIndexList selected = selections->selectedIndexes();
        foreach (QModelIndex index, selected) {
            int column = index.column();
            QString text = index.data().toString();
            if (column == 0) {
                ids << text;
            } else if (column == 7) {
                addrs << text;
            }
            else if (column == 4)
            {
                ips << text;
            }
        }

        DbQuery::deleteIpcInfos(ids.join(","));
        DbQuery::deletePollInfos(addrs.join(","));
        AppEvent::Instance()->slot_saveIpcInfo(true);
        model->select();
        emit ipcDelSig(ids, ips);
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
    }
}

void frmConfigIpc::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigIpc::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有信息吗? 清空后不能恢复!") == QMessageBox::Yes) {
        DbQuery::clearIpcInfo();
        AppEvent::Instance()->slot_saveIpcInfo(true);
        model->select();
    }
}

void frmConfigIpc::on_btnInput_clicked()
{
    FormHelper::inputData(model, columnNames, "IpcInfo", "摄像机信息");
    on_btnSave_clicked();
}

void frmConfigIpc::on_btnOutput_clicked()
{
    FormHelper::outputData("IpcID asc", columnNames, "IpcInfo", "摄像机信息");
}

void frmConfigIpc::dataout(quint8 type)
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "名称" << "录像机" << "厂家" << "启用" << "备注";
    columnWidths << 50 << 120 << 120 << 150 << 60 << 100;

    QString columns = "IpcID,IpcName,NvrName,IpcType,IpcEnable,IpcMark";
    QString where = "order by IpcID asc";
    FormHelper::dataout(type, columnNames, columnWidths, "摄像机信息", "IpcInfo", columns, where);
}

void frmConfigIpc::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigIpc::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigIpc::on_btnPlus_clicked()
{
    frmConfigPlus::Instance()->show();
}

void frmConfigIpc::on_btnSearch_clicked()
{
    ui->widgetIpcSearch->setVisible(!ui->widgetIpcSearch->isVisible());
    AppConfig::VisibleIpcSearch = ui->widgetIpcSearch->isVisible();
    AppConfig::writeConfig();
}
