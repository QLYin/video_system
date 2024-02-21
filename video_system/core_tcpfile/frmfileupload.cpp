#include "frmfileupload.h"
#include "ui_frmfileupload.h"
#include "tcpfilehelper.h"
#include "tcpsendfilethread.h"

frmFileUpload::frmFileUpload(QWidget *parent) : QWidget(parent), ui(new Ui::frmFileUpload)
{
    ui->setupUi(this);
    this->initForm();
}

frmFileUpload::~frmFileUpload()
{
    delete ui;
}

void frmFileUpload::showEvent(QShowEvent *)
{
    //根据列头宽度自动调整全选框位置
    int width = ui->tableWidget->verticalHeader()->width();
    //ui->tableWidget->verticalHeader()->setFixedWidth(width);
    ckAll->move(width + 1, -2);
}

QStringList frmFileUpload::getFileNames()
{
    return this->fileNames;
}

QTableWidget *frmFileUpload::getTable()
{
    return ui->tableWidget;
}

void frmFileUpload::initForm()
{
    //列名称+列宽度
    QStringList columnName;
    columnName << "" << "文件类型" << "文件名称" << "文件大小" << "单位" << "上传进度";
    QList<int> columnWidth;
    columnWidth << 30 << 100 << 250 << 100 << 60 << 100;

    //设置列数和列宽
    int columnCount = columnWidth.count();
    ui->tableWidget->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        ui->tableWidget->setColumnWidth(i, columnWidth.at(i));
    }

    //设置行高
    ui->tableWidget->setRowCount(TcpFileHelper::maxRow);
    ui->tableWidget->setHorizontalHeaderLabels(columnName);
    TcpFileHelper::initTableView(ui->tableWidget, 25, true);

    //增加一个全选按钮
    ckAll = new QCheckBox(this);
    connect(ckAll, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
    ckAll->setChecked(true);
}

void frmFileUpload::stateChanged(int arg1)
{
    int count = fileNames.count();
    for (int i = 0; i < count; ++i) {
        QCheckBox *itemCk = (QCheckBox *)ui->tableWidget->cellWidget(i, 0);
        if (itemCk) {
            itemCk->setChecked(arg1 != 0);
        }
    }
}

void frmFileUpload::sendFinsh(const QString &ip, int port, const QString &error)
{
    //每次来一个就累加直到和文件数量一致表示全部发送完成
    sendFinshCount++;
    if (sendFinshCount == files.count()) {
        emit finshed();
    }
}

void frmFileUpload::on_tableWidget_cellPressed(int row, int column)
{
    //单击单元格切换选中不选中
    QCheckBox *ckbox = (QCheckBox *)ui->tableWidget->cellWidget(row, 0);
    if (ckbox) {
        ckbox->setChecked(ckbox->checkState() != Qt::Checked);
    }
}

void frmFileUpload::initStyle(bool progressBarStyle)
{
    //设置样式
    QStringList list;
    if (progressBarStyle) {
        //list << QString("QProgressBar{padding:2px;}");
        list << QString("QProgressBar,QProgressBar::chunk{text-align:center;border-radius:%1px}").arg(0);
    }

    list << QString("QCheckBox{padding:0px 0px 0px 7px;}");
    this->setStyleSheet(list.join(""));
}

void frmFileUpload::clear()
{
    fileNames.clear();
    ui->tableWidget->clearContents();
}

void frmFileUpload::addFile(const QString &type, const QString &name, const QString &path)
{
    //添加完整路径
    int row = fileNames.count();
    QString fileName = path + "/" + name;
    fileNames << fileName;
    quint64 fileSize = QFile(fileName).size();

    //复选框,如果勾选了全选则自动选中当前设备
    QCheckBox *checkBox = new QCheckBox(this);
    checkBox->setChecked(ckAll->isChecked());

    //进度条
    QProgressBar *progressBar = new QProgressBar(this);
    TcpFileHelper::initProgressBar(progressBar, fileSize);

    //文件类型
    QTableWidgetItem *itemType = new QTableWidgetItem;
    itemType->setText(type);
    itemType->setTextAlignment(Qt::AlignCenter);

    //文件名称
    QTableWidgetItem *itemFile = new QTableWidgetItem;
    itemFile->setText(name);
    itemFile->setData(Qt::UserRole, QString(path));
    itemFile->setTextAlignment(Qt::AlignCenter);

    //文件大小
    QTableWidgetItem *itemSize = new QTableWidgetItem;
    QString text = TcpFileHelper::getSize(fileSize);
    itemSize->setText(text);
    itemSize->setData(Qt::UserRole, fileSize);
    itemSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //文件大小单位
    QTableWidgetItem *itemUnit = new QTableWidgetItem;
    itemUnit->setText(text.split(" ").last());
    itemUnit->setTextAlignment(Qt::AlignCenter);

    ui->tableWidget->setCellWidget(row, 0, checkBox);
    ui->tableWidget->setCellWidget(row, 5, progressBar);
    ui->tableWidget->setItem(row, 1, itemType);
    ui->tableWidget->setItem(row, 2, itemFile);
    ui->tableWidget->setItem(row, 3, itemSize);
    ui->tableWidget->setItem(row, 4, itemUnit);
}

void frmFileUpload::upload(const QString &receiveHost, int receivePort, int maxPackageSize)
{
    sendFinshCount = 0;
    int count = fileNames.count();
    if (count == 0) {
        emit error("没有文件需要上传");
        return;
    }

    //过滤选中的文件
    files.clear();
    QList<QProgressBar *> bars;
    for (int i = 0; i < count; ++i) {
        QCheckBox *checkBox = (QCheckBox *)ui->tableWidget->cellWidget(i, 0);
        QProgressBar *progressBar = (QProgressBar *)ui->tableWidget->cellWidget(i, 5);
        if (checkBox && checkBox->isChecked()) {
            files << fileNames.at(i);
            bars << progressBar;
        }
    }

    count = files.count();
    if (count == 0) {
        emit error("没有文件需要上传");
        return;
    }

    //每个上传都一个线程
    for (int i = 0; i < count; ++i) {
        QString fileName = files.at(i);
        //准备发送结构体数据
        SendFileConfig sendFileConfig;
        sendFileConfig.fileName = fileName;
        sendFileConfig.fileSize = QFile(fileName).size();
        sendFileConfig.maxPackageSize = maxPackageSize;
        sendFileConfig.timeout = 1000;
        sendFileConfig.receiveHost = receiveHost;
        sendFileConfig.receivePort = receivePort;
        sendFileConfig.progressBar = bars.at(i);
        sendFileConfig.progressBar->setValue(0);

        //新建线程发送文件
        TcpSendFileThread *sendFileThread = new TcpSendFileThread(this);
        connect(sendFileThread, SIGNAL(sendFinsh(QString, int, QString)), this, SLOT(sendFinsh(QString, int, QString)));
        sendFileThread->setSendFileConfig(sendFileConfig);
        sendFileThread->start();
    }
}
