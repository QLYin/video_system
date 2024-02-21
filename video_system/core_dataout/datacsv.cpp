#include "datacsv.h"

bool DataCsv::CsvString = false;
QString DataCsv::CsvFilter = QString::fromUtf8("csv文件(*.csv)");
QString DataCsv::CsvExtension = ".csv";
QString DataCsv::CsvSpliter = ",";

QStringList DataCsv::getContent(const QString &tableName,
                                const QString &columnNames,
                                const QString &whereSql,
                                const QString &title,
                                const QString &spliter)
{
    QStringList content;
    QString sql = QString("select %1 from %2 %3").arg(columnNames).arg(tableName).arg(whereSql);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return content;
    }

    //如果标题字段名称不为空则先添加标题
    if (!title.isEmpty()) {
        content << title;
    }

    int columnCount = query.record().count();
    while (query.next()) {
        QStringList list;
        for (int i = 0; i < columnCount; ++i) {
            list << query.value(i).toString();
        }

        content << list.join(spliter);
    }

    return content;
}

void DataCsv::initTextStream(const QString &fileName, QTextStream *stream)
{
    //cvs格式如果是utf8编码在excel中打开乱码(wps正常),csv默认中文支持ANSI编码即GBK
    //如果是utf8带bom那在excel中打开也正常,最终选用这个策略
    if (fileName.endsWith(".csv")) {
        //探测下具体文件是utf8还是gbk然后再设置对应的编码
        QString flag;
        bool ansi = (findCode(fileName, flag) == 0);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
        stream->setCodec(ansi ? "gbk" : "utf-8");
#else
        stream->setEncoding(ansi ? QStringConverter::System : QStringConverter::Utf8);
#endif

        //设置自动unicode
        stream->setAutoDetectUnicode(true);
        //设置带BOM
        stream->setGenerateByteOrderMark(true);
    }
}

bool DataCsv::existBom(const QString &fileName, bool rewrite)
{
    QByteArray data;
    bool exist = false;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        if (rewrite) {
            data = file.readAll();
            QString flag = data.left(3).toHex().toUpper();
            if (flag == "EFBBBF") {
                exist = true;
                //去掉前面bom数据
                data = data.right(data.length() - 3);
                file.close();
            }
        } else {
            //只读取前面三个字节就好
            uchar data[4] = {0};
            file.read((char *)data, 3);
            if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF) {
                exist = true;
            }
        }

        file.close();
    }

    //去掉bom重新写入文件
    if (exist && rewrite) {
        //打开的时候清空文件内容
        if (file.open(QIODevice::WriteOnly | QFile::Truncate)) {
            file.write(data);
            file.close();
        }
    }

    return exist;
}

int DataCsv::findCode(const QString &fileName, QString &flag)
{
    //假定默认编码utf8
    int code = 3;
    flag = "UTF-8";

    QFile file(fileName);
    if (file.size() >= 3 && file.open(QIODevice::ReadOnly)) {
        //读取3字节用于判断
        QByteArray buffer = file.read(3);
        quint8 b1 = buffer.at(0);
        quint8 b2 = buffer.at(1);
        quint8 b3 = buffer.at(2);
        if (b1 == 0xFF && b2 == 0xFE) {
            code = 1;
            flag = "UTF-16LE";
        } else if (b1 == 0xFE && b2 == 0xFF) {
            code = 2;
            flag = "UTF-16BE";
        } else if (b1 == 0xEF && b2 == 0xBB && b3 == 0xBF) {
            code = 4;
            flag = "UTF-8BOM";
        } else {
            //尝试用utf8转换,如果可用字符数大于0,则表示是ansi编码
            QTextCodec::ConverterState state;
            QTextCodec *codec = QTextCodec::codecForName("utf-8");
            codec->toUnicode(buffer.constData(), buffer.length(), &state);
            if (state.invalidChars > 0) {
                code = 0;
                flag = "ANSI";
            }
        }

        file.close();
    }

    return code;
}

bool DataCsv::inputData(int columnCount,
                        const QString &columnNames,
                        const QString &tableName,
                        QString &fileName,
                        const QString &defaultDir,
                        bool existTitle)
{
    fileName = QFileDialog::getOpenFileName(0, "选择文件", defaultDir, CsvFilter);
    return inputData(columnCount, columnNames, tableName, fileName, existTitle);
}

bool DataCsv::inputData(int columnCount, const QString &columnNames, const QString &tableName, QString &fileName, bool existTitle)
{
    if (fileName.isEmpty()) {
        return false;
    }

    QFile file(fileName);
    bool ok = file.open(QIODevice::ReadOnly | QFile::Text);
    if (!ok) {
        return false;
    }

    //传入了字段集合则取字段
    if (!columnNames.isEmpty()) {
        columnCount = columnNames.split(",").count();
    }

    //先读取第一行判断列数是否和目标列数一致,不一致则返回
    QString line = QString::fromUtf8(file.readLine());
    QStringList list = line.split(CsvSpliter);
    if (list.count() != columnCount) {
        return false;
    }

    //先删除原来的数据
    QString sql = QString("delete from %1").arg(tableName);
    QSqlQuery query;
    if (!query.exec(sql)) {
        qDebug() << TIMEMS << query.lastError().text() << sql;
        return false;
    }

    //采用文件流的形式读取速度最快
    QTextStream stream(&file);
    initTextStream(fileName, &stream);
    stream.seek(0);

    //开启数据库事务加速处理
    QSqlDatabase::database().transaction();

    bool isremoveTitle = false;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        //如果存在标题则不需要处理第一行标题
        if (existTitle && !isremoveTitle) {
            isremoveTitle = true;
            continue;
        }

        //列数必须完全一致才行
        QStringList list = line.split(CsvSpliter);
        if (list.count() == columnCount) {
            //指定了列名则插入的时候也要指定列名
            if (!columnNames.isEmpty()) {
                sql = QString("insert into %1(%2) values").arg(tableName).arg(columnNames);
            } else {
                sql = QString("insert into %1 values").arg(tableName);
            }

            //字段值集合
            QStringList values;
            for (int i = 0; i < columnCount; ++i) {
                QString text = list.at(i).trimmed();
                //如果启用了字符串模式则需要去掉前后字符
                if (CsvString && text.startsWith("=\"")) {
                    int len = text.length();
                    text = text.mid(2, len - 3);
                }
                values << text;
            }

            //组成新的sql语句
            QString sqlx = QString("%1('%2')").arg(sql).arg(values.join("','"));
            query.clear();
            query.exec(sqlx);
        }
    }

    //提交数据库事务
    if (!QSqlDatabase::database().commit()) {
        QSqlDatabase::database().rollback();
        return false;
    }

    file.close();
    return true;
}

bool DataCsv::outputData(const QString &defaultName,
                         const QStringList &content,
                         QString &fileName,
                         const QString &defaultDir)
{
    QString defaultPath = QString("%1/%2").arg(defaultDir).arg(defaultName);
    fileName = QFileDialog::getSaveFileName(0, "选择文件", defaultPath, CsvFilter);
    return outputData(fileName, content);
}

bool DataCsv::outputData(QString &fileName, const QStringList &content)
{
    if (fileName.isEmpty()) {
        return false;
    }

    int rowCount = content.count();
    if (rowCount == 0) {
        fileName.clear();
        return false;
    }

    //自动补全拓展名
    if (!fileName.endsWith(CsvExtension)) {
        fileName = fileName + CsvExtension;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        QTextStream stream(&file);
        initTextStream(fileName, &stream);
        for (int i = 0; i < rowCount; ++i) {
            QString text = content.at(i);
            //保证字符串中前面的0以及其他内容不被优化
            if (CsvString) {
                QStringList line;
                QStringList list = text.split(CsvSpliter);
                int count = list.count();
                for (int j = 0; j < count; ++j) {
                    line << QString("=\"%1\"").arg(list.at(j));
                }
                text = line.join(CsvSpliter);
            }
            stream << text << "\n";
        }

        file.close();
    }

    return true;
}
