#ifndef FRMFILEUPLOAD_H
#define FRMFILEUPLOAD_H

#include <QWidget>
class QCheckBox;
class QTableWidget;

namespace Ui {
class frmFileUpload;
}

class frmFileUpload : public QWidget
{
    Q_OBJECT

public:
    explicit frmFileUpload(QWidget *parent = 0);
    ~frmFileUpload();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmFileUpload *ui;

    //全选复选框
    QCheckBox *ckAll;
    //添加进来的文件名称集合
    QStringList fileNames;
    //等待上传的文件名称集合
    QStringList files;
    //统计已经发送完毕的数量
    int sendFinshCount;

public:
    //获取文件名称集合
    QStringList getFileNames();
    //获取表格控件
    QTableWidget *getTable();

private slots:
    //初始化窗体数据
    void initForm();
    //复选框状态发生变化
    void stateChanged(int arg1);

    //发送文件完成
    void sendFinsh(const QString &ip, int port, const QString &error);
    //表格单元格按下自动切换选中不选中
    void on_tableWidget_cellPressed(int row, int column);

public:
    //设置样式
    void initStyle(bool progressBarStyle = true);

    //清除数据
    void clear();
    //添加文件
    void addFile(const QString &type, const QString &name, const QString &path);
    //上传文件
    void upload(const QString &receiveHost, int receivePort, int maxPackageSize = 0xFFFF);

signals:
    //所有发送完成
    void finshed();
    //发送出错
    void error(const QString &text);
};

#endif // FRMFILEUPLOAD_H
