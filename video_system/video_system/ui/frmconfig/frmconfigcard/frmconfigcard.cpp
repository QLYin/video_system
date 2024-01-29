﻿#include "frmconfigcard.h"
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
    columnNames << "" << "IP地址" << "设备ID" << "分辨率" << "版本";
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
    ui->tableCard->verticalHeader()->setVisible(false);
    QHeaderView* header = ui->tableCard->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);  
    header->setSectionResizeMode(1, QHeaderView::Stretch);
    header->setSectionResizeMode(2, QHeaderView::Stretch); 
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tableCard->setColumnWidth(0, 30);
    QtHelper::initTableView(ui->tableCard, AppData::RowHeight, true, false);
    ui->tableCard->verticalHeader()->setVisible(false);
}

void frmConfigCard::updateTableWidget(const QVector<DevInfo>& deviceInfo)
{
    if (deviceInfo.isEmpty())
    {
        return;
    }

    auto devCount = deviceInfo.size();
    for (int i = 0; i < devCount; ++i)
    {
        QCheckBox* itemCk = new QCheckBox(this);
        itemCk->setChecked(false);
        ui->tableCard->setCellWidget(i, 0, itemCk);
        ui->tableCard->setItem(i, 0, new QTableWidgetItem);
        QString itemValue = deviceInfo.at(i).ipaddr;
        auto item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 1, item);
        itemValue = QString::number(i);
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 2, item);
        itemValue = QString("%1x%2_%3HZ").arg(deviceInfo.at(i).width).arg(deviceInfo.at(i).height).arg(deviceInfo.at(i).fresh_freq);
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 3, item);
        itemValue = deviceInfo.at(i).softwareversion;
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 4, item);
    }
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