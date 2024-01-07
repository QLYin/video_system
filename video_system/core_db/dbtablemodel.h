#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

#include "dbhead.h"

//表格模型设置列居中和右对齐+鼠标整行换色+指定列替换字符
class DbTableModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit DbTableModel(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    QVariant data(const QModelIndex &index, int role) const;

private:
    //所有居中
    bool allCenter;
    //居中对齐列
    QList<int> alignCenterColumn;
    //右对齐列
    QList<int> alignRightColumn;

public:
    //设置需要悬停整行变色的表格
    void setTableView(QTableView *tableView);
    //设置所有列居中
    void setAllCenter(bool allCenter);
    //设置居中对齐列索引集合
    void setAlignCenterColumn(const QList<int> &alignCenterColumn);
    //设置右对齐列索引集合
    void setAlignRightColumn(const QList<int> &alignRightColumn);
};

#endif // DBTABLEMODEL_H
