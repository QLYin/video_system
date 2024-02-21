#include "frmdatauser.h"
#include "ui_frmdatauser.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "dbpage.h"
#include "dbquery.h"
#include "dbdelegate.h"
#include "frmvideowidgetslider.h"

frmDataUser::frmDataUser(QWidget *parent) : QWidget(parent), ui(new Ui::frmDataUser)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initIcon();
    on_btnSelect_clicked();
}

frmDataUser::~frmDataUser()
{
    delete ui;
}

void frmDataUser::initForm()
{
    ui->frameRight->setFixedWidth(AppData::RightWidth);

    for (int i = 1; i <= AppConfig::VideoCount; ++i) {
        ui->cboxLogCh->addItem(QString("通道 %1").arg(i, 2, 10, QChar('0')));
    }

    QStringList listType;
    listType << "运行日志" << "设备日志" << "报警日志" << "用户操作";
    ui->cboxLogType->addItems(listType);

    QtHelper::initDataTimeEdit(ui->dateStart, 0);
    QtHelper::initDataTimeEdit(ui->dateEnd, 1);

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmDataUser::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);

    //设置图标
    QPixmap pixOk = IconHelper::getPixmap(GlobalConfig::TextColor, 0xf071, 15, 15, 15);
    QPixmap pixNo = IconHelper::getPixmap(GlobalConfig::TextColor, 0xf075, 15, 15, 15);
    d_pixmap->setCheckOkImage(pixOk);
    d_pixmap->setCheckNoImage(pixNo);
}

void frmDataUser::initData()
{
    whereSql = "where 1=1";
    columnNames << "序号" << "时间" << "用户" << "通道" << "类型" << "内容" << "备注";
    columnWidths << 60 << 160 << 120 << 60 << 100 << 160 << 90;

    //实例化翻页类
    dbPage = new DbPage(this);
    //关联查询记录总数
    connect(DbData::DbLocal, SIGNAL(receiveCount(QString, int, int)), dbPage, SLOT(receiveCount(QString, int, int)));

    //设置插入一列用于图标显示对应的记录报警级别
    dbPage->setInsertColumnIndex(0);
    dbPage->setInsertColumnName("标识");
    //dbPage->setAllCenter(true);
    dbPage->setAlignCenterColumn(QList<int>() << 0 << 1 << 2 << 3 << 4 << 5);
    dbPage->setColumnNames(columnNames);
    dbPage->setColumnWidths(columnWidths);
    dbPage->setRecordsPerpage(AppConfig::RecordsPerpage);
    dbPage->setDbType(AppConfig::LocalDbType);
    dbPage->setTableName("LogInfo");
    dbPage->setOrderSql(QString("LogID %1").arg("desc"));

    //分页导航控件
    ui->navPage->setShowGoPage(true);
    connect(ui->navPage, SIGNAL(selectPage(int)), dbPage, SLOT(selectPage(int)));
    connect(dbPage, SIGNAL(receivePage(quint32, quint32, quint32, quint32)),
            ui->navPage, SLOT(receivePage(quint32, quint32, quint32, quint32)));
    QLabel *labInfo = ui->navPage->getLabInfo();
    QList<QPushButton *> btns = ui->navPage->getBtnAll();
    dbPage->setControl(ui->tableView, 0, 0, 0, 0, 0, labInfo, btns.at(0), btns.at(1), btns.at(2), btns.at(3));

    //设置第一列委托,自动根据内容显示不同图标
    d_pixmap = new DbDelegate(this);
    d_pixmap->setDelegateColumn(0);
    d_pixmap->setDelegateType("QPixmap");
    d_pixmap->setCheckColumn(6);
    d_pixmap->setCheckType("contains");
    d_pixmap->setCheckValue("离线|故障|报警|错误|失败");
    ui->tableView->setItemDelegateForColumn(0, d_pixmap);
}

void frmDataUser::on_btnSelect_clicked()
{
    //构建SQL语句
    QString sql = "where 1=1";
    if (ui->ckTimeStart->isChecked()) {
        QDateTime dateStart = ui->dateStart->dateTime();
        QDateTime dateEnd = ui->dateEnd->dateTime();
        if (dateStart > dateEnd) {
            QtHelper::showMessageBoxError("开始时间不能大于结束时间!", 3);
            return;
        }

        QString timeStart = dateStart.toString("yyyy-MM-dd 00:00:00");
        QString timeEnd = dateEnd.toString("yyyy-MM-dd 23:59:59");
        DbHelper::getBetweenDate(sql, AppConfig::LocalDbType, "TriggerTime", timeStart, timeEnd);
    }

    if (ui->ckLogCh->isChecked()) {
        sql += " and LogCh='" + ui->cboxLogCh->currentText().right(2) + "'";
    }

    if (ui->ckLogType->isChecked()) {
        sql += " and LogType='" + ui->cboxLogType->currentText() + "'";
    }

    //绑定数据到表格
    whereSql = sql;
    dbPage->setWhereSql(whereSql);
    dbPage->select();
    DbData::DbLocal->selectCount("LogInfo", "LogID", whereSql);
}

void frmDataUser::on_btnDelete_clicked()
{
    if (!UserHelper::checkPermission("删除记录")) {
        return;
    }

    QString timeStart, timeEnd;
    if (QtHelper::showDateSelect(timeStart, timeEnd) == QMessageBox::Ok) {
        if (QtHelper::showMessageBoxQuestion("确定要删除该时间段的数据吗?") == QMessageBox::Yes) {
            DbQuery::deleteUserLog(timeStart, timeEnd);
            QString msg = "删除操作记录成功";
            DbQuery::addUserLog(msg);
            QtHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

            //重新查询数据
            on_btnSelect_clicked();
        }
    }
}

void frmDataUser::on_btnClear_clicked()
{
    if (!UserHelper::checkPermission("删除记录")) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有数据吗?") == QMessageBox::Yes) {
        DbQuery::clearUserLog();
        QString msg = "清空操作记录成功";
        DbQuery::addUserLog(msg);
        QtHelper::showMessageBoxInfo(QString("%1!").arg(msg), 3);

        //重新查询数据
        on_btnSelect_clicked();
    }
}

void frmDataUser::dataout(quint8 type)
{
    //先判断行数,超过一定大小弹出提示
    if (!FormHelper::checkRowCount(type, dbPage->getRecordsTotal())) {
        return;
    }

    //设定导出数据字段及排序字段
    QString columns = "*";
    QString where = whereSql + " order by " + QString("LogID %1").arg("desc");
    FormHelper::dataout(type, columnNames, columnWidths, "本地日志", "LogInfo", columns, where, true, true);
}

void frmDataUser::on_btnPrint_clicked()
{
    dataout(0);
}

void frmDataUser::on_btnPdf_clicked()
{
    dataout(1);
}

void frmDataUser::on_btnXls_clicked()
{
    dataout(2);
}

void frmDataUser::on_ckTimeStart_stateChanged(int arg1)
{
    ui->ckTimeEnd->setChecked(arg1 != 0);
}

void frmDataUser::on_ckTimeEnd_stateChanged(int arg1)
{
    ui->ckTimeStart->setChecked(arg1 != 0);
}

void frmDataUser::on_tableView_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if (row < 0) {
        return;
    }

    //双击列表弹出报警图片或者报警视频
    QString url = index.model()->index(row, 7).data().toString();
    if (!url.isEmpty() && QFile(url).exists()) {
        if (url.endsWith(".mp4")) {
            DeviceHelper::showAlarmVideo(url);
        } else {
            DeviceHelper::showAlarmImage(url);
        }
    }
}
