#include "frmconfignvr.h"
#include "ui_frmconfignvr.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "dbquery.h"
#include "dbdelegate.h"

frmConfigNvr::frmConfigNvr(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigNvr)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initIcon();
}

frmConfigNvr::~frmConfigNvr()
{
    delete ui;
}

void frmConfigNvr::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigNvr::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后立即应用");

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmConfigNvr::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->widgetTop);
}

void frmConfigNvr::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "NvrInfo");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    columnNames << "编号" << "设备名称" << "设备类型" << "设备地址" << "用户姓名" << "用户密码" << "启用" << "备注";
    columnWidths << 40 << 120 << 80 << 130 << 100 << 130 << 40 << 60;

    //特殊分辨率重新设置列宽
    int count = columnNames.count();
    if (QtHelper::deskWidth() >= 1920) {
        for (int i = 0; i < count - 2; ++i) {
            columnWidths[i] += 30;
        }
    }

    //挨个设置列名和列宽
    for (int i = 0; i < count; ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //类型委托
    DbDelegate *d_cbox_nvrType = new DbDelegate(this);
    d_cbox_nvrType->setDelegateType("QComboBox");
    d_cbox_nvrType->setDelegateValue(AppData::NvrTypes);
    ui->tableView->setItemDelegateForColumn(2, d_cbox_nvrType);

    //用户密码委托
    DbDelegate *d_txt_userPwd = new DbDelegate(this);
    d_txt_userPwd->setDelegateType("QLineEdit");
    d_txt_userPwd->setDelegatePwd(true);
    d_txt_userPwd->setDelegateColumn(5);
    ui->tableView->setItemDelegateForColumn(5, d_txt_userPwd);

    //启用禁用委托
    DbDelegate *d_ckbox_nvrEnable = new DbDelegate(this);
    d_ckbox_nvrEnable->setDelegateColumn(6);
    d_ckbox_nvrEnable->setDelegateType("QCheckBox");
    d_ckbox_nvrEnable->setCheckBoxText("启用", "禁用");
    ui->tableView->setItemDelegateForColumn(6, d_ckbox_nvrEnable);
}

void frmConfigNvr::on_btnAdd_clicked()
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //获取上一行的对应列的数据
    int row = count - 1;
    int nvrID = model->index(row, 0).data().toInt() + 1;
    QString nvrName = model->index(row, 1).data().toString();
    QString nvrType = model->index(row, 2).data().toString();
    QString nvrIP = model->index(row, 3).data().toString();
    QString userName = model->index(row, 4).data().toString();
    QString userPwd = model->index(row, 5).data().toString();
    QString nvrEnable = model->index(row, 6).data().toString();

    if (nvrName.startsWith("录像机#")) {
        nvrName = QString("录像机#%1").arg(nvrID);
    }

    //当前为第一行时候的默认值
    if (count == 0) {
        nvrID = 1;
        nvrName = AppConfig::DefaultNvrName;
        nvrType = "海康";
        nvrIP = "192.168.1.128";
        userName = "admin";
        userPwd = "admin";
        nvrEnable = "启用";
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), nvrID);
    model->setData(model->index(count, 1), nvrName);
    model->setData(model->index(count, 2), nvrType);
    model->setData(model->index(count, 3), nvrIP);
    model->setData(model->index(count, 4), userName);
    model->setData(model->index(count, 5), userPwd);
    model->setData(model->index(count, 6), nvrEnable);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigNvr::on_btnSave_clicked()
{
    //重新设置下焦点避免mac系统上当单元格处于编辑状态保存不成功的bug
    ui->tableView->setFocus();

    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadNvrInfo();
        emit nvrNameChanged();
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QtHelper::showMessageBoxError("保存信息失败, 请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigNvr::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要删除的行!");
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除该录像机吗? 录像机对应的所有摄像机都会被删除!") == QMessageBox::Yes) {
        QString nvrName = model->index(row, 1).data().toString();
        DbQuery::deleteIpcInfo(nvrName);
        DbQuery::loadIpcInfo();

        model->removeRow(row);
        model->submitAll();
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
        DbQuery::loadNvrInfo();
    }
}

void frmConfigNvr::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigNvr::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有信息吗? 清空后不能恢复!") == QMessageBox::Yes) {
        DbQuery::clearNvrInfo();
        DbQuery::loadNvrInfo();
        model->select();
    }
}

void frmConfigNvr::on_btnInput_clicked()
{
    FormHelper::inputData(model, columnNames, "NvrInfo", "录像机信息");
    on_btnSave_clicked();
}

void frmConfigNvr::on_btnOutput_clicked()
{
    FormHelper::outputData("NvrID asc", columnNames, "NvrInfo", "录像机信息");
}

void frmConfigNvr::dataout(quint8 type)
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "设备名称" << "设备类型" << "设备地址" << "启用" << "备注";
    columnWidths << 50 << 150 << 120 << 150 << 60 << 100;

    QString columns = "NvrID,NvrName,NvrType,NvrIP,NvrEnable,NvrMark";
    QString where = "order by NvrID asc";
    FormHelper::dataout(type, columnNames, columnWidths, "录像机信息", "NvrInfo", columns, where);
}

void frmConfigNvr::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigNvr::on_btnXls_clicked()
{
    dataout(2);
}
