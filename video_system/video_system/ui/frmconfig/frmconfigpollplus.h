#ifndef FRMCONFIGPOLLPLUS_H
#define FRMCONFIGPOLLPLUS_H

#include <QWidget>

class QSqlTableModel;
class QListWidgetItem;

namespace Ui {
class frmConfigPollPlus;
}

class frmConfigPollPlus : public QWidget
{
    Q_OBJECT

public:
    explicit frmConfigPollPlus(QWidget *parent = 0);
    ~frmConfigPollPlus();

protected:
    void showEvent(QShowEvent *);

private:
    Ui::frmConfigPollPlus *ui;
    QSqlTableModel *model;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化数据
    void initData();

    //加载动作菜单
    void initAction();
    //执行动作菜单
    void doAction();

    //加载轮询分组
    void initPollGroup();
    //保存轮询分组
    void savePollGroup();
    //校验轮询分组
    bool checkPollRow(int row);
    //校验轮询分组
    bool checkPollGroup(const QString &pollGroup);

private slots:
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_btnUpdate_clicked();
    void on_btnClear_clicked();
    void on_btnAddSelect_clicked();
    void on_btnAddPlus_clicked();
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

signals:
    //轮询分组变化信号
    void pollGroupChanged();
    //批量添加地址信号
    void addPlus(const QStringList &rtspMains, const QStringList &rtspSubs);
};

#endif // FRMCONFIGPOLLPLUS_H
