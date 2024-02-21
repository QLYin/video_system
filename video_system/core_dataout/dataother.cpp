#include "dataother.h"
#include "datareport.h"

#ifdef qchart
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
QT_CHARTS_USE_NAMESPACE
#endif
#endif

void DataOther::savePdf(const QString &fileName, const QString &content)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,3,0))
    //创建pdf文件对象
    QFile pdfFile(fileName);
    if (!pdfFile.open(QIODevice::WriteOnly)) {
        return;
    }

    //创建pdf写入对象
    QPdfWriter pdfWriter(&pdfFile);
    //设置纸张尺寸
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    //设置纸张分辨率
    pdfWriter.setResolution(96);
    //设置页边距
    int margin = 20;
    pdfWriter.setPageMargins(QMarginsF(margin, margin, margin, margin), QPageLayout::Millimeter);
    //设置纸张方向
    pdfWriter.setPageOrientation(QPageLayout::Portrait);

    //设置标题和创建者
    pdfWriter.setTitle("测试标题");
    pdfWriter.setCreator("创建者Qt");

    //创建pdf绘图对象
    QPainter pdfPainter;
    pdfPainter.begin(&pdfWriter);

    //设置字体
    QFont font;
    font.setPixelSize(20);
    pdfPainter.setFont(font);

    //绘制第1页内容 - 纵向排版
    pdfPainter.drawText(0, 0, "第一页内容");

    //绘制第2页内容 - 横向排版
    pdfWriter.setPageOrientation(QPageLayout::Landscape);
    pdfWriter.newPage();
    pdfPainter.drawText(0, 0, "第二页内容");

    //绘制第3页内容 - 富文本
    pdfWriter.setPageOrientation(QPageLayout::Portrait);
    pdfWriter.newPage();
    //pdfPainter.drawText(0, 0, "第三页内容");

    //采用html格式内容输出非常强大
    QStringList list;
#if 1
    list << "<h1>这里是标题</h1>";
    list << "<p>这里是段落。</p>";
    int width = pdfWriter.width();
    list << QString("<table width='%1' border='0.5' cellspacing='0' cellpadding='6'>").arg(width);
    list << QString("<tr style='font-weight:bold;'><td align='center'>姓名</td><td align='center'>成绩</td></tr>");
    for (int i = 0; i < 10; ++i) {
        list << QString("<tr><td align='center'>姓名%1</td><td align='center'>%2</td></tr>").arg(i + 1).arg(rand() % 100);
    }
    list << "</table>";
#else
    UavsReportData data;
    DataCreat::creatUavsReportHead(list, data);
    DataCreat::creatUavsReportBody(list, data);
#endif
    QString html = list.join("");

    QTextDocument pdfDoc;
    pdfDoc.begin();
    pdfDoc.setHtml(html);
    pdfDoc.drawContents(&pdfPainter);
    pdfDoc.end();

    //绘制第4页内容 - 传入的富文本
    if (!content.isEmpty()) {
        pdfWriter.setPageOrientation(QPageLayout::Portrait);
        pdfWriter.newPage();
        pdfDoc.begin();
        pdfDoc.setHtml(content);
        pdfDoc.drawContents(&pdfPainter);
        pdfDoc.end();
    }

    //结束绘制并关闭文件
    pdfPainter.end();
    pdfFile.close();
#endif
}

void DataOther::toPdf(const QPixmap &pixmap, const QString &fileName, int scale)
{
    QPrinter printer(QPrinter::HighResolution);
    if (scale > 1) {
        printer.setResolution(96);
    }
    printer.setFullPage(false);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
#if (QT_VERSION >= QT_VERSION_CHECK(5,3,0))
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
#else
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
#endif

    //调整图片大小比如等比例缩放拉伸填充等
    QRectF rect = printer.pageRect(QPrinter::DevicePixel);
    QPixmap pix = pixmap;

    //保存原图看下效果
#if 0
    QString file = fileName;
    file.replace("pdf", "png");
    pix.save(file, "png");
#endif

    if (scale == 0) {
        //pix = pix.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio, Qt::FastTransformation);
        pix = pix.scaled(rect.width(), rect.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else if (scale == 1) {
        pix = pix.scaled(rect.width(), rect.height());
    }

    QPainter painter;
    painter.begin(&printer);

    int x = 0;
    int y = 0;
    //图片宽度小于绘制区域宽度没有缩放处理过的图片 才需要按照比例自动居中绘制
    if (pixmap.width() < rect.width() && scale > 1) {
        x = rect.center().x() - pix.width() / 2;
        //y = rect.center().y() - pix.height() / 2;
    }

    painter.drawPixmap(QPoint(x, y), pix);
    painter.end();
}

void DataOther::widgetToPdf(QWidget *widget, const QString &fileName, int scale)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QPixmap pixmap = widget->grab();
#else
    QPixmap pixmap = QPixmap::grabWindow(widget->winId());
#endif
    toPdf(pixmap, fileName, scale);
}

QString DataOther::imageToPdf(const QString &imageFile, const QString &pdfFile, int scale)
{
    //为空则同名文件
    QString fileName = pdfFile;
    if (fileName.isEmpty()) {
        fileName = imageFile;
        fileName.replace("." + QFileInfo(imageFile).suffix(), ".pdf");
    }

    //资源文件则当前目录下
    if (imageFile.startsWith(":/")) {
        fileName = qApp->applicationDirPath() + "/temp/" + QFileInfo(imageFile).baseName() + ".pdf";
    }

    toPdf(QPixmap(imageFile), fileName, scale);
    return fileName;
}

void DataOther::toPrint(const QPixmap &pixmap, const QString &fileName)
{
    //保存图片
    QString imageFile = qApp->applicationDirPath() + "/temp/temp.jpg";
    pixmap.save(imageFile, "jpg");

    //根据系统缩放计算合适的尺寸
    qreal ratio = DataHelper::getScreenRatio();
    qreal width = (qreal)pixmap.width() / ratio;
    qreal height = (qreal)pixmap.height() / ratio;

    //准备数据结构体
    DataContent dataContent;    
    //dataContent.html = QString("<img src='%1'>").arg(imageFile);
    dataContent.html = QString("<img src='%1' width=%2 height=%3>").arg(imageFile).arg(width).arg(height);

    //调用打印类打印
    DataPrint dataPrint;
    if (fileName.isEmpty()) {
        dataPrint.setDataContent(dataContent);
        dataPrint.print();
    } else {
        dataContent.fileName = fileName;
        dataPrint.setDataContent(dataContent);
        dataPrint.open();
        dataPrint.takeContent();
        dataPrint.close();
    }

    //删除临时图片
    QFile::remove(imageFile);
}

void DataOther::widgetToPrint(QWidget *widget)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QPixmap pixmap = widget->grab();
#else
    QPixmap pixmap = QPixmap::grabWindow(widget->winId());
#endif
    toPrint(pixmap);
}

void DataOther::imageToPrint(const QString &imageFile)
{
    toPrint(QPixmap(imageFile));
}

void DataOther::chartToPrint(int width, int height)
{
#ifdef qchart
    QPieSeries *series = new QPieSeries;
    series->append("Jane", 1);
    series->append("Joe", 2);
    series->append("Andy", 3);
    series->append("Barbara", 4);
    series->append("Axel", 5);

    QPieSlice *slice = series->slices().at(1);
    slice->setExploded();
    slice->setLabelVisible();
    slice->setPen(QPen(Qt::darkGreen, 2));
    slice->setBrush(Qt::green);

    QChart *chart = new QChart;
    chart->addSeries(series);
    chart->setTitle(DATETIME);
    chart->legend()->hide();

    //设置背景区域圆角角度
    chart->setBackgroundRoundness(0);
    //设置内边界边距
    chart->setMargins(QMargins(0, 0, 0, 0));
    //设置外边界边距
    chart->layout()->setContentsMargins(0, 0, 0, 0);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(width, height);
    //移动到看不见的位置
    chartView->move(10000, 10000);

    //要显示后停顿下才能抓到图
    chartView->show();
    qApp->processEvents();
    //QPixmap pix = chartView->grab();
    //pix.save("d:/1.jpg", "jpg");
    //toPrint(pix);
    widgetToPrint(chartView);

    //搞定以后记得删除对象不然会内存泄漏
    chartView->deleteLater();
#endif
}

QString DataOther::csvToXls(DataContent &dataContent, const QString &csvFile, const QString &xlsFile, bool quatation, const QString &spliter)
{
    //为空则同名文件
    QString fileName = xlsFile;
    if (fileName.isEmpty()) {
        fileName = csvFile;
        fileName.replace("." + QFileInfo(csvFile).suffix(), ".xls");
    }

    //内容集合
    QStringList content;
    //字段名称
    QList<QString> columnNames;
    //字段宽度
    QList<int> columnWidths;

    //读取csv文件的内容
    QFile file(csvFile);
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        //采用文件流的形式读取速度最快
        QTextStream stream(&file);
        DataCsv::initTextStream(csvFile, &stream);
        stream.seek(0);

        //循环读取文件
        int row = 0;
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line.isEmpty()) {
                continue;
            }

            //删除特殊字符,这个字符会导致读取不到内容TNND
            QString pattern = "[\\x200B]";
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
            line.remove(QRegularExpression(pattern));
#else
            line.remove(QRegExp(pattern));
#endif

            row++;
            QStringList list = line.split(spliter);
            //第一行是字段
            if (row == 1) {
                int count = list.count();
                for (int i = 0; i < count; ++i) {
                    //去掉空字段名
                    QString columnName = list.at(i);
                    if (columnName.isEmpty()) {
                        continue;
                    }

                    columnNames << columnName;
                    //设置过则取设置好的
                    if (dataContent.columnWidths.count() > i) {
                        columnWidths << dataContent.columnWidths.at(i);
                    } else {
                        columnWidths << 90;
                    }
                }
                continue;
            }

            QString separator = dataContent.separator;
            //处理半角引号括起来的是一整行,大部分的场景不需要,不会出现这种复杂的带引号和换行的内容
            if (quatation && checkQuatation(content, line, spliter, separator)) {
                continue;
            }

            //每行数据作为一个整体字符串带分割符 ; 存入
            content << list.join(separator);
        }
    }

    if (content.count() > 0) {
        //填充内容
        dataContent.content = content;
        //设置列名列宽
        dataContent.columnNames = columnNames;
        dataContent.columnWidths = columnWidths;
        //设置文件名
        dataContent.fileName = fileName;
        //调用静态函数导出
        DataXls::saveXls(dataContent);
    }

    return fileName;
}

bool DataOther::checkQuatation(QStringList &content, const QString &line, const QString &spliter, const QString &separator)
{
    static QString textStart, textEnd;
    //存在文本说明中间部分需要一直累加
    if (!textEnd.isEmpty()) {
        //如果分隔符不是 ; 则中间加上 &#10; 换行符会自动换行
        if (separator == ";") {
            textEnd = textEnd + line;
        } else {
            textEnd = textEnd + "&#10;" + line;
        }
    }

    if (line.contains("\"")) {
        //存在文本并且又来了一次引号说明是结束
        if (!textEnd.isEmpty()) {
            //移除对应的引号
            textEnd.replace("\"", "");
            //将前面和后面部分加起来
            QString text = textStart + textEnd;
            //将分隔符替换成需要的分隔符
            text.replace(spliter, separator);
            //qDebug() << "xxx" << text;
            //添加到队列
            content << text;
            //清空数据等待下一次该格式的数据
            textEnd.clear();
        } else {
            //把引号分割前面的存起来
            QStringList l = line.split("\"");
            textStart = l.first();
            textEnd = l.last();
            //可能有些不是 "2022 而是换行了
            if (textEnd.isEmpty()) {
                //主动加个字符就不会跳过下面这个判断
                textEnd = "\"";
            }
        }

        return true;
    }

    //存在文本说明引号还没结束
    if (!textEnd.isEmpty()) {
        return true;
    }

    return false;
}
