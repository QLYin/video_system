#include "frmconfigpollplus.h"
#include "ui_frmconfigpollplus.h"
#include "qthelper.h"
#include "dbhelper.h"
#include "frmconfigplus.h"

frmConfigPollPlus::frmConfigPollPlus(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigPollPlus)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initAction();
    this->initPollGroup();
}

frmConfigPollPlus::~frmConfigPollPlus()
{
    delete ui;
}

void frmConfigPollPlus::showEvent(QShowEvent *)
{
    model->select();
}

void frmConfigPollPlus::initForm()
{
    ui->frameGroup->setFixedWidth(AppData::RightWidth);
    QtHelper::initTableView(ui->tableView, AppData::RowHeight, false, false);
    //支持多选
    ui->tableView->setSelectionMode(QAbstractItemView::MultiSelection);

    //关联批量添加窗体信号
    connect(frmConfigPlus::Instance(), SIGNAL(addPlus(QStringList, QStringList)), this, SIGNAL(addPlus(QStringList, QStringList)));
}

void frmConfigPollPlus::initData()
{
    //实例化数据库表模型
    model = new QSqlTableModel(this);
    //绑定数据库表到数据模型
    DbHelper::bindTable(AppConfig::LocalDbType, model, "IpcInfo");
    //设置列排序
    model->setSort(0, Qt::AscendingOrder);
    //设置提交模式
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //立即查询一次
    model->select();
    //将数据库表模型设置到表格上
    ui->tableView->setModel(model);

    //初始化列名和列宽
    QList<QString> columnNames;
    columnNames << "编号" << "名称" << "录像机" << "厂家" << "设备地址" << "媒体地址" << "云台地址" << "主码流地址" << "子码流地址"
                << "经纬度" << "地图" << "X坐标" << "Y坐标" << "用户姓名" << "用户密码" << "状态" << "安装位置";
    QList<int> columnWidths;
    columnWidths << 40 << 90 << 90 << 80 << 250 << 250 << 250 << 130 << 130
                 << 150 << 90 << 45 << 45 << 80 << 80 << 50 << 60;

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

    //设置隐藏列
    for (int i = 0; i < columnNames.count(); ++i) {
        if (i == 0 || i == 1 || i == 7) {
            continue;
        }

        ui->tableView->setColumnHidden(i, true);
    }
}

void frmConfigPollPlus::initAction()
{
    QAction *actionAll = new QAction("全部选中", this);
    QAction *actionInvert = new QAction("反向选中", this);
    QAction *actionClear = new QAction("清空选中", this);
    connect(actionAll, SIGNAL(triggered(bool)), this, SLOT(doAction()));
    connect(actionInvert, SIGNAL(triggered(bool)), this, SLOT(doAction()));
    connect(actionClear, SIGNAL(triggered(bool)), this, SLOT(doAction()));

    ui->tableView->addAction(actionAll);
    ui->tableView->addAction(actionInvert);
    ui->tableView->addAction(actionClear);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void frmConfigPollPlus::doAction()
{
    QAction *action = (QAction *)sender();
    QString text = action->text();
    if (text == "全部选中") {
        ui->tableView->selectAll();
    } else if (text == "反向选中") {
        //找到所有选中的行集合
        QList<int> rows;
        QModelIndexList list = ui->tableView->selectionModel()->selectedRows();
        int count = list.count();
        for (int i = 0; i < count; ++i) {
            rows << list.at(i).row();
        }

        //先清空所有选中
        ui->tableView->clearSelection();
        //不在选中行集合的则选中
        count = ui->tableView->model()->rowCount();
        for (int i = 0; i < count; ++i) {
            if (!rows.contains(i)) {
                ui->tableView->selectRow(i);
            }
        }
    } else if (text == "清空选中") {
        ui->tableView->clearSelection();
    }
}

void frmConfigPollPlus::initPollGroup()
{
    QStringList list = AppConfig::PollGroupNames.split("|");
    ui->listWidget->clear();
    ui->listWidget->addItems(list);
    ui->listWidget->setCurrentRow(0);
}

void frmConfigPollPlus::savePollGroup()
{
    QStringList list;
    int count = ui->listWidget->count();
    for (int i = 0; i < count; ++i) {
        list << ui->listWidget->item(i)->text();
    }

    AppConfig::PollGroupNames = list.join("|");
    AppConfig::writeConfig();
    emit pollGroupChanged();
}

bool frmConfigPollPlus::checkPollRow(int row)
{
    if (row < 0) {
        QtHelper::showMessageBoxError("请先选择要删除的分组!", 3);
        return false;
    } else if (row == 0) {
        QtHelper::showMessageBoxError("默认分组不能删除!", 3);
        return false;
    }

    return true;
}

bool frmConfigPollPlus::checkPollGroup(const QString &pollGroup)
{
    if (pollGroup.isEmpty()) {
        QtHelper::showMessageBoxError("请先输入分组名称!", 3);
        ui->txtPollGroup->setFocus();
        return false;
    }

    int count = ui->listWidget->count();
    for (int i = 0; i < count; ++i) {
        if (ui->listWidget->item(i)->text() == pollGroup) {
            QtHelper::showMessageBoxError("该分组已经存在, 请重新输入!", 3);
            ui->txtPollGroup->setFocus();
            return false;
        }
    }

    return true;
}

void frmConfigPollPlus::on_btnAdd_clicked()
{
    QString pollGroup = ui->txtPollGroup->text().trimmed();
    if (!checkPollGroup(pollGroup)) {
        return;
    }

    ui->listWidget->addItem(pollGroup);
    savePollGroup();
}

void frmConfigPollPlus::on_btnDelete_clicked()
{
    int row = ui->listWidget->currentRow();
    if (!checkPollRow(row)) {
        return;
    }

    QListWidgetItem *item = ui->listWidget->takeItem(row);
    ui->listWidget->removeItemWidget(item);
    delete item;
    savePollGroup();
}

void frmConfigPollPlus::on_btnUpdate_clicked()
{
    int row = ui->listWidget->currentRow();
    if (!checkPollRow(row)) {
        return;
    }

    QString pollGroup = ui->txtPollGroup->text().trimmed();
    if (!checkPollGroup(pollGroup)) {
        return;
    }

    ui->listWidget->item(row)->setText(pollGroup);
    savePollGroup();
}

void frmConfigPollPlus::on_btnClear_clicked()
{
    AppConfig::PollGroupNames = QString::fromUtf8("默认分组");
    AppConfig::PollGroupLast = QString::fromUtf8("默认分组");
    AppConfig::writeConfig();
    initPollGroup();
}

void frmConfigPollPlus::on_btnAddSelect_clicked()
{
    //将选中行的地址取出来
    QStringList rtspMains, rtspSubs;
    QModelIndexList indexs = ui->tableView->selectionModel()->selectedIndexes();
    foreach (QModelIndex index, indexs) {
        if (index.column() == 7) {
            rtspMains << index.data().toString();
        } else if (index.column() == 8) {
            rtspSubs << index.data().toString();
        }
    }

    emit addPlus(rtspMains, rtspSubs);
}

void frmConfigPollPlus::on_btnAddPlus_clicked()
{
    frmConfigPlus::Instance()->show();
}

void frmConfigPollPlus::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    //需要加个判断
    if (current) {
        QString text = current->text();
        ui->txtPollGroup->setText(text);
        //保存最后选中的轮询分组
        AppConfig::PollGroupLast = text;
        AppConfig::writeConfig();
    }
}
