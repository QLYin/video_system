#include "dataxls.h"
#include "datahelper.h"

DataXls *DataXls::dataXls = 0;
void DataXls::saveXls(const DataContent &dataContent)
{
    //没有实例化过则先实例化
    if (dataXls == 0) {
        dataXls = new DataXls;
    }

    //没有内容数据则不用处理
    if (dataContent.content.count() == 0) {
        return;
    }

    dataXls->init();
    dataXls->setDataContent(dataContent);
    dataXls->open();
    dataXls->takeContent();
    dataXls->close();
}

void DataXls::saveXls(const QString &fileName, const QString &sheetName, const QString &title,
                      const QList<QString> &columnNames, const QList<int> &columnWidths,
                      const QStringList &content)
{
    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.sheetName = sheetName;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    DataXls::saveXls(dataContent);
}

DataXls::DataXls(QObject *parent) : QThread(parent)
{
    this->init();
}

DataXls::~DataXls()
{
    this->stop();
    this->wait();
}

void DataXls::run()
{
    time.restart();
    while (!stopped) {
        takeContent();
        //可以自行调整下面这个间隔以便保证完整输出
        //发现有些电脑需要延时久一点
        msleep(10);
    }

    stopped = false;
}

QString DataXls::getUseTime()
{
    return QString::number((float)time.elapsed() / 1000, 'f', 3);
}

QString DataXls::getDataType(const QString &data)
{
    //自动识别数据类型字符串
    QString type = "String";
    //0开头的统一用字符串
    if (data == "0") {
        type = "Number";
    } else if (data.startsWith("0") && !data.startsWith("0.")) {
        type = "String";
    } else if (dataContent.autoType) {
        //长度超过最大值也转成字符串显示比较合理
        bool ok = false;
        double value = data.toDouble(&ok);
        if (value < 0x7fffffff && ok) {
            type = "Number";
        }
    }

    return type;
}

void DataXls::appendHead()
{
    //固定头部信息
    stream << "<?xml version=\"1.0\"?>\n";
    stream << "<?mso-application progid=\"Xls.Sheet\"?>\n";
    stream << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    stream << " xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n";
    stream << " xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n";
    stream << " xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n";
    stream << " xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n";
}

void DataXls::appendDocument()
{
    //文档信息
    stream << " <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n";
    stream << QString("  <LastAuthor>%1</LastAuthor>\n").arg("AAA");
    stream << QString("  <Created>%1</Created>\n").arg(DATETIME);
    stream << QString("  <LastSaved>%1</LastSaved>\n").arg(DATETIME);
    stream << "  <Version>11.6360</Version>\n";
    stream << " </DocumentProperties>\n";

    stream << " <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n";
    stream << "  <AllowPNG/>\n";
    stream << " </OfficeDocumentSettings>\n";

    stream << " <XlsWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";
    stream << "  <WindowHeight>9795</WindowHeight>\n";
    stream << "  <WindowWidth>21435</WindowWidth>\n";
    stream << "  <WindowTopX>120</WindowTopX>\n";
    stream << "  <WindowTopY>75</WindowTopY>\n";
    stream << "  <WindowTopY>75</WindowTopY>\n";
    stream << "  <ProtectWindows>False</ProtectWindows>\n";
    stream << " </XlsWorkbook>\n";
}

void DataXls::appendStyle()
{
    QString align = "Center";
    if (dataContent.defaultAlignment == 1) {
        align = "Left";
    } else if (dataContent.defaultAlignment == 2) {
        align = "Right";
    }

    //样式表集合,先定义好,后面直接通过标识符引入即可
    stream << " <Styles>\n";

    //默认样式--没有指定样式的单元格就采用这个样式
    appendStyle("Default", align, false);
    //标题样式--居中大号加粗字体
    appendStyle("s_title", "Center", true, 15, 1);
    //子标题样式--左对齐普通字体
    appendStyle("s_subtitle", "Left", true, 10, 1);
    //正文样式--居中对齐
    appendStyle("s_center", "Center", true);
    //正文样式--左对齐
    appendStyle("s_left", "Left", true);
    //正文样式--右对齐
    appendStyle("s_right", "Right", true);
    //正文样式--检验颜色
    appendStyle("s_check", align, true, 10, 0, dataContent.checkColor);

    //生成多个带背景色的样式
    if (dataContent.randomColor) {
        QStringList colorName;
        colorName << "#FF99CC" << "#FFCC99" << "#FF8080" << "#8DB4E2" << "#EBF1DE" << "#CC99FF" << "#99CC00" << "#99CCFF" << "#FFFF00" << "#FF6600";
        for (int i = 0; i < 10; i++) {
            appendStyle(QString("s_color_%1").arg(i), "Center", true, 10, 0, "#000000", colorName.at(i));
        }
    }

    stream << " </Styles>\n";
}

void DataXls::appendStyle(const QString &id, const QString &align,
                          bool border, int fontSize, int fontBold,
                          const QString &textColor, const QString &bgColor)
{
    //样式唯一标识
    stream << QString("  <Style ss:ID=\"%1\">\n").arg(id);
    //文字对齐方式
    stream << QString("   <Alignment ss:Horizontal=\"%1\" ss:Vertical=\"Center\" ss:WrapText=\"%2\"/>\n").arg(align).arg(dataContent.wrapText);

    //如果边框宽度为0则不显示边框
    int borderWidth = dataContent.borderWidth;
    //开启单元格样式才有边框
    if (border && borderWidth > 0 && dataContent.cellStyle) {
        stream << "  <Borders>\n";
        stream << QString("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << QString("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"%1\"/>\n").arg(borderWidth);
        stream << "  </Borders>\n";
    }

    //文字字体
    stream << QString("   <Font ss:FontName=\"Microsoft Yahei\" x:CharSet=\"134\" ss:Size=\"%1\" ss:Color=\"%3\" ss:Bold=\"%2\"/>\n").arg(fontSize).arg(fontBold).arg(textColor);
    //背景填充
    if (!bgColor.isEmpty()) {
        stream << QString("   <Interior ss:Color=\"%1\" ss:Pattern=\"Solid\"/>\n").arg(bgColor);
    }

    //数字格式
    //stream << "   <NumberFormat/>\n";
    stream << "  </Style>\n";
}

void DataXls::appendTable()
{
    //工作表名称
    stream << QString(" <Worksheet ss:Name=\"%1\">\n").arg(dataContent.sheetName);

    //表格开始
    stream << QString("  <Table ss:ExpandedColumnCount=\"%1\" x:FullColumns=\"1\"\n").arg(columnCount);
    stream << "   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n";

    //设置字段宽度
    for (int i = 0; i < columnCount; i++) {        
        stream << QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n").arg(dataContent.columnWidths.at(i));
    }

    //表格标题,如果没有则不输出
    if (!dataContent.title.isEmpty()) {
        stream << "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"22\">\n";
        stream << QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_title\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(dataContent.title);
        stream << "   </Row>";
    }

    //表格子标题,如果没有则不输出
    if (!dataContent.subTitle.isEmpty()) {
        stream << "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n";
        stream << QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(columnCount - 1).arg(dataContent.subTitle);
        stream << "   </Row>";
    }
}

void DataXls::appendColumn()
{
    //逐个添加字段名称
    if (columnCount > 0) {
        stream << "   <Row ss:AutoFitHeight=\"0\">\n";
        for (int i = 0; i < columnCount; i++) {
            QString style = DataHelper::getAlignStyle(dataContent, 0, i);
            stream << QString("    <Cell ss:StyleID=\"s_%1\"><Data ss:Type=\"String\">%2</Data></Cell>\n").arg(style).arg(dataContent.columnNames.at(i));
        }
        stream << "   </Row>\n";
    }
}

void DataXls::appendContent(const QStringList &listContent)
{
    //逐个添加数据
    int rowCount = listContent.count();
    for (int i = 0; i < rowCount; i++) {
        stream << QString("   <Row ss:AutoFitHeight=\"%1\">\n").arg(dataContent.autoFitHeight);
        QString temp = listContent.at(i);
        QStringList value = temp.split(dataContent.separator);

        if (dataContent.randomColor) {
            //如果启用了随机背景颜色则随机取背景颜色样式
            QString style = "s_center";
            //如果没有指定列随机背景颜色则对整行
            static int styleIndex = 0;
            for (int j = 0; j < columnCount; j++) {
                if (dataContent.colorColumn.count() == 0) {
                    style = QString("s_color_%1").arg(styleIndex);
                } else {
                    if (dataContent.colorColumn.contains(j)) {
                        style = QString("s_color_%1").arg(styleIndex);
                    } else {
                        style = "s_center";
                    }
                }

                QString data = value.at(j);
                QString type = getDataType(data);
                stream << QString("    <Cell ss:StyleID=\"%1\"><Data ss:Type=\"%2\">%3</Data></Cell>\n").arg(style).arg(type).arg(data);
            }

            styleIndex++;
            if (styleIndex > 9) {
                styleIndex = 0;
            }
        } else {
            //如果启用了过滤数据,则将符合条件的数据突出颜色显示
            bool exist = false;
            if (dataContent.checkColumn >= 0) {
                exist = DataHelper::checkColumn(value.at(dataContent.checkColumn), dataContent.checkType, dataContent.checkValue);
            }

            //每个单元格列填充数据
            for (int j = 0; j < columnCount; j++) {
                QString data = value.at(j);
                QString type = getDataType(data);

                //如果不采用样式则文件体积可以减少约30%
                QString style;
                //先判断是否符合条件数据,符合必须先设置
                if (exist) {
                    style = QString(" ss:StyleID=\"%1\"").arg("s_check");
                } else if (dataContent.cellStyle) {
                    style = DataHelper::getAlignStyle(dataContent, 1, j);
                    style = QString(" ss:StyleID=\"s_%1\"").arg(style);
                }

                stream << QString("    <Cell%1><Data ss:Type=\"%2\">%3</Data></Cell>\n").arg(style).arg(type).arg(data);
            }
        }

        stream << "   </Row>\n";
    }
}

void DataXls::takeContent()
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
            stream << "   <Row ss:AutoFitHeight=\"0\">\n";
            stream << "   </Row>";
            QString strRow = "   <Row ss:AutoFitHeight=\"0\" ss:Height=\"18\">\n";
            QString strCell = QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"s_subtitle\"><Data ss:Type=\"String\">").arg(columnCount - 1);

            //子标题1
            if (subTitle1Count > i) {
                if (!listSubTitle1.at(i).isEmpty()) {
                    stream << strRow;
                    stream << QString("%1%2</Data></Cell>\n").arg(strCell).arg(listSubTitle1.at(i));
                    stream << "   </Row>";
                }
            }

            //子标题2
            if (subTitle2Count > i) {
                if (!listSubTitle2.at(i).isEmpty()) {
                    stream << strRow;
                    stream << QString("%1%2</Data></Cell>\n").arg(strCell).arg(listSubTitle2.at(i));
                    stream << "   </Row>";
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

void DataXls::appendEnd()
{
    //表格结尾
    stream << "  </Table>\n";
    //固定结尾格式
    stream << "  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n";

    //页面设置比如横向和边距等
    stream << "   <PageSetup>\n";
    if (dataContent.landscape) {
        stream << "    <Layout x:Orientation=\"Landscape\"/>\n";
    }
    stream << "    <Header x:Margin=\"0.3\"/>\n";
    stream << "    <Footer x:Margin=\"0.3\"/>\n";
    stream << "    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n";
    stream << "   </PageSetup>\n";

    stream << "   <Unsynced/>\n";
    stream << "   <Selected/>\n";
    stream << "   <ProtectObjects>False</ProtectObjects>\n";
    stream << "   <ProtectScenarios>False</ProtectScenarios>\n";
    stream << "  </WorksheetOptions>\n";
    stream << " </Worksheet>\n";
    stream << "</Workbook>\n";
}

void DataXls::setDataContent(const DataContent &dataContent)
{
    this->dataContent = dataContent;
}

void DataXls::init()
{
    stopped = false;
}

void DataXls::open()
{
    columnCount = dataContent.columnNames.count();
    if (dataContent.fileName.isEmpty()) {
        return;
    }

    if (!DataHelper::checkDataContent(dataContent)) {
        return;
    }

    //如果打开了则先关闭
    if (file.isOpen()) {
        file.close();
    }

    //指定文件名称
    file.setFileName(dataContent.fileName);
    if (!file.open(QFile::WriteOnly | QIODevice::Text)) {
        return;
    }

    //设置流对象及编码
    stream.setDevice(&file);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    stream.setCodec("utf-8");
#endif

    //输出固定头部信息
    appendHead();
    //输出文档信息
    appendDocument();
    //输出边框样式
    appendStyle();
    //输出表结构
    appendTable();

    //输出字段名称
    if (dataContent.subTitle1.count() == 0) {
        appendColumn();
    }
}

void DataXls::close()
{
    if (!file.isOpen()) {
        return;
    }

    //输出固定尾部信息
    appendEnd();
    //关闭文件
    file.close();
}

void DataXls::stop()
{
    stopped = true;
}

void DataXls::append(const QString &content, const QString &subTitle1, const QString &subTitle2)
{
    if (content.isEmpty()) {
        return;
    }

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
