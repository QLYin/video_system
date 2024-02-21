#include "datahelper.h"

int DataHelper::getScreenIndex()
{
    //需要对多个屏幕进行处理
    int screenIndex = 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    int screenCount = qApp->screens().count();
#else
    int screenCount = qApp->desktop()->screenCount();
#endif

    if (screenCount > 1) {
        //找到当前鼠标所在屏幕
        QPoint pos = QCursor::pos();
        for (int i = 0; i < screenCount; ++i) {
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
            if (qApp->screens().at(i)->geometry().contains(pos)) {
#else
            if (qApp->desktop()->screenGeometry(i).contains(pos)) {
#endif
                screenIndex = i;
                break;
            }
        }
    }
    return screenIndex;
}

qreal DataHelper::getScreenRatio(bool devicePixel)
{
    qreal ratio = 1.0;
    int screenIndex = getScreenIndex();
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QScreen *screen = qApp->screens().at(screenIndex);
    if (devicePixel) {
        //需要开启 AA_EnableHighDpiScaling 属性才能正常获取
        ratio = screen->devicePixelRatio() * 100;
    } else {
        ratio = screen->logicalDotsPerInch();
    }
#else
    //Qt4不能动态识别缩放更改后的值
    ratio = qApp->desktop()->screen(screenIndex)->logicalDpiX();
#endif
    return ratio / 96;
}

bool DataHelper::checkColumn(const QString &sourceValue, const QString &checkType, const QString &checkValue)
{
    bool exist = false;

    //转成 QByteArray 比较就涵盖了所有数据类型
    //最开始用的 QVariant 后面发现Qt4不支持
#if 0
    QVariant sourceValue1 = sourceValue;
    QVariant checkValue1 = checkValue;

    //checkType如果后面带了数据类型(==|int >|double)则转为对应的数据类型
    QString checkType1 = checkType;
    QStringList list = checkType.split("|");
    if (list.count() == 2) {
        //将正确的比较类型取出来
        checkType1 = list.first();
        //可以自行增加其他类型
        QString type = list.last();
        if (type == "int") {
            sourceValue1 = sourceValue.toInt();
            checkValue1 = checkValue.toInt();
        } else if (type == "float") {
            sourceValue1 = sourceValue.toFloat();
            checkValue1 = checkValue.toFloat();
        } else if (type == "double") {
            sourceValue1 = sourceValue.toDouble();
            checkValue1 = checkValue.toDouble();
        }
    }

    if (checkType1 == "==") {
        exist = (sourceValue1 == checkValue1);
    } else if (checkType1 == ">") {
        exist = (sourceValue1 > checkValue1);
    } else if (checkType1 == ">=") {
        exist = (sourceValue1 >= checkValue1);
    } else if (checkType1 == "<") {
        exist = (sourceValue1 < checkValue1);
    } else if (checkType1 == "<=") {
        exist = (sourceValue1 <= checkValue1);
    } else if (checkType1 == "!=") {
        exist = (sourceValue1 != checkValue1);
    } else if (checkType1 == "contains") {
        exist = sourceValue.contains(checkValue);
    }
#else
    QByteArray sourceValue1;
    QByteArray checkValue1;

    //checkType如果后面带了数据类型(==|int >|double)则转为对应的数据类型
    QString checkType1 = checkType;
    QStringList list = checkType.split("|");
    if (list.count() == 2) {
        sourceValue1.clear();
        checkValue1.clear();
        //将正确的比较类型取出来
        checkType1 = list.first();
        //可以自行增加其他类型
        QString type = list.last();
        if (type == "int") {
            sourceValue1.append(sourceValue.toInt());
            checkValue1.append(checkValue.toInt());
        } else if (type == "float") {
            sourceValue1.append(sourceValue.toFloat());
            checkValue1.append(checkValue.toFloat());
        } else if (type == "double") {
            sourceValue1.append(sourceValue.toDouble());
            checkValue1.append(checkValue.toDouble());
        } else {
            sourceValue1.append(sourceValue.toUtf8());
            checkValue1.append(checkValue.toUtf8());
        }
    } else {
        sourceValue1.append(sourceValue.toUtf8());
        checkValue1.append(checkValue.toUtf8());
    }

    if (checkType1 == "==") {
        exist = (sourceValue1 == checkValue1);
    } else if (checkType1 == ">") {
        exist = (sourceValue1 > checkValue1);
    } else if (checkType1 == ">=") {
        exist = (sourceValue1 >= checkValue1);
    } else if (checkType1 == "<") {
        exist = (sourceValue1 < checkValue1);
    } else if (checkType1 == "<=") {
        exist = (sourceValue1 <= checkValue1);
    } else if (checkType1 == "!=") {
        exist = (sourceValue1 != checkValue1);
    } else if (checkType1 == "contains") {
        exist = sourceValue.contains(checkValue);
    }
#endif

    //qDebug() << TIMEMS << exist << sourceValue << checkType << checkValue;
    return exist;
}

void DataHelper::getMaxCount(quint8 type, int &maxCount, int &warnCount)
{
    maxCount = 5000;
    warnCount = 500;
    if (type == 2) {
        maxCount = 100000;
        warnCount = 10000;
    }
}

QString DataHelper::getAlignStyle(const DataContent &dataContent, quint8 type, int column)
{
    //找出对应列名的对齐方式
    //这里不用担心越界问题,外边已经处理好了队列数量永远一致
    int align = 0;
    if (type == 0) {
        align = dataContent.columnAlignment.at(column);
    } else {
        align = dataContent.contentAlignment.at(column);
    }

    QString style = "center";
    if (align == 1) {
        style = "left";
    } else if (align == 2) {
        style = "right";
    }

    return style;
}

bool DataHelper::checkDataContent(DataContent &dataContent)
{
    //校验字段名称和宽度,个数不能为0,而且字段名称的个数要和字段宽度的个数一致
    int columnCount = dataContent.columnNames.count();
    int widthCount = dataContent.columnWidths.count();
    if (columnCount == 0 || columnCount != widthCount) {
        qDebug() << TIMEMS << "传入的列名数量和列宽数量不一致";
        return false;
    }

    //自动补齐列对齐和内容对齐队列
    //没有设置的对应列需要设置下
    //防止没有对应的对齐值下面取值会溢出
    int count1 = dataContent.columnAlignment.count();
    int count2 = dataContent.contentAlignment.count();
    int align = dataContent.defaultAlignment;
    for (int i = 0; i < columnCount; ++i) {
        if (i >= count1) {
            dataContent.columnAlignment << align;
        }
        if (i >= count2) {
            dataContent.contentAlignment << align;
        }
    }

    return true;
}

void DataHelper::init()
{
    //没有实例化过则先实例化
    if (DataPrint::dataPrint == 0) {
        DataPrint::dataPrint = new DataPrint;
    }
    if (DataXls::dataXls == 0) {
        DataXls::dataXls = new DataXls;
    }
}

void DataHelper::dataout(const DataContent &dataContent)
{
    //判断规则
    //存在 fileName+sheetName 则表示导出 xls
    //存在 fileName 则表示导出 pdf
    //其余表示打印
    if (!dataContent.sheetName.isEmpty()) {
        DataXls::saveXls(dataContent);
    } else if (!dataContent.fileName.isEmpty()) {
        DataPrint::savePdf(dataContent);
    } else {
        DataPrint::print(dataContent);
    }
}

void DataHelper::dataout(const QString &fileName, const QString &sheetName, const QString &title,
                         const QList<QString> &columnNames, const QList<int> &columnWidths,
                         const QStringList &content, bool landscape)
{
    DataContent dataContent;
    dataContent.fileName = fileName;
    dataContent.sheetName = sheetName;
    dataContent.title = title;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = landscape;
    dataout(dataContent);
}

QString DataHelper::dataout(QTableView *table, QStandardItemModel *model, quint8 type, const QString &file, const QString &title, const QString &sheet)
{
    //从数据模型拿到行列
    int row = model->rowCount();
    int column = model->columnCount();
    if (row == 0 || column == 0) {
        return QString();
    }

    //获取数据集合
    QStringList content;
    for (int i = 0; i < row; ++i) {
        QStringList list;
        for (int j = 0; j < column; ++j) {
            list << model->item(i, j)->text();
        }
        //每行数据作为一个整体字符串带分割符 ; 存入
        content << list.join(";");
    }

    //获取列名和列宽
    QList<QString> columnNames;
    QList<int> columnWidths;
    for (int i = 0; i < column; ++i) {
        columnNames << model->headerData(i, Qt::Horizontal).toString();
        columnWidths << table->columnWidth(i);
    }

    return dataout(type, file, title, sheet, content, columnNames, columnWidths);
}

QString DataHelper::dataout(QTableWidget *table, quint8 type, const QString &file, const QString &title, const QString &sheet)
{
    //从数据模型拿到行列
    int row = table->rowCount();
    int column = table->columnCount();
    if (row == 0 || column == 0) {
        return QString();
    }

    //获取数据集合
    QStringList content;
    for (int i = 0; i < row; ++i) {
        QStringList list;
        for (int j = 0; j < column; ++j) {
            list << table->item(i, j)->text();
        }
        //每行数据作为一个整体字符串带分割符 ; 存入
        content << list.join(";");
    }

    //获取列名和列宽
    QList<QString> columnNames;
    QList<int> columnWidths;
    for (int i = 0; i < column; ++i) {
        columnNames << table->horizontalHeaderItem(i)->text();
        columnWidths << table->columnWidth(i);
    }

    return dataout(type, file, title, sheet, content, columnNames, columnWidths);
}

QString DataHelper::dataout(quint8 type, const QString &file, const QString &title,
                            const QString &sheet, const QStringList &content,
                            const QStringList &columnNames, const QList<int> &columnWidths)
{
    //设置结构体数据
    DataContent dataContent;
    //填充内容
    dataContent.content = content;
    //设置列名列宽
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    //设置标题
    dataContent.title = title;
    //设置默认对齐
    dataContent.defaultAlignment = 0;

    //如果是导出数据则先判断文件名称是否为空,为空则弹出文件对话框选择
    QString fileName = file;
    if (fileName.isEmpty() && type < 3) {
        //不同的格式后缀
        QString filter = "all files (*.*)";
        if (type == 0) {
            filter = "csv files (*.csv)";
        } else if (type == 1) {
            filter = "xls files (*.xls)";
        } else if (type == 2) {
            filter = "pdf files (*.pdf)";
        }

        fileName = QFileDialog::getSaveFileName(0, "选择保存文件", "", filter);
        if (fileName.isEmpty()) {
            return fileName;
        }
    }

    //设置文件名
    dataContent.fileName = fileName;
    //xls还需要设置表名
    dataContent.sheetName = sheet;

    //调用静态函数
    if (type == 0) {
        //重新组织内容,前面写入标题,分隔符换成csv的定义的分隔符
        QStringList list;
        //把标题加到内容中
        list << columnNames.join(DataCsv::CsvSpliter);
        //重新更换分隔符
        foreach (QString text, content) {
            text.replace(";", DataCsv::CsvSpliter);
            list << text;
        }
        DataCsv::outputData(fileName, list);
    } else if (type == 1) {
        DataXls::saveXls(dataContent);
    } else if (type == 2) {
        DataPrint::savePdf(dataContent);
    } else if (type == 3) {
        DataPrint::print(dataContent);
    }

    return fileName;
}
