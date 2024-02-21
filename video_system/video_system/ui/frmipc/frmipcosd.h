#ifndef FRMIPCOSD_H
#define FRMIPCOSD_H

#include <QWidget>
#include "onvifstruct.h"
class QListWidgetItem;

namespace Ui {
class frmIpcOsd;
}

class frmIpcOsd : public QWidget
{
    Q_OBJECT

public:
    explicit frmIpcOsd(QWidget *parent = 0);
    ~frmIpcOsd();

private:
    Ui::frmIpcOsd *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //视频控件鼠标按下处坐标
    void receivePoint(int type, const QPoint &point);

private slots:
    OnvifOsdInfo getOSD();
    void on_btnGetOSDs_clicked();
    void on_btnCreateOSD_clicked();
    void on_btnSetOSD_clicked();
    void on_btnDeleteOSD_clicked();
    void on_listWidget_itemPressed(QListWidgetItem *item);
};

#endif // FRMIPCOSD_H
