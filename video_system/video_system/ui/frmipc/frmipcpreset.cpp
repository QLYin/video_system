#include "frmipcpreset.h"
#include "ui_frmipcpreset.h"
#include "qthelper.h"
#include "deviceonvif.h"

frmIpcPreset::frmIpcPreset(QWidget *parent) : QWidget(parent), ui(new Ui::frmIpcPreset)
{
    ui->setupUi(this);
    this->initForm();
    this->initTable();
}

frmIpcPreset::~frmIpcPreset()
{
    delete ui;
}

void frmIpcPreset::resizeEvent(QResizeEvent *)
{
    //自动设置名称列自适应宽度拉伸填充
    ui->tableWidget->setColumnWidth(1, this->width() - 170);
}

void frmIpcPreset::initForm()
{
    //定时器模拟自动巡航
    presetIndex = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(navigate()));
    timer->setInterval(3000);
}

void frmIpcPreset::initTable()
{
    QList<QString> columnNames;
    QList<int> columnWidths;
    columnNames << "编号" << "名称" << "" << "" << "";
    columnWidths << 55 << 0 << 30 << 30 << 30;

    int columnCount = columnNames.count();
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(columnNames);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidget->setColumnWidth(i, columnWidths.at(i));
    }

    //通用函数设置表格控件
    QtHelper::initTableView(ui->tableWidget);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void frmIpcPreset::buttonClicked()
{
    //识别按钮的是哪个按钮
    QPushButton *btn = (QPushButton *)sender();
    QString presetToken = btn->property("token").toString();
    QString presetName = ui->txtPresetName->text().trimmed();

    //根据不同的按钮执行不同的动作
    QString objName = btn->objectName();
    if (objName == "btnPresetGoto") {
        DeviceOnvif::ptzPreset(0, AppData::CurrentUrl, presetToken);
    } else if (objName == "btnPresetSet") {
        DeviceOnvif::ptzPreset(1, AppData::CurrentUrl, presetToken, presetName);
    } else if (objName == "btnPresetRemove") {
        DeviceOnvif::ptzPreset(2, AppData::CurrentUrl, presetToken);
    }

    //需要重新获取
    if (objName == "btnPresetSet" || objName == "btnPresetRemove") {
        on_btnGetPresets_clicked();
    }
}

void frmIpcPreset::on_btnGetPresets_clicked()
{
    //清空数据
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    //获取预置位信息
    QList<OnvifPresetInfo> presets = DeviceOnvif::getPresets(AppData::CurrentUrl);
    int count = presets.count();
    ui->tableWidget->setRowCount(count);

    //循环设置数据到表格
    for (int i = 0; i < count; ++i) {
        OnvifPresetInfo preset = presets.at(i);

        //预置位编号
        QTableWidgetItem *itemToken = new QTableWidgetItem;
        itemToken->setText(QString("%1").arg(preset.token.toInt(), 3, 10, QChar('0')));
        itemToken->setCheckState(Qt::Unchecked);
        itemToken->setData(Qt::UserRole, preset.token);
        ui->tableWidget->setItem(i, 0, itemToken);

        //预置位名称
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(preset.name));

        //三个按钮 调用 添加(设置) 删除
        QPushButton *btnGoto = new QPushButton(this);
        QPushButton *btnSet = new QPushButton(this);
        QPushButton *btnRemove = new QPushButton(this);

        //设置按钮对象名称
        btnGoto->setObjectName("btnPresetGoto");
        btnSet->setObjectName("btnPresetSet");
        btnRemove->setObjectName("btnPresetRemove");

        //设置弱属性标明当前按钮所在行方便单击函数判断处理
        btnGoto->setProperty("token", preset.token);
        btnSet->setProperty("token", preset.token);
        btnRemove->setProperty("token", preset.token);

        //关联按钮单击事件响应处理
        connect(btnGoto, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        connect(btnSet, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));
        connect(btnRemove, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

        //设置图形字体图标
        IconHelper::setIcon(btnGoto, 0xf04b);
        IconHelper::setIcon(btnSet, 0xf067);
        IconHelper::setIcon(btnRemove, 0xf00d);

        //添加到表格单元格中
        ui->tableWidget->setCellWidget(i, 2, btnGoto);
        ui->tableWidget->setCellWidget(i, 3, btnSet);
        ui->tableWidget->setCellWidget(i, 4, btnRemove);
    }
}

void frmIpcPreset::on_btnSetPresets_clicked()
{
    QString presetName = ui->txtPresetName->text().trimmed();
    DeviceOnvif::ptzPreset(1, AppData::CurrentUrl, "", presetName);
    on_btnGetPresets_clicked();
}

void frmIpcPreset::on_btnGotoHomePosition_clicked()
{
    DeviceOnvif::ptzPreset(3, AppData::CurrentUrl);
}

void frmIpcPreset::on_btnSetHomePosition_clicked()
{
    DeviceOnvif::ptzPreset(4, AppData::CurrentUrl);
}

void frmIpcPreset::navigate()
{
    //如果到了末尾则停止
    if (presetIndex == presetTokens.count()) {
        on_btnStopNavigate_clicked();
        return;
    }

    //执行调用预置位
    QString presetToken = presetTokens.at(presetIndex);
    DeviceOnvif::ptzPreset(0, AppData::CurrentUrl, presetToken);

    //对应列表框选中当前执行的预置位
    ui->tableWidget->setCurrentCell(presetRows.at(presetIndex), 0);
    presetIndex++;
}

void frmIpcPreset::on_btnStartNavigate_clicked()
{
    presetIndex = 0;
    presetRows.clear();
    presetTokens.clear();

    //先取出选中的预置位
    int count = ui->tableWidget->rowCount();
    for (int i = 0; i < count; ++i) {
        QTableWidgetItem *item = ui->tableWidget->item(i, 0);
        if (item->checkState() == Qt::Checked) {
            presetRows << i;
            presetTokens << item->data(Qt::UserRole).toString();
        }
    }

    if (presetTokens.count() > 0) {
        timer->start();
        navigate();
    }
}

void frmIpcPreset::on_btnStopNavigate_clicked()
{
    //先停止定时器然后停止云台
    timer->stop();
    DeviceOnvif::ptzControl(0, AppData::CurrentUrl, 0, 0, 0);
}

void frmIpcPreset::on_tableWidget_itemPressed(QTableWidgetItem *item)
{
    if (item->column() == 0) {
        bool checked = (item->checkState() == Qt::Checked);
        item->setCheckState(checked ? Qt::Unchecked : Qt::Checked);
    }
}
