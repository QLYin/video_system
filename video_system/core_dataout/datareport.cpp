#include "datareport.h"

QString DataReport::getImageCode(const QString &image)
{
    //根据系统缩放计算合适的尺寸
    QImage img(image);
    qreal ratio = DataHelper::getScreenRatio();
    qreal width = (qreal)img.width() / ratio;
    qreal height = (qreal)img.height() / ratio;
    //QString code = QString("<img src='%1'>").arg(image);
    QString code = QString("<img src='%1' width=%2 height=%3>").arg(image).arg(width).arg(height);
    return code;
}

void DataReport::creatUavsReportHead(QStringList &list, const UavsReportData &reportData)
{
    //表格开始
    list << "<table border='0.0' cellspacing='0' cellpadding='6' style='font-size:18px';>";

    //标题
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:22px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(reportData.title);
    list << "</tr>";

    //横线或者换行
    list << "<hr>";
    //list << "<br>";

    //设备名称
    list << "<tr>";
    list << QString("<td colspan='%1'>%2</td>").arg(2).arg(QString("设备名称: %1").arg(reportData.name));
    list << "</tr>";

    //经度纬度
    list << "<tr>";
    list << QString("<td>%1</td>").arg(QString("设备经度: %1").arg(reportData.lng));
    list << QString("<td>%1</td>").arg(QString("设备纬度: %1").arg(reportData.lat));
    list << "</tr>";

    //告警日期+报告时间
    list << "<tr>";
    list << QString("<td>%1</td>").arg(QString("告警日期: %1").arg(reportData.timeAlarm));
    list << QString("<td>%1</td>").arg(QString("报告时间: %1").arg(reportData.timeReport));
    list << "</tr>";

    //表格结束
    list << "</table>";
}

void DataReport::creatUavsReportBody(QStringList &list, const UavsReportData &reportData, int imageWidth)
{
    //换行
    list << "<br><br>";

    //表格开始
    list << "<table border='0.5' cellspacing='0' cellpadding='5' style='font-size:18px;'>";

    //标题
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:20px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(reportData.subTitle);
    list << "</tr>";

    //告警类型
    list << "<tr>";
    list << QString("<td colspan='%1'>%2</td>").arg(2).arg(QString("告警类型: %1").arg(reportData.type));
    list << "</tr>";

    //其他信息
#if 0
    list << "<tr>";
    list << QString("<td colspan='%1'>%2</td>").arg(2).arg("");
    list << "</tr>";
    list << "<tr>";
    list << QString("<td colspan='%1'>%2</td>").arg(2).arg("");
    list << "</tr>";
    list << "<tr>";
    list << QString("<td colspan='%1'>%2</td>").arg(2).arg("");
    list << "</tr>";
#endif

    //目标数量+威胁等级
    list << "<tr>";
    list << QString("<td>%2</td>").arg(QString("目标数量: %1").arg(reportData.textCount));
    list << QString("<td>%2</td>").arg(QString("威胁等级: %1").arg(reportData.textLevel));
    list << "</tr>";

    //图像队列
    QStringList listImage = reportData.images;
    int count = listImage.count();

    //告警图像
    QString text = QString("告警图像 ( 共 %1 张 )").arg(count);
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:20px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(text);
    list << "</tr>";

    //放到临时目录
    QString path = qApp->applicationDirPath() + "/temp/";
    QDir dir(path);
    //目录不存在则新建目录
    if (!dir.exists()) {
        dir.mkdir(path);
    }

    //先清空临时目录所有文件
    QStringList files = dir.entryList(QStringList() << "*.*", QDir::Files);
    foreach (QString file, files) {
        QFile::remove(path + file);
    }

    //按照时间和序号
    QString time = QTime::currentTime().toString("HH-mm-ss");
    //计算绘制区域大小,先将图像缩放到合适大小并保存在本地
    for (int i = 0; i < count; ++i) {
        QImage image(listImage.at(i));
        if (image.width() > imageWidth) {
            QString name = path + QString("%1-%2.jpg").arg(time).arg(i);
            image = image.scaled(imageWidth, image.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            image.save(name, "jpg");
            listImage[i] = name;
        }
    }

    for (int i = 0; i < count; i = i + 2) {
        list << "<tr style='vertical-align:middle;'>";
        list << QString("<td align='center'>%1</td>").arg(getImageCode(listImage.at(i)));
        //需要过滤判断下,很可能是奇数会超过下一个
        if (i < count - 1) {
            list << QString("<td align='center'>%1</td>").arg(getImageCode(listImage.at(i + 1)));
        }
        list << "</tr>";
    }

    //表格结束
    list << "</table>";
}

void DataReport::creatPersonReportHead(QStringList &list, const PersonReportData &reportData)
{
    //表格开始
    list << "<table border='0.0' cellspacing='0' cellpadding='6' style='font-size:15px;'>";

    //标题
    list << "<tr>";
    list << QString("<td width='100%' align='center' style='font-size:25px;font-weight:bold;' colspan='%1'>%2</td>").arg(2).arg(reportData.title);
    list << "</tr>";

    //换行
    list << "<br>";

    //报表日期
    list << "<tr style='font-size:18px;'>";
    //可以自行修改左对齐右对齐,不要的内容为空就行
    list << QString("<td align='left'>%1</td>").arg(QString("报表日期: %1").arg(reportData.time));
    list << QString("<td align='right'>%1</td>").arg("制表部门: 人事部");
    list << "</tr>";

    //表格结束
    list << "</table>";
}

void DataReport::creatPersonReportBody(QStringList &list, const PersonReportData &reportData, const QStringList &columnValue, int brCount)
{
    //如果字段数量不够或者值数量不一致则不用处理
    QStringList columnName = reportData.columnName;
    int count = columnName.count() - 1;
    if (count < 5 || count > columnValue.count()) {
        return;
    }

    //表格开始
    list << "<table border='0.1' cellspacing='0' cellpadding='5' style='font-size:16px;'>";

    //5行3列
    list << "<tr style='vertical-align:middle;'>";
    list << QString("<td width='%1' align='right'>%2</td>").arg(reportData.columnWidth1).arg(columnName.at(0));
    list << QString("<td width='%1' align='left'>%2</td>").arg(reportData.columnWidth2).arg(columnValue.at(0));

    //末尾数据是图片路径,将图片放到临时目录并缩放
    QString image = getImageCode(columnValue.last());

    //头像占5行
    list << QString("<td width='%1' align='center' rowspan='5'>%2</td>").arg(reportData.columnWidth3).arg(image);
    list << "</tr>";

    //头像左侧其他行
    for (int i = 1; i <= 4; ++i) {
        list << "<tr>";
        list << QString("<td align='right'>%1</td>").arg(columnName.at(i));
        list << QString("<td align='left'>%1</td>").arg(columnValue.at(i));
        list << "</tr>";
    }

    //没有头像的行,第二行占2列
    for (int i = 5; i < count; ++i) {
        list << "<tr>";
        list << QString("<td align='right'>%1</td>").arg(columnName.at(i));
        list << QString("<td align='left' colspan='2'>%1</td>").arg(columnValue.at(i));
        list << "</tr>";
    }

    //表格结束
    list << "</table>";

    //换行填充空行构成一页
    for (int i = 0; i < brCount; ++i) {
        list << "<br>";
    }
}
