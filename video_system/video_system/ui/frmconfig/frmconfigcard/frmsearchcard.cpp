﻿#include "frmsearchcard.h"
#include "ui_frmsearchcard.h"
#include "qthelper.h"
#include "dbquery.h"
#include "frmmain.h"
#include "frmautonetset.h"
#include "class/deviceconnect/tcpcmddef.h"
#include "class/deviceconnect/tcpclienthelper.h"
#include "ui/frmbase/Indicator.h"

frmSearchCard::frmSearchCard(QWidget *parent) : QDialog(parent), ui(new Ui::frmSearchCard)
{
    ui->setupUi(this);
    this->initStyle();
    this->initTitle();
    this->initForm();
    this->initIcon();
    QtHelper::setFormInCenter(this);
}

frmSearchCard::~frmSearchCard()
{
    delete ui;
}

void frmSearchCard::setBanner(const QString &image)
{
    //QString qss = QString("border-image:url(%1);").arg(image);
    //ui->labName->setStyleSheet(qss);
}

void frmSearchCard::initStyle()
{
    resize(900, 600);
    //初始化无边框窗体
    QtHelper::setFramelessForm(this, ui->widgetTitle, ui->labIco, ui->btnMenu_Close, false);
    //关联关闭按钮退出
    connect(ui->btnMenu_Close, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(hide()));
    connect(ui->btnMin, SIGNAL(clicked()), this, SLOT(minimumSize()));
}

void frmSearchCard::initTitle()
{
    this->setWindowTitle(ui->labTitle->text());
}

void frmSearchCard::initForm()
{
    QList<QString> columnNames;
    columnNames << "序号" << "IP地址" << "子网掩码" << "默认网关" << "MAC地址" << "版本" << "状态";
    QList<int> columnWidths;
    columnWidths << 50 << 140 << 140 << 140 << 140 << 140 << 100;
    ui->tableWidget->setStyleSheet("QCheckBox{padding:0px 0px 0px 7px;}");

    //设置列数和列宽
    int columnCount = columnWidths.count();
    ui->tableWidget->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidget->setColumnWidth(i, columnWidths.at(i));
    }

    //设置行数列名等
    ui->tableWidget->setRowCount(255);
    ui->tableWidget->setHorizontalHeaderLabels(columnNames);
    ui->tableWidget->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
    QtHelper::initTableView(ui->tableWidget, AppData::RowHeight, true, false);

    connect(ui->btnAutoNetworking, SIGNAL(clicked()), this, SLOT(onAutoNetWorkClicked()));
    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(onBtnAddClicked()));
    connect(ui->btnSelectAll, SIGNAL(clicked()), this, SLOT(onBtnAddClicked()));
    connect(ui->btnSelectAll, SIGNAL(clicked()), this, SLOT(onBtnSelectAllClicked()));
    connect(ui->btnSelectNone, SIGNAL(clicked()), this, SLOT(onBtnSelectNoneClicked()));
    connect(ui->btnAutoNetworking, SIGNAL(clicked()), this, SLOT(onBtnAutoNetClicked()));
    connect(ui->btnDetectConfilct, SIGNAL(clicked()), this, SLOT(onBtnDetectConfilctClicked()));
    connect(ui->btnSet, SIGNAL(clicked()), this, SLOT(onBtnSetClicked()));
}

void frmSearchCard::initIcon()
{
    ////图片文件不存在则设置为图形字体
    //QtHelper::setIconBtn(ui->btnLogin, ":/image/btn_ok.png", 0xf00c);
    //QtHelper::setIconBtn(ui->btnClose, ":/image/btn_close.png", 0xf00d);
}

void frmSearchCard::onAutoNetWorkClicked()
{
    auto dialog = new frmAutoNetSet;
    dialog->show();
}

void frmSearchCard::updateTableWidget(QVector<DevInfo>& deviceInfo)
{
    Indicator::showLoading(false, this);
    if (deviceInfo.isEmpty())
    {
        return;
    }

    auto customCompare = [](DevInfo& dev1, DevInfo& dev2)
    {
        return dev1.dev_id < dev2.dev_id;
    };

    std::sort(deviceInfo.begin(), deviceInfo.end(), customCompare);


    m_searchCards = deviceInfo;
    auto devCount = deviceInfo.size();
    for (int i = 0; i < devCount; ++i)
    {
        QCheckBox* itemCk = new QCheckBox(this);
        itemCk->setChecked(false);
        itemCk->setText(QString::number(deviceInfo.at(i).dev_id));
        ui->tableWidget->setCellWidget(i, 0, itemCk);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem);
        //ip
        QString itemValue = deviceInfo.at(i).ipaddr;
        auto item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 1, item);
        //子网掩码
        itemValue = deviceInfo.at(i).netmask;
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, item);
        //网关
        itemValue = deviceInfo.at(i).gateway;
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 3, item);
        //mac地址
        itemValue = deviceInfo.at(i).mac;
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 4, item);
        //版本
        itemValue = deviceInfo.at(i).softwareversion;
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 5, item);
        //状态
        itemValue = "success";
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 6, item);
    }
}

void frmSearchCard::onBtnAddClicked()
{
    auto rows = ui->tableWidget->rowCount();
    QVector<DevInfo> cards;
    for (int row = 0; row < rows; row++) {
        QCheckBox* itemCk = (QCheckBox*)ui->tableWidget->cellWidget(row, 0);
        if (itemCk && itemCk->isChecked()) {
            cards.push_back(m_searchCards.at(row));
        }
    }

    if (!cards.isEmpty())
    {
        emit cardUpdateSig(cards);
    }

}

void frmSearchCard::onBtnSelectAllClicked()
{
    auto rows = ui->tableWidget->rowCount();
    for (int row = 0; row < rows; row++) {
        QCheckBox* itemCk = (QCheckBox*)ui->tableWidget->cellWidget(row, 0);
        if (itemCk) {
            itemCk->setChecked(true);
        }
    }
}

void frmSearchCard::onBtnSelectNoneClicked()
{
    auto rows = ui->tableWidget->rowCount();
    for (int row = 0; row < rows; row++) {
        QCheckBox* itemCk = (QCheckBox*)ui->tableWidget->cellWidget(row, 0);
        if (itemCk) {
            itemCk->setChecked(false);
        }
    }
}

void frmSearchCard::onBtnAutoNetClicked()
{

}

void frmSearchCard::onBtnDetectConfilctClicked()
{

}

void frmSearchCard::onBtnSetClicked()
{
    auto rows = ui->tableWidget->rowCount();
    for (int row = 0; row < rows; row++) {
        QCheckBox* itemCk = (QCheckBox*)ui->tableWidget->cellWidget(row, 0);
        if (itemCk && itemCk->isChecked()) {
            m_searchCards[row].ipaddr =  ui->tableWidget->item(row, 1)->text();
            m_searchCards[row].netmask = ui->tableWidget->item(row, 2)->text();
            m_searchCards[row].gateway = ui->tableWidget->item(row, 3)->text();
        }
    }
}