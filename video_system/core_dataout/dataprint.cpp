#include "dataprint.h"
#include "datahelper.h"

DataPrint *DataPrint::dataPrint = 0;
void DataPrint::savePdf(const DataContent &dataContent)
{
    //没有实例化过则先实例化
    if (dataPrint == 0) {
        dataPrint = new DataPrint;
    }

    //没有内容数据则不用处理
    if (dataContent.content.count() == 0) {
        return;
    }

    dataPrint->init();
    dataPrint->setDataContent(dataContent);
    dataPrint->open();
    dataPrint->takeContent();
    dataPrint->close();
}

void DataPrint::savePdf(const QString &fileName, const QString &title,
                        const QList<QString> &columnNames, const QList<int> &columnWidths,
                        const QStringList &content)
{
    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    DataPrint::savePdf(dataContent);
}

void DataPrint::print(const DataContent &dataContent)
{
    //没有实例化过则先实例化
    if (dataPrint == 0) {
        dataPrint = new DataPrint;
    }

    //没有内容数据则不用处理
    if (dataContent.content.count() == 0) {
        return;
    }

    dataPrint->init();
    dataPrint->setDataContent(dataContent);
    dataPrint->print();
}

void DataPrint::print(const QString &title,
                      const QList<QString> &columnNames, const QList<int> &columnWidths,
                      const QStringList &content, bool landscape)
{
    DataContent dataContent;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = landscape;
    DataPrint::print(dataContent);
}

DataPrint::DataPrint(QObject *parent) : QThread(parent)
{
    this->init();
}

DataPrint::~DataPrint()
{
    this->stop();
    this->wait();
}

void DataPrint::run()
{
    time.restart();
    while (!stopped) {
        takeContent();
        msleep(100);
    }

    printView(&printer);
    stopped = false;
}

QString DataPrint::getUseTime()
{
    return QString::number((float)time.elapsed() / 1000, 'f', 3);
}

QPrinter *DataPrint::getPrinter()
{
    return &printer;
}

void DataPrint::appendHead()
{
    //清空原有数据,确保每次都是新的数据
    list.clear();
    //表格开始
    list << QString("<table border='%1' cellspacing='0' cellpadding='3'>").arg((float)dataContent.borderWidth / 2);
}

void DataPrint::appendTitle()
{
    //标题占一行,居中显示
    if (!dataContent.title.isEmpty()) {
        list << "<tr>";
        list << QString("<td align='center' colspan='%1' style='font-size:18px;font-weight:bold;'>%2</td>").arg(columnCount).arg(dataContent.title);
        list << "</tr>";
    }

    //子标题占一行,左对齐显示
    if (!dataContent.subTitle.isEmpty()) {
        list << "<tr>";
        list << QString("<td align='left' colspan='%1' style='font-size:16px;font-weight:normal;'>%2</td>").arg(columnCount).arg(dataContent.subTitle);
        list << "</tr>";
    }
}

void DataPrint::appendColumn()
{
    //循环写入字段名,字段名占一行,居中显示
    if (columnCount > 0) {
        //字段名称行背景颜色特殊
        list << "<tr bgcolor='lightgray'>";
        for (int i = 0; i < columnCount; i++) {
            QString width = QString::number(dataContent.columnWidths.at(i));
            //最后一列自适应宽度
            if (i == columnCount - 1 && dataContent.stretchLast) {
                width = "100%";
            }

            QString align = DataHelper::getAlignStyle(dataContent, 0, i);
            list << QString("<td width='%1' align='%2'>%3</td>").arg(width).arg(align).arg(dataContent.columnNames.at(i));
        }
        list << "</tr>";
    }
}

void DataPrint::appendEnd()
{
    list << "</table>";
}

void DataPrint::appendContent(const QStringList &listContent)
{
    //循环一行行构建数据
    int rowCount = listContent.count();
    for (int i = 0; i < rowCount; i++) {
        list << "<tr>";
        QString temp = listContent.at(i);
        QStringList value = temp.split(dataContent.separator);

        //如果启用了过滤数据,则将符合条件的数据突出颜色显示
        bool exist = false;
        if (dataContent.checkColumn >= 0) {
            exist = DataHelper::checkColumn(value.at(dataContent.checkColumn), dataContent.checkType, dataContent.checkValue);
        }        

        for (int j = 0; j < columnCount; j++) {
            QString text = value.at(j);
            if (exist) {
                text = QString("<font color='%1'>%2</font>").arg(dataContent.checkColor).arg(text);
            }

            QString align = DataHelper::getAlignStyle(dataContent, 1, j);
            list << QString("<td width='%1' align='%2' style='vertical-align:middle;'>%3</td>").arg(dataContent.columnWidths.at(j)).arg(align).arg(text);
        }

        list << "</tr>";
    }
}

void DataPrint::takeContent()
{
    //行数不对则返回
    int count = dataContent.content.count();
    if (count <= 0) {
        return;
    }

    //取出当前所有行内容
    mutex.lock();
    QStringList listContent = dataContent.content;
    QStringList listSubTitle1 = dataContent.subTitle1;
    QStringList listSubTitle2 = dataContent.subTitle2;
    dataContent.content.clear();
    dataContent.subTitle1.clear();
    dataContent.subTitle2.clear();
    mutex.unlock();

    //启动计时
    QElapsedTimer time;
    time.start();

    int contentCount = listContent.count();
    int subTitle1Count = listSubTitle1.count();
    int subTitle2Count = listSubTitle2.count();
    if (subTitle1Count > 0 && subTitle1Count == subTitle2Count) {
        //循环添加子标题/字段名/内容
        for (int i = 0; i < contentCount; i++) {
            //加个空行隔开
            list << QString("<tr><td colspan='%1'></td></tr>").arg(columnCount);

            //子标题1
            if (subTitle1Count > i) {
                if (!listSubTitle1.at(i).isEmpty()) {
                    list << QString("<tr><td align='left' colspan='%1'>%2</td></tr>").arg(columnCount).arg(listSubTitle1.at(i));
                }
            }

            //子标题2
            if (subTitle2Count > i) {
                if (!listSubTitle2.at(i).isEmpty()) {
                    list << QString("<tr><td align='left' colspan='%1'>%2</td></tr>").arg(columnCount).arg(listSubTitle2.at(i));
                }
            }

            //逐个添加字段名称
            appendColumn();
            //逐行添加该分类下的内容
            appendContent(listContent.at(i).split(dataContent.subSeparator));
        }
    } else {
        appendContent(listContent);
    }

    emit appendFinshed(count, time.elapsed());
}

void DataPrint::setDataContent(const DataContent &dataContent)
{
    this->dataContent = dataContent;

    //设置统一分辨率,不设置的话在不同系统上默认值不一样,而且导出到pdf和打印也不一样
    printer.setResolution(96);

#if (QT_VERSION >= QT_VERSION_CHECK(5,3,0))
    //设置纸张规格
    printer.setPageSize(QPageSize(QPageSize::A4));
    //设置纸张方向 横向+纵向
    printer.setPageOrientation(dataContent.landscape ? QPageLayout::Landscape : QPageLayout::Portrait);
    //设置页边距 可以自行调整
    printer.setPageMargins(dataContent.pageMargin, QPageLayout::Millimeter);
#else
    //设置纸张规格
    printer.setPageSize(QPrinter::A4);
    //设置纸张方向 横向+纵向
    printer.setOrientation(dataContent.landscape ? QPrinter::Landscape : QPrinter::Portrait);
    //设置页边距 可以自行调整
    printer.setPageMargins(dataContent.pageMargin.left(), dataContent.pageMargin.top(), dataContent.pageMargin.right(), dataContent.pageMargin.bottom(), QPrinter::Millimeter);
#endif
}

void DataPrint::init()
{
    stopped = false;
    list.clear();
}

bool DataPrint::append()
{
    //如果有html数据则不需要添加
    columnCount = dataContent.columnNames.count();
    if (!dataContent.html.isEmpty()) {
        return true;
    }

    if (!DataHelper::checkDataContent(dataContent)) {
        return false;
    }

    //输出固定头部信息
    appendHead();
    //输出标题信息
    appendTitle();
    //输出字段名称
    if (dataContent.subTitle1.count() == 0) {
        appendColumn();
    }

    return true;
}

void DataPrint::open(bool thread)
{
    if (dataContent.fileName.isEmpty()) {
        return;
    }
    if (thread) {
        textDocument.moveToThread(this);
    }
    if (!append()) {
        return;
    }

    //设置输出格式
    printer.setOutputFormat(QPrinter::PdfFormat);
    //设置输出文件保存位置
    printer.setOutputFileName(dataContent.fileName);
}

void DataPrint::print()
{
    if (!append()) {
        return;
    }

    //打印必须在主线程所以这里采用直接输出内容
    takeContent();

    //设置输出格式
    printer.setOutputFormat(QPrinter::NativeFormat);

    //弹出打印预览对话框
    QPrintPreviewDialog preview(&printer);
    //设置默认缩放100%
    QComboBox *cbox = preview.findChildren<QComboBox *>().first();
    cbox->setCurrentIndex(cbox->findText("100%"));
    cbox->setProperty("noinput", true);
    //设置对话框默认大小
    preview.resize(1200, 850);
    //设置工具栏控件样式
    preview.setStyleSheet("QToolButton{background:none;margin:2px;padding:0px;border-width:0px;border-radius:0px;}QLineEdit{border-width:0px;}");
    //设置对话框标题
    preview.setWindowTitle("打印预览");
    //设置置顶显示
    preview.setWindowFlags(Qt::WindowStaysOnTopHint);
    //关联信号槽在槽函数中绘制内容
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), this, SLOT(printView(QPrinter *)));
    //最大化显示
    preview.showMaximized();
    preview.exec();

    //打印到纸张完成以后清空内存数据,减少内存占用
    list.clear();
}

void DataPrint::close()
{
    printView(&printer);

    //打印到pdf完成以后清空内存数据,减少内存占用
    list.clear();
}

void DataPrint::printView(QPrinter *printer)
{
    //将html内容赋值到打印文档中
    //如果外部没有设置过内容则取当前输出的内容
    QString html;
    if (dataContent.html.isEmpty()) {
        //输出结尾信息
        appendEnd();
        html = list.join("");
    } else {
        html = dataContent.html;
    }

    emit receiveHtml(html);
    textDocument.setHtml(html);
    QRectF rect = printer->pageRect(QPrinter::DevicePixel);
    //不设置页面尺寸最后的效果有页码我也是醉了
    textDocument.setPageSize(rect.size());
    textDocument.print(printer);
    textDocument.end();
}

void DataPrint::stop()
{
    stopped = true;
}

void DataPrint::append(const QString &content, const QString &subTitle1, const QString &subTitle2)
{
    if (content.isEmpty()) {
        return;
    }

    //先要加锁
    mutex.lock();
    dataContent.content << content;

    if (!subTitle1.isEmpty()) {
        dataContent.subTitle1 << subTitle1;
    }

    if (!subTitle2.isEmpty()) {
        dataContent.subTitle2 << subTitle2;
    }

    mutex.unlock();
}
