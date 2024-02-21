#include "frmconfiguser.h"
#include "ui_frmconfiguser.h"
#include "qthelper.h"
#include "dbquery.h"
#include "dbdelegate.h"

frmConfigUser::frmConfigUser(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigUser)
{
    ui->setupUi(this);
    this->initForm();    
}

frmConfigUser::~frmConfigUser()
{
    delete ui;
}

void frmConfigUser::showEvent(QShowEvent *)
{
    //保证只加载一次
    if (!isLoad) {
        isLoad = true;
        this->initData();
        this->initIcon();
    }

    model->select();
}

void frmConfigUser::setDelegate(const QList<int> &delegateWidth, const QList<QString> &delegateType, const QList<QStringList> &delegateValue)
{
    this->delegateWidth = delegateWidth;
    this->delegateType = delegateType;
    this->delegateValue = delegateValue;
}

void frmConfigUser::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后重启应用");

    //默认委托类型和值
    isLoad = false;
    for (int i = 0; i < UserHelper::PermissionName.count(); ++i) {
        delegateWidth << 80;
        delegateType << "QCheckBox";
        delegateValue << QStringList();
    }

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, true);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmConfigUser::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->widgetTop);
}

void frmConfigUser::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "UserInfo");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    columnNames << "用户名称" << "用户密码" << "用户类型";
    columnWidths << 100 << 120 << 80;

    columnNames << UserHelper::PermissionName;
    for (int i = 0; i < UserHelper::PermissionName.count(); ++i) {
        columnWidths << delegateWidth.at(i);
    }

    columnNames << "备注";
    columnWidths << 100;

    //特殊分辨率重新设置列宽
    int count = columnNames.count();
    if (QtHelper::deskWidth() >= 1920) {
        for (int i = 0; i < count; ++i) {
            //columnWidths[i] += 30;
        }
    }

    //挨个设置列名和列宽
    for (int i = 0; i < count; ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }

    //用户密码委托
    DbDelegate *d_txt_userPwd = new DbDelegate(this);
    d_txt_userPwd->setDelegateType("QLineEdit");
    d_txt_userPwd->setDelegatePwd(true);
    d_txt_userPwd->setDelegateColumn(1);
    ui->tableView->setItemDelegateForColumn(1, d_txt_userPwd);

    //用户类型委托
    QStringList userType;
    userType << "操作员" << "管理员";
    DbDelegate *d_cbox_userType = new DbDelegate(this);
    d_cbox_userType->setDelegateType("QComboBox");
    d_cbox_userType->setDelegateValue(userType);
    ui->tableView->setItemDelegateForColumn(2, d_cbox_userType);

    //用户权限委托
    int index = 3;
    count = UserHelper::PermissionName.count() + index;
    for (int i = index; i < count; ++i) {
        //取出委托类型和值
        QString type = delegateType.at(i - index);
        QStringList value = delegateValue.at(i - index);
        DbDelegate *delegate = new DbDelegate(this);
        delegate->setDelegateType(type);

        if (type == "QCheckBox") {
            delegate->setDelegateColumn(i);
            delegate->setCheckBoxText("启用", "禁用");
        } else if (type == "QComboBox") {
            delegate->setDelegateValue(value);
        }

        ui->tableView->setItemDelegateForColumn(i, delegate);
    }
}

void frmConfigUser::on_btnAdd_clicked()
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //规则清晰直接用循环来赋值
    int columns = columnNames.count();
    for (int i = 0; i < columns; ++i) {
        //获取上一行的对应列的数据
        QString data = model->index(count - 1, i).data().toString();
        //设置新增加的行默认值
        model->setData(model->index(count, i), data);
    }

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigUser::on_btnSave_clicked()
{
    //重新设置下焦点避免mac系统上当单元格处于编辑状态保存不成功的bug
    ui->tableView->setFocus();

    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QtHelper::showMessageBoxError("保存信息失败, 请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigUser::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要删除的行!");
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除该用户吗? 删除后不能恢复!") == QMessageBox::Yes) {
        QString userName = model->index(row, 0).data().toString();
        if (userName == "admin") {
            QtHelper::showMessageBoxError("管理员 [admin] 不能被删除!", 3);
            return;
        }

        model->removeRow(row);
        model->submitAll();
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
    }
}

void frmConfigUser::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigUser::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有信息吗? 清空后不能恢复!") == QMessageBox::Yes) {
        UserHelper::clearUserInfo();
        model->select();
    }
}

void frmConfigUser::on_btnInput_clicked()
{
    FormHelper::inputData(model, columnNames, "UserInfo", "用户信息");
    on_btnSave_clicked();
}

void frmConfigUser::on_btnOutput_clicked()
{
    FormHelper::outputData("UserName asc", columnNames, "UserInfo", "用户信息");
}

void frmConfigUser::dataout(quint8 type)
{
    QString columns = "*";
    QString where = "order by UserName asc";
    FormHelper::dataout(type, columnNames, columnWidths, "用户信息", "UserInfo", columns, where, true);
}

void frmConfigUser::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigUser::on_btnXls_clicked()
{
    dataout(2);
}

