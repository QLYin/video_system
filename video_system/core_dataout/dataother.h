#ifndef DATAOTHER_H
#define DATAOTHER_H

#include "datahead.h"

class DataOther
{
public:
    //分页导出到pdf带排版方向控制
    static void savePdf(const QString &fileName, const QString &content = QString());

    //导出图片到pdf scale: 0=等比例缩放 1=拉伸填充 2=不处理
    static void toPdf(const QPixmap &pixmap, const QString &fileName, int scale = 0);
    //控件截图到pdf
    static void widgetToPdf(QWidget *widget, const QString &fileName, int scale = 0);
    //图片转pdf文件 没有传入导出文件名称则取同名文件
    static QString imageToPdf(const QString &imageFile, const QString &pdfFile = QString(), int scale = 0);

    //打印图片(如果文件名不为空则打印到pdf)
    static void toPrint(const QPixmap &pixmap, const QString &fileName = QString());
    //打印控件
    static void widgetToPrint(QWidget *widget);
    //打印图片文件
    static void imageToPrint(const QString &imageFile);
    //打印图表控件
    static void chartToPrint(int width = 400, int height = 300);

    //csv转xls 没有传入导出文件名称则取同名文件
    static QString csvToXls(DataContent &dataContent, const QString &csvFile, const QString &xlsFile = QString(), bool quatation = true, const QString &spliter = ",");
    //校验引号中的内容
    static bool checkQuatation(QStringList &content, const QString &line, const QString &spliter, const QString &separator);
};

#endif // DATAOTHER_H
