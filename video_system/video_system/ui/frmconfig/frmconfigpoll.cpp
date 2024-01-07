#include "frmconfigpoll.h"
#include "ui_frmconfigpoll.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "dbquery.h"
#include "dbdelegate.h"

frmConfigPoll::frmConfigPoll(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigPoll)
{
    ui->setupUi(this);
    this->initForm();
    this->initIcon();
    this->initData();
}

frmConfigPoll::~frmConfigPoll()
{
    delete ui;
}

void frmConfigPoll::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigPoll::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后立即应用");

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));

    //显示面板以及关联对应的信号槽
    ui->widgetPollPlus->setVisible(AppConfig::VisiblePollPlus);
    connect(ui->widgetPollPlus, SIGNAL(pollGroupChanged()), this, SLOT(pollGroupChanged()));
    connect(ui->widgetPollPlus, SIGNAL(addPlus(QStringList, QStringList)), this, SLOT(addPlus(QStringList, QStringList)));
}

void frmConfigPoll::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->widgetTop);
}

void frmConfigPoll::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "PollInfo");
    //设置列排序
    model->setSort(1, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    columnNames << "编号" << "轮询分组" << "主码流地址" << "子码流地址";
    columnWidths << 40 << 90 << 370 << 370;

    //特殊分辨率重新设置列宽
    int count = columnNames.count();
    if (QtHelper::deskWidth() >= 1920) {
        for (int i = 0; i < count; ++i) {
            columnWidths[i] += 30;
        }
    }

    //挨个设置列名和列宽
    for (int i = 0; i < columnNames.count(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //轮询分组委托
    d_cbox_pollGroup = new DbDelegate(this);
    d_cbox_pollGroup->setDelegateType("QComboBox");
    ui->tableView->setItemDelegateForColumn(1, d_cbox_pollGroup);
    pollGroupChanged();
}

void frmConfigPoll::pollGroupChanged()
{
    QStringList list = AppConfig::PollGroupNames.split("|");
    d_cbox_pollGroup->setDelegateValue(list);
}

void frmConfigPoll::addPlus(const QStringList &rtspMains, const QStringList &rtspSubs)
{
    int count = rtspMains.count();
    if (count == 0) {
        return;
    }

    QString pollGroup = AppConfig::PollGroupLast;
    for (int i = 0; i < count; ++i) {
        //设置新增加的行默认值
        int index = model->rowCount();
        model->insertRow(index);
        model->setData(model->index(index, 1), pollGroup);
        model->setData(model->index(index, 2), rtspMains.at(i));
        model->setData(model->index(index, 3), rtspSubs.at(i));
    }

    on_btnSave_clicked();
}

void frmConfigPoll::on_btnAdd_clicked()
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //获取上一行的对应列的数据
    int row = count - 1;
    int pollID = model->index(row, 0).data().toInt() + 1;
    QString pollGroup = AppConfig::PollGroupLast;
    QString rtspMain = model->index(row, 2).data().toString();
    QString rtspSub = model->index(row, 3).data().toString();

    //当前为第一行时候的默认值
    if (count == 0) {
        pollID = 1;
        rtspMain = "rtsp://192.168.1.128:554/0";
        rtspSub = "rtsp://192.168.1.128:554/1";
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), pollID);
    model->setData(model->index(count, 1), pollGroup);
    model->setData(model->index(count, 2), rtspMain);
    model->setData(model->index(count, 3), rtspSub);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigPoll::on_btnSave_clicked()
{
    //重新设置下焦点避免mac系统上当单元格处于编辑状态保存不成功的bug
    ui->tableView->setFocus();

    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadPollInfo();
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QtHelper::showMessageBoxError("保存信息失败, 请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigPoll::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要删除的行!");
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除该轮询点吗? 删除后不能恢复!") == QMessageBox::Yes) {
        model->removeRow(row);
        model->submitAll();
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
        DbQuery::loadPollInfo();
    }
}

void frmConfigPoll::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigPoll::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有信息吗? 清空后不能恢复!") == QMessageBox::Yes) {
        DbQuery::clearPollInfo();
        DbQuery::loadPollInfo();
        model->select();
    }
}

void frmConfigPoll::on_btnInput_clicked()
{
    FormHelper::inputData(model, columnNames, "PollInfo", "轮询点信息");
    on_btnSave_clicked();
}

void frmConfigPoll::on_btnOutput_clicked()
{
    FormHelper::outputData("PollID asc", columnNames, "PollInfo", "轮询点信息");
}

void frmConfigPoll::dataout(quint8 type)
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "轮询分组" << "主码流地址";
    columnWidths << 50 << 100 << 370;

    QString columns = "PollID,PollGroup,RtspMain";
    QString where = "order by PollID asc";
    FormHelper::dataout(type, columnNames, columnWidths, "轮询点信息", "PollInfo", columns, where);
}

void frmConfigPoll::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigPoll::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigPoll::on_btnPlus_clicked()
{
    ui->widgetPollPlus->setVisible(!ui->widgetPollPlus->isVisible());
    AppConfig::VisiblePollPlus = ui->widgetPollPlus->isVisible();
    AppConfig::writeConfig();
}
