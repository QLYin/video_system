#ifndef TCPFILEHELPER_H
#define TCPFILEHELPER_H

#include "tcpfilehead.h"

class TcpFileHelper
{
public:
    //将字节转为 KB MB GB 单位
    static QString getSize(quint64 size);
    //获取正确的路径
    static QString getPath(const QString &path);

    //初始化进度条
    static void initProgressBar(QProgressBar *progressBar, quint64 fileSize);
    //更新发送进度
    static void updateProgressBar(QProgressBar *progressBar, quint64 fileSize, quint64 size);

    //表格最大行
    static int maxRow;
    //初始化表格控件
    static void initTable(QTableWidget *tableWidget, int rowHeight);
    //初始化表格
    static void initTableView(QTableView *tableView, int rowHeight = 25,
                              bool headVisible = false, bool edit = false,
                              bool stretchLast = true);
    //生成表格行
    static void addRow(QTableWidget *tableWidget, QProgressBar *progressBar,
                       int &row, const QString &ip, int port,
                       const QString &fileName, quint64 fileSize);

    //异或运算
    static void xorData(QByteArray &data, quint8 key);
    //数据加密解密 默认采用异或运算可以自行更改成其他
    static void encryData(QByteArray &data, const QString &key);
    static void decryData(QByteArray &data, const QString &key);

    //嵌入式linux上解压文件+权限赋值
    static void tarAndChmod(const QString &fileName, const QString &savePath);
    //嵌入式linux重启系统
    static void reboot();
};

#endif // TCPFILEHELPER_H
