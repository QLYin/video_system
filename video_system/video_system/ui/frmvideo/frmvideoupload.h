#ifndef FRMVIDEOUPLOAD_H
#define FRMVIDEOUPLOAD_H

#include <QWidget>

namespace Ui {
class frmVideoUpload;
}

class frmVideoUpload : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoUpload(QWidget *parent = 0);
    ~frmVideoUpload();

private:
    Ui::frmVideoUpload *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();

    //加载配置文件
    void initConfig();
    //保存配置文件
    void saveConfig();

    //所有发送完成
    void finshed();
    //发送出错
    void error(const QString &text);

private slots:
    void on_btnSelect_clicked();
    void on_btnUpload_clicked();
};

#endif // FRMVIDEOUPLOAD_H
