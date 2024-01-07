#ifndef FRMCONFIGPLUS_H
#define FRMCONFIGPLUS_H

#include <QWidget>
#include "singleton.h"

namespace Ui {
class frmConfigPlus;
}

class frmConfigPlus : public QWidget
{
    Q_OBJECT SINGLETON_DECL(frmConfigPlus)

public:
    explicit frmConfigPlus(QWidget *parent = 0);
    ~frmConfigPlus();

private:
    Ui::frmConfigPlus *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //加载配置文件
    void initConfig();
    //保存配置文件
    void saveConfig();

private slots:
    void on_btnAddPlus_clicked();
    void on_cboxPlusType_currentIndexChanged(int index);

signals:
    void addPlus(const QStringList &rtspMains, const QStringList &rtspSubs);
};

#endif // FRMCONFIGPLUS_H
