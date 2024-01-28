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

    CmdHandlerMgr::Instance()->registHandler(this);
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
    updateTableWidget();
}

void frmConfigCard::updateTableWidget()
{
    if (m_devList.isEmpty())
    {
        return;
    }

    auto devCount = m_devList.size();
    for (int i = 0; i < devCount; ++i)
    {
        QCheckBox* itemCk = new QCheckBox(this);
        itemCk->setChecked(false);
        ui->tableCard->setCellWidget(i, 0, itemCk);
        ui->tableCard->setItem(i, 0, new QTableWidgetItem);
        QString itemValue = m_devList.at(i).ipaddr;
        auto item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 1, item);
        itemValue = QString::number(i);
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 2, item);
        itemValue = QString("%1x%2_%3HZ").arg(m_devList.at(i).width).arg(m_devList.at(i).height).arg(m_devList.at(i).fresh_freq);
        item = new QTableWidgetItem(itemValue);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCard->setItem(i, 3, item);
        itemValue = m_devList.at(i).softwareversion;
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

void frmConfigCard::handle(const QVariantMap& data)
{
    auto cmd = data["cmd"].toString();
    if (cmd == CommandNS::kCmdSyncDevInfoR)
    {
        QVector<QVariantMap> vecData;
        QVariant variant = data["cmdDataArrary"];
        vecData = variant.value<QVector<QVariantMap>>();
        for (auto& item : vecData)
        {
            if (!item.isEmpty())
            {
                DevInfo devItem;
                devItem.dev_id = item["dev_id"].toInt();
                devItem.chn_cnt = item["chn_cnt"].toInt();
                devItem.floor = item["floor"].toInt();
                devItem.upper = item["upper"].toInt();
                devItem.width = item["width"].toInt();
                devItem.height = item["height"].toInt();
                devItem.fresh_freq = item["fresh_freq"].toInt();
                devItem.ipaddr = item["ipaddr"].toString();
                devItem.softwareversion = item["softwareversion"].toString();
                if (devItem.chn_cnt > 0)
                {
                    for (int i = 0; i < devItem.chn_cnt; ++i)
                    {
                        auto key = QString("chn_") + QString::number(i);
                        devItem.ipc_indexs.push_back(item[key].toInt());
                    }
                }

                m_devList.append(devItem);
            }
        }
        updateTableWidget();
    }
}

const QVector<DevInfo>& frmConfigCard::devListInfo()
{
    return m_devList;
}