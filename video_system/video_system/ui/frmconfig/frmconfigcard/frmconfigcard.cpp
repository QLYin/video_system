#include "frmconfigcard.h"
#include "ui_frmconfigcard.h"
#include "qthelper.h"
#include "frmconfigcard/frmsearchcard.h"
#include "frmconfigcard/frmscreencharset.h"
#include "frmconfigcard/frmresolutionset.h"

frmConfigCard::frmConfigCard(QWidget *parent) : QWidget(parent), ui(new Ui::frmConfigCard)
{
    ui->setupUi(this);
    auto label = new QLabel(this);
    this->initForm();
    this->initData();
    this->initIcon();
}

frmConfigCard::~frmConfigCard()
{
    delete ui;
}

void frmConfigCard::showEvent(QShowEvent *)
{
}

void frmConfigCard::initForm()
{
    ui->frameRight->setFixedWidth(AppData::RightWidth);
    connect(ui->btnSearch, SIGNAL(clicked()), this, SLOT(onBtnSearchClicked()));
    connect(ui->btnScreenChar, SIGNAL(clicked()), this, SLOT(onBtnScreenCharClicked()));
    connect(ui->btnResolution, SIGNAL(clicked()), this, SLOT(onBtnResolutionClicked()));
}

void frmConfigCard::initIcon()
{
   
}

void frmConfigCard::initData()
{
    QList<QString> columnNames;
    columnNames << "序号" << "IP地址" << "设备ID" << "分辨率" << "版本";
    QList<int> columnWidths;
    columnWidths << 80 << 200 << 80 << 350 << 200;
    ui->tableCard->setStyleSheet("QCheckBox{padding:0px 0px 0px 7px;}");

    //设置列数和列宽
    int columnCount = columnWidths.count();
    ui->tableCard->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableCard->setColumnWidth(i, columnWidths.at(i));
    }

    //设置行数列名等
    ui->tableCard->setRowCount(255);
    ui->tableCard->setHorizontalHeaderLabels(columnNames);
    ui->tableCard->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    QtHelper::initTableView(ui->tableCard, AppData::RowHeight, true, false);

    //根据同步的解码卡信息初始化

}

void frmConfigCard::onBtnSearchClicked()
{
    auto dialog = new frmSearchCard;
    dialog->show();
}

void frmConfigCard::onBtnScreenCharClicked()
{
    auto dialog = new frmScreenCharSet;
    dialog->show();
}

void frmConfigCard::onBtnResolutionClicked()
{
    auto dialog = new  frmResolutionSet;
    dialog->show();
}