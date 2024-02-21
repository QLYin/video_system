#ifndef FRMIPCPRESET_H
#define FRMIPCPRESET_H

#include <QWidget>
class QTableWidgetItem;

namespace Ui {
class frmIpcPreset;
}

class frmIpcPreset : public QWidget
{
    Q_OBJECT

public:
    explicit frmIpcPreset(QWidget *parent = 0);
    ~frmIpcPreset();

protected:
    void resizeEvent(QResizeEvent *);

private:
    Ui::frmIpcPreset *ui;

    //当前巡航预置位索引
    int presetIndex;
    //续航预置位行集合
    QList<int> presetRows;
    //巡航预置位标识集合
    QList<QString> presetTokens;
    //定时器执行巡航
    QTimer *timer;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化表格控件
    void initTable();
    //按钮单击事件
    void buttonClicked();

private slots:
    void on_btnGetPresets_clicked();
    void on_btnSetPresets_clicked();
    void on_btnGotoHomePosition_clicked();
    void on_btnSetHomePosition_clicked();

    //执行自动巡航
    void navigate();
    void on_btnStartNavigate_clicked();
    void on_btnStopNavigate_clicked();

    void on_tableWidget_itemPressed(QTableWidgetItem *item);
};

#endif // FRMIPCPRESET_H
