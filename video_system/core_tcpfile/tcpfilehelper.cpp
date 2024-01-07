#include "tcpfilehelper.h"

QString TcpFileHelper::getSize(quint64 size)
{
    //转换成单位 KB MB GB 更直观
    QString flag = "KB";
    double value = (double)size / 1024;

    if (value > 1024) {
        value = value / 1024;
        flag = "MB";
    }

    if (value > 1024) {
        value = value / 1024;
        flag = "GB";
    }

    return QString("%1 %2").arg(QString::number(value, 'f', 2)).arg(flag);
}

QString TcpFileHelper::getPath(const QString &path)
{
    QString dir = path;
    if (dir.startsWith("./")) {
        dir = qApp->applicationDirPath() + dir.mid(1, dir.length());
    }

    return dir;
}

void TcpFileHelper::initProgressBar(QProgressBar *progressBar, quint64 fileSize)
{
    //值居中显示
    progressBar->setAlignment(Qt::AlignCenter);
    //由于进度条最大值是int类型,大文件大小会溢出,所以这里缩小1000倍
    progressBar->setRange(0, fileSize > 1000 ? (fileSize / 1000) : 1000);
    progressBar->setValue(0);
}

void TcpFileHelper::updateProgressBar(QProgressBar *progressBar, quint64 fileSize, quint64 size)
{
    if (progressBar) {
        //-1=出错 0=发送完成 其他=发送的进度
        if (size == -1) {
            progressBar->setValue(0);
        } else if (size == 0) {
            progressBar->setValue(progressBar->maximum());
        } else {
            int percent = fileSize > 1000 ? 1000 : 1;
            progressBar->setValue(progressBar->value() + size / percent);
        }
    }
}

int TcpFileHelper::maxRow = 99;
void TcpFileHelper::initTable(QTableWidget *tableWidget, int rowHeight)
{
    static QStringList columnName;
    if (columnName.count() == 0) {
        columnName << "地址" << "端口" << "文件名称" << "文件大小" << "当前包" << "发送进度";
    }

    static QList<int> columnWidth;
    if (columnWidth.count() == 0) {
        columnWidth << 130 << 80 << 180 << 80 << 60 << 80;
    }

    //设置列数和列宽
    int columnCount = columnWidth.count();
    tableWidget->setColumnCount(columnCount);
    for (int i = 0; i < columnCount; ++i) {
        tableWidget->setColumnWidth(i, columnWidth.at(i));
    }

    //设置行高
    tableWidget->setRowCount(maxRow);
    for (int i = 0; i < maxRow; ++i) {
        tableWidget->setRowHeight(i, rowHeight);
    }

    //设置字段名称
    tableWidget->setHorizontalHeaderLabels(columnName);
    //初始化表格控件属性
    initTableView(tableWidget, rowHeight, true);
}

void TcpFileHelper::initTableView(QTableView *tableView, int rowHeight,
                                  bool headVisible, bool edit, bool stretchLast)
{
    //设置弱属性用于应用qss特殊样式
    tableView->setProperty("model", true);
    //取消自动换行
    tableView->setWordWrap(false);
    //超出文本不显示省略号
    tableView->setTextElideMode(Qt::ElideNone);
    //奇数偶数行颜色交替
    tableView->setAlternatingRowColors(false);
    //垂直表头是否可见
    tableView->verticalHeader()->setVisible(headVisible);
    //选中一行表头是否加粗
    tableView->horizontalHeader()->setHighlightSections(false);
    //最后一行拉伸填充
    tableView->horizontalHeader()->setStretchLastSection(stretchLast);
    //行标题最小宽度尺寸
    tableView->horizontalHeader()->setMinimumSectionSize(0);
    //行标题最大高度
    tableView->horizontalHeader()->setFixedHeight(rowHeight);
    //默认行高
    tableView->verticalHeader()->setDefaultSectionSize(rowHeight);
    //选中时一行整体选中
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //只允许选择单个
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    //表头不可单击
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    tableView->horizontalHeader()->setSectionsClickable(false);
#else
    tableView->horizontalHeader()->setClickable(false);
#endif

    //鼠标按下即进入编辑模式
    if (edit) {
        tableView->setEditTriggers(QAbstractItemView::CurrentChanged | QAbstractItemView::DoubleClicked);
    } else {
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void TcpFileHelper::addRow(QTableWidget *tableWidget, QProgressBar *progressBar,
                           int &row, const QString &ip, int port,
                           const QString &fileName, quint64 fileSize)
{
    //实例化单元格控件
    QTableWidgetItem *itemHost = new QTableWidgetItem(ip);
    QTableWidgetItem *itemPort = new QTableWidgetItem(QString::number(port));
    QTableWidgetItem *itemName = new QTableWidgetItem(fileName);
    QTableWidgetItem *itemSize = new QTableWidgetItem(getSize(fileSize));
    itemSize->setData(Qt::UserRole, fileSize);
    QTableWidgetItem *itemPackage = new QTableWidgetItem("0");

    //设置居中
    itemHost->setTextAlignment(Qt::AlignCenter);
    itemPort->setTextAlignment(Qt::AlignCenter);
    itemName->setTextAlignment(Qt::AlignCenter);
    itemSize->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    itemPackage->setTextAlignment(Qt::AlignCenter);

    //挨个设置单元格
    tableWidget->setItem(row, 0, itemHost);
    tableWidget->setItem(row, 1, itemPort);
    tableWidget->setItem(row, 2, itemName);
    tableWidget->setItem(row, 3, itemSize);
    tableWidget->setItem(row, 4, itemPackage);
    tableWidget->setCellWidget(row, 5, progressBar);
    row++;
}

void TcpFileHelper::xorData(QByteArray &data, quint8 key)
{
    for (int i = 0; i < data.length(); ++i) {
        data[i] = data.at(i) ^ key;
    }
}

void TcpFileHelper::encryData(QByteArray &data, const QString &key)
{
    xorData(data, key.toInt());
}

void TcpFileHelper::decryData(QByteArray &data, const QString &key)
{
    xorData(data, key.toInt());
}

void TcpFileHelper::tarAndChmod(const QString &fileName, const QString &savePath)
{
#ifndef __arm__
    return;
#endif
    //根据拓展名判断是否要进行解压
    //如果是可执行文件则立即赋予权限
    if (fileName.endsWith(".tar.gz")) {
        QString cmd = QString("tar zxvf %1 -C /").arg(fileName);
        system(cmd.toLatin1());
    } else {
        QString cmd = QString("chmod 777 %1/%2").arg(savePath).arg(fileName);
        system(cmd.toLatin1());
    }
}

void TcpFileHelper::reboot()
{
#ifndef __arm__
    return;
#endif
    system("reboot");
}
