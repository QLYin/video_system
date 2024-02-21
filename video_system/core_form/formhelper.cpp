#include "formhelper.h"
#include "dbquery.h"
#include "qthelper.h"
#include "datahelper.h"

void FormHelper::reboot()
{
#ifndef Q_OS_WASM
    AppData::IsReboot = true;
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    qApp->closeAllWindows();
#endif
}

QString FormHelper::getDefaultDir()
{
#ifdef __arm__
    QString defaultDir = "/udisk";
#ifdef arma7
    //默认插入U盘后,会自动在/media/root下生成对应文件夹,该文件夹下还有固定的boot rootfs两个文件夹
    defaultDir = "/media/root";
    QDir dir(defaultDir);
    QStringList list = dir.entryList();
    foreach (QString name, list) {
        if (!name.startsWith("boot") && !name.startsWith("root") && !name.startsWith("BOOT") && !name.contains(".")) {
            defaultDir = defaultDir + "/" + name;
            break;
        }
    }
#endif
#else
    QString defaultDir = QtHelper::appPath();
#endif
    return defaultDir;
}

void FormHelper::saveSelectDirName(const QString &fileName)
{
    //自动保存最后一次选择的文件夹路径
    if (fileName.length() > 0) {
        QFileInfo fileInfo(fileName);
        AppConfig::SelectDirName = fileInfo.absolutePath();
        AppConfig::writeConfig();
    }
}

QString FormHelper::getOpenFileName(const QString &defaultName, const QString &filter)
{
    //第四个参数用来标识是否采用本地对话框
    QString fileName = QtHelper::getOpenFileName(filter, AppConfig::SelectDirName, defaultName);
    saveSelectDirName(fileName);
    return fileName;
}

QString FormHelper::getSaveFileName(const QString &defaultName, const QString &filter)
{
    //第四个参数用来标识是否采用本地对话框
    QString fileName = QtHelper::getSaveFileName(filter, AppConfig::SelectDirName, defaultName);
    saveSelectDirName(fileName);
    return fileName;
}

bool FormHelper::inputData(QSqlTableModel *model, const QStringList &columnNames, const QString &tableName, const QString &flag)
{
    QString name = flag + DataCsv::CsvExtension;
    QString fileName = FormHelper::getOpenFileName(name, DataCsv::CsvFilter);
    bool ok = DataCsv::inputData(columnNames.count(), "", tableName, fileName, true);
    if (!fileName.isEmpty()) {
        QString msg = QString("导入%1").arg(flag);
        DbQuery::addUserLog("用户操作", msg);
        if (ok) {
            QtHelper::showMessageBoxInfo(msg + "成功!", 3);
            model->select();
            return true;
        } else {
            QtHelper::showMessageBoxError(msg + "失败!", 3);
        }
    }
    return false;
}

bool FormHelper::outputData(const QString &orderColumn, const QStringList &columnNames, const QString &tableName, const QString &flag)
{
    //拿到要导出的数据
    QString columns = "*";
    QString where = orderColumn.isEmpty() ? "" : QString("order by %1").arg(orderColumn);
    QString title = columnNames.join(DataCsv::CsvSpliter);
    QStringList content = DataCsv::getContent(tableName, columns, where, title);

    QString name = flag + DataCsv::CsvExtension;
    QString fileName = FormHelper::getSaveFileName(name, DataCsv::CsvFilter);
    bool ok = DataCsv::outputData(fileName, content);
    if (!fileName.isEmpty()) {
        QString msg = QString("导出%1").arg(flag);
        DbQuery::addUserLog("用户操作", msg);
        if (ok) {
            QtHelper::showMessageBoxInfo(msg + "成功!", 3);
            return true;
        } else {
            QtHelper::showMessageBoxError(msg + "失败!", 3);
        }
    }
    return false;
}

bool FormHelper::outputData(const QString &tableName, const QString &flag)
{
    QStringList content = DataCsv::getContent(tableName, "*");
    QString name = QString("%1/db/%2%3").arg(QtHelper::appPath()).arg(flag).arg(DataCsv::CsvExtension);
    return DataCsv::outputData(name, content);
}

bool FormHelper::checkRowCount(quint8 type, int rowCount)
{
    int maxCount, warnCount;
    DataHelper::getMaxCount(type, maxCount, warnCount);
    return QtHelper::checkRowCount(rowCount, maxCount, warnCount);
}

void FormHelper::dataout(quint8 type, const QList<QString> &columnNames, const QList<int> &columnWidths,
                         const QString &name, const QString &table,
                         const QString &columns, const QString &where,
                         bool landscape, bool nameWithDate,
                         const QStringList &listContent)
{
    //arm上暂时不支持打印
    if (type == 0) {
#ifdef __arm__
        return;
#endif
    }

    QString suffix = "pdf";
    if (type == 2) {
        suffix = "xls";
    }

    //弹出保存文件对话框
    QString fileName;
    if (type == 1 || type == 2) {
        QString defaultName;
        if (nameWithDate) {
            defaultName = QString("%1_%2.%3").arg(name).arg(STRDATETIME).arg(suffix);
        } else {
            defaultName = QString("%1.%2").arg(name).arg(suffix);
        }

        QString filter = QString("保存文件(*.%1)").arg(suffix);
        fileName = FormHelper::getSaveFileName(defaultName, filter);
        if (fileName.isEmpty()) {
            return;
        }
    }

    //拿到要导出的数据集合
    QStringList content;
    if (listContent.count() == 0) {
        content = DataCsv::getContent(table, columns, where, "", ";");
    } else {
        content = listContent;
    }
    if (content.count() == 0) {
        return;
    }

    DataContent dataContent;
    //pdf和xls需要设置文件名
    if (type == 1 || type == 2) {
        dataContent.fileName = fileName;
    }
    //xls需要设置表名
    if (type == 2) {
        dataContent.sheetName = name;
    }

    dataContent.title = name;
    dataContent.columnNames = columnNames;
    dataContent.columnWidths = columnWidths;
    dataContent.content = content;
    dataContent.landscape = landscape;
    DataHelper::dataout(dataContent);

    //插入日志记录+询问打开文件
    QString msg = QString("导出%1").arg(name);
    if (type == 0) {
        msg = QString("打印%1").arg(name);
    }

    DbQuery::addUserLog(msg);
    QtHelper::openFile(fileName, msg);
}

void FormHelper::checkPosition(int &x, int &y, int width, int height)
{
    //递增X坐标
    if (x < AppData::MapWidth - 100) {
        x = x + width;
    } else {
        //到了最右侧重新设置X坐标
        x = 5;
        y = y + height;
    }

    //到了最底部重新设置Y坐标
    if (y > AppData::MapHeight - 100) {
        y = 5;
    }
}
