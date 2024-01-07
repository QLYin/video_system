#include "frmdatadevice.h"
#include "ui_frmdatadevice.h"
#include "qthelper.h"
#include "devicehelper.h"
#include "dbdelegate.h"
#include "dbquery.h"

frmDataDevice::frmDataDevice(QWidget *parent) : QWidget(parent), ui(new Ui::frmDataDevice)
{
    ui->setupUi(this);
    this->initForm();
    this->initData();
    this->initIcon();
    this->loadData();
}

frmDataDevice::~frmDataDevice()
{
    delete ui;
}

void frmDataDevice::initForm()
{
    ui->frameRight->setFixedWidth(AppData::RightWidth);
    ui->cboxDeviceIP->addItems(DbData::NvrInfo_NvrIP);

    QStringList listType;
    listType << "全部类型" << "系统操作" << "配置操作" << "报警操作" << "录像操作" << "文件操作" << "远程操作" << "其他操作";
    ui->cboxLogType->addItems(listType);

    QtHelper::initDataTimeEdit(ui->dateStart, 0);
    QtHelper::initDataTimeEdit(ui->dateEnd, 1);
    QtHelper::initTableView(ui->tableWidget, AppData::RowHeight);

    //关联样式改变信号自动重新设置图标
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(initIcon()));
    connect(AppEvent::Instance(), SIGNAL(changeStyle()), this, SLOT(loadData()));
}

void frmDataDevice::initIcon()
{
    //设置按钮图标
    CommonNav::setIconBtn(ui->frameRight);

    //设置图标
    QPixmap pixOk = IconHelper::getPixmap(GlobalConfig::TextColor, 0xf071, 15, 15, 15);
    QPixmap pixNo = IconHelper::getPixmap(GlobalConfig::TextColor, 0xf075, 15, 15, 15);
    d_pixmap->setCheckOkImage(pixOk);
    d_pixmap->setCheckNoImage(pixNo);
}

void frmDataDevice::initData()
{
    columnNames << "标识" << "序号" << "时间" << "用户" << "通道" << "类型" << "内容" << "备注";
    columnWidths << 50 << 60 << 160 << 120 << 60 << 100 << 160 << 50;
    int columnCount = columnNames.count();
    ui->tableWidget->setColumnCount(columnCount);
    ui->tableWidget->setHorizontalHeaderLabels(columnNames);

    //设置列宽
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidget->setColumnWidth(i, columnWidths.at(i));
    }

    //设置第一列委托,自动根据内容显示不同图标
    d_pixmap = new DbDelegate(this);
    d_pixmap->setDelegateColumn(0);
    d_pixmap->setDelegateType("QPixmap");
    d_pixmap->setCheckColumn(6);
    d_pixmap->setCheckType("contains");
    d_pixmap->setCheckValue("离线|故障|报警|错误|失败");
    ui->tableWidget->setItemDelegateForColumn(0, d_pixmap);
}

void frmDataDevice::loadData()
{
    //下面是举例数据随机模拟的,真实处理应该是用私有sdk去nvr上拉取日志记录
    QStringList content;
    content << "设备上线" << "设备离线" << "设备正常" << "设备故障" << "设备报警";

    QStringList type;
    type << "运行日志" << "设备日志" << "报警日志" << "用户操作";
    QDateTime now = ui->dateStart->dateTime();

    //设置行数+行高
    int count = AppConfig::RecordsPerpage;
    ui->tableWidget->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        now = now.addSecs(60 * 30 * i);
        QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
        QTableWidgetItem *item2 = new QTableWidgetItem(now.toString("yyyy-MM-dd HH:mm:ss"));
        QTableWidgetItem *item3 = new QTableWidgetItem("admin");
        QTableWidgetItem *item4 = new QTableWidgetItem(QString("%1").arg((rand() % 64) + 1, 2, 10, QChar('0')));
        QTableWidgetItem *item5 = new QTableWidgetItem(type.at(rand() % 3));
        QTableWidgetItem *item6 = new QTableWidgetItem(content.at(rand() % 5));

        item1->setTextAlignment(Qt::AlignCenter);
        item2->setTextAlignment(Qt::AlignCenter);
        item3->setTextAlignment(Qt::AlignCenter);
        item4->setTextAlignment(Qt::AlignCenter);
        item5->setTextAlignment(Qt::AlignCenter);
        item6->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(i, 1, item1);
        ui->tableWidget->setItem(i, 2, item2);
        ui->tableWidget->setItem(i, 3, item3);
        ui->tableWidget->setItem(i, 4, item4);
        ui->tableWidget->setItem(i, 5, item5);
        ui->tableWidget->setItem(i, 6, item6);
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem);
    }
}

void frmDataDevice::on_btnSelect_clicked()
{
    this->loadData();
}

void frmDataDevice::on_btnDelete_clicked()
{
    if (!UserHelper::checkPermission("删除记录")) {
        return;
    }

    //先弹出选择要删除的日期范围
    if (QtHelper::showMessageBoxQuestion("确定要删除记录吗? 删除后不可恢复!") == QMessageBox::Yes) {
        QtHelper::showMessageBoxInfo("执行远程删除日志命令成功!", 3);
    }
}

void frmDataDevice::on_btnClear_clicked()
{
    //执行远程清空日志命令
    QtHelper::showMessageBoxInfo("执行远程清空日志命令", 3);
}

void frmDataDevice::dataout(quint8 type)
{
    //查询需要导出的数据内容
    QStringList listContent;
    int columnCount = ui->tableWidget->columnCount();
    int rowCount = ui->tableWidget->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStringList list;
        for (int j = 1; j < columnCount; j++) {
            list << ui->tableWidget->item(i, j)->text();
        }
        listContent << list.join(";");
    }

    //先判断行数,超过一定大小弹出提示
    if (!FormHelper::checkRowCount(type, rowCount)) {
        return;
    }

    //字段要移除前面这个标识
    QList<QString> columnNames = this->columnNames;
    QList<int> columnWidths = this->columnWidths;
    columnNames.removeFirst();
    columnWidths.removeFirst();
    FormHelper::dataout(type, columnNames, columnWidths, "设备日志", "", "", "", true, true, listContent);
}

void frmDataDevice::on_btnPrint_clicked()
{
    dataout(0);
}

void frmDataDevice::on_btnPdf_clicked()
{
    dataout(1);
}

void frmDataDevice::on_btnXls_clicked()
{
    dataout(2);
}
