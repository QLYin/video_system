#include "frmconfigrecord.h"
#include "ui_frmconfigrecord.h"
#include "qthelper.h"
#include "dbquery.h"

frmConfigRecord::frmConfigRecord(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigRecord)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initIcon();
}

frmConfigRecord::~frmConfigRecord()
{
    delete ui;
}

void frmConfigRecord::initForm()
{
    ui->widgetTop->setProperty("flag", "navbtn");
    ui->labTip->setText("提示 → 改动后立即应用");

    //初始化通用的表格属性
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, false);
    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
}

void frmConfigRecord::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->widgetTop);
}

void frmConfigRecord::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "RecordInfo");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    columnNames << "通道" << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六" << "星期日" << "备注";
    columnWidths << 40 << 130 << 130 << 130 << 130 << 130 << 130 << 130 << 40;

    //挨个设置列名和列宽
    int count = columnNames.count();
    for (int i = 0; i < count; ++i) {
        model->setHeaderData(i, Qt::Horizontal, columnNames.at(i));
        ui->tableView->setColumnWidth(i, columnWidths.at(i));
    }
}

void frmConfigRecord::on_btnAdd_clicked()
{
    //插入一行
    int count = model->rowCount();
    model->insertRow(count);

    //获取上一行的对应列的数据
    int row = count - 1;
    int recordCh = model->index(row, 0).data().toInt() + 1;
    QString recordWeek1 = model->index(row, 1).data().toString();
    QString recordWeek2 = model->index(row, 2).data().toString();
    QString recordWeek3 = model->index(row, 3).data().toString();
    QString recordWeek4 = model->index(row, 4).data().toString();
    QString recordWeek5 = model->index(row, 5).data().toString();
    QString recordWeek6 = model->index(row, 6).data().toString();
    QString recordWeek7 = model->index(row, 7).data().toString();

    //当前为第一行时候的默认值
    if (count == 0) {
        recordCh = 1;
        QStringList list;
        for (int i = 0; i < 48; ++i) {
            list << "1";
        }

        QString recordWeek = list.join(",");
        recordWeek1 = recordWeek;
        recordWeek2 = recordWeek;
        recordWeek3 = recordWeek;
        recordWeek4 = recordWeek;
        recordWeek5 = recordWeek;
        recordWeek6 = recordWeek;
        recordWeek7 = recordWeek;
    }

    //设置新增加的行默认值
    model->setData(model->index(count, 0), recordCh);
    model->setData(model->index(count, 1), recordWeek1);
    model->setData(model->index(count, 2), recordWeek2);
    model->setData(model->index(count, 3), recordWeek3);
    model->setData(model->index(count, 4), recordWeek4);
    model->setData(model->index(count, 5), recordWeek5);
    model->setData(model->index(count, 6), recordWeek6);
    model->setData(model->index(count, 7), recordWeek7);

    //立即选中当前新增加的行
    ui->tableView->setCurrentIndex(model->index(count, 0));
}

void frmConfigRecord::on_btnSave_clicked()
{
    //重新设置下焦点避免mac系统上当单元格处于编辑状态保存不成功的bug
    ui->tableView->setFocus();

    //开启数据库事务提交数据
    model->database().transaction();
    if (model->submitAll()) {
        model->database().commit();
        DbQuery::loadRecordInfo();
        //立即发送给监控管理类重置录像计划
        AppEvent::Instance()->slot_changeVideoManage();
    } else {
        //提交失败则回滚事务并打印错误信息
        model->database().rollback();
        qDebug() << TIMEMS << model->database().lastError();
        QtHelper::showMessageBoxError("保存信息失败, 请重新填写!");
    }

    //有些数据库需要主动查询一下不然是空白的比如odbc数据源
    model->select();
}

void frmConfigRecord::on_btnDelete_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要删除的行!");
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要删除该录像计划吗? 删除后不能恢复!") == QMessageBox::Yes) {
        model->removeRow(row);
        model->submitAll();
        ui->tableView->setCurrentIndex(model->index(model->rowCount() - 1, 0));
        DbQuery::loadRecordInfo();
        //立即发送给监控管理类重置录像计划
        AppEvent::Instance()->slot_changeVideoManage();
    }
}

void frmConfigRecord::on_btnReturn_clicked()
{
    model->revertAll();
}

void frmConfigRecord::on_btnClear_clicked()
{
    if (model->rowCount() <= 0) {
        return;
    }

    if (QtHelper::showMessageBoxQuestion("确定要清空所有信息吗? 清空后不能恢复!") == QMessageBox::Yes) {
        DbQuery::clearRecordInfo();
        DbQuery::loadRecordInfo();
        //立即发送给监控管理类重置录像计划
        AppEvent::Instance()->slot_changeVideoManage();
        //重新查询数据
        model->select();
    }
}

void frmConfigRecord::on_btnInput_clicked()
{
    FormHelper::inputData(model, columnNames, "RecordInfo", "录像计划信息");
    on_btnSave_clicked();
}

void frmConfigRecord::on_btnOutput_clicked()
{
    FormHelper::outputData("RecordCh asc", columnNames, "RecordInfo", "录像计划信息");
}

void frmConfigRecord::dataout(quint8 type)
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六" << "星期日" << "备注";
    columnWidths << 40 << 90 << 90 << 90 << 90 << 90 << 90 << 90 << 40;

    QString columns = "RecordCh,RecordWeek1,RecordWeek2,RecordWeek3,RecordWeek4,RecordWeek5,RecordWeek6,RecordWeek7,RecordMark";
    QString where = "order by RecordCh asc";
    FormHelper::dataout(type, columnNames, columnWidths, "录像计划信息", "RecordInfo", columns, where);
}

void frmConfigRecord::on_btnPrint_clicked()
{
    dataout(0);
}

void frmConfigRecord::on_btnXls_clicked()
{
    dataout(2);
}

void frmConfigRecord::on_btnPlus_clicked()
{
    //先要清空保证通道数正确
    DbQuery::clearRecordInfo();
    model->select();

    //有多少个通道就执行多少次添加按钮
    for (int i = 0; i < AppConfig::VideoCount; ++i) {        
        on_btnAdd_clicked();
    }

    //最后保存
    on_btnSave_clicked();
}

void frmConfigRecord::on_tableView_pressed()
{
    //取出录像计划值设置到任务策略表格中
    int row = ui->tableView->currentIndex().row();
    for (int column = 1; column <= 7; ++column) {
        QString flag = model->index(row, column).data().toString();
        ui->taskTableView->setSelectRow(column, flag);
    }
}

void frmConfigRecord::on_taskTableView_selectChanged()
{
    int row = ui->tableView->currentIndex().row();
    if (row < 0) {
        QtHelper::showMessageBoxError("请选择要设置的行!");
        return;
    }

    //将任务策略表格中的录像计划设置到表格数据中
    for (int column = 1; column <= 7; ++column) {
        QString flag = ui->taskTableView->getSelectRow(column);
        model->setData(model->index(row, column), flag);
    }
}
