#include "dbpagemodel.h"

DbPageModel::DbPageModel(QObject *parent) : QSqlQueryModel(parent)
{
    allCenter = false;
    alignCenterColumn.clear();
    alignRightColumn.clear();
}

bool DbPageModel::eventFilter(QObject *watched, QEvent *event)
{
    //悬停整行换颜色
    QTableView *table = (QTableView *)watched->parent();
    if (event->type() == QEvent::HoverMove) {
        QMouseEvent *mouse = (QMouseEvent *)event;
        table->setCurrentIndex(table->indexAt(mouse->pos()));
    }

    return QObject::eventFilter(watched, event);
}

QVariant DbPageModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);

    //超过100万的数值会被科学计数显示需要这里转成字符串显示
    if (role == Qt::DisplayRole) {
        int result = value.toInt();
        if (result >= 1000000) {
            value = QString::number(result);
        }
    }

    if (allCenter) {
        if (role == Qt::TextAlignmentRole) {
            value = Qt::AlignCenter;
        }
    } else {
        //逐个从列索引中查找是否当前列在其中
        int column = index.column();
        bool existCenter = alignCenterColumn.contains(column);
        bool existRight = alignRightColumn.contains(column);

        if (role == Qt::TextAlignmentRole) {
            if (existCenter) {
                value = Qt::AlignCenter;
            }
            if (existRight) {
                value = (QVariant)(Qt::AlignVCenter | Qt::AlignRight);
            }
        }
    }

#if 0
    //实现鼠标经过整行换色,如果设置了hoverRow才需要处理
    //设置悬停背景颜色 setProperty("hoverBgColor", "#FF0000");
    //设置悬停文本颜色 setProperty("hoverTextColor", "#FFFFFF");
    //每次鼠标移动的时候设置当前行 setProperty("hoverRow", 0);
    if (property("hoverRow").isValid()) {
        int row = property("hoverRow").toInt();
        if (row == index.row()) {
            if (role == Qt::BackgroundRole) {
                value = QColor(property("hoverBgColor").toString());
            } else if (role == Qt::ForegroundRole) {
                value = QColor(property("hoverTextColor").toString());
            }
        }
    }
#endif

    //实现隐藏部分显示,指定列替换字符,比如电话号码中间圆点显示
    //设置需要隐藏处理的列 setProperty("hideColumn", 0);
    //设置替换后的字符 setProperty("hideLetter", "*");
    //设置需要替换的开始位置 setProperty("hideStart", 4);
    //设置需要替换的结束位置 setProperty("hideEnd", 8);
    if (property("hideColumn").isValid()) {
        int column = property("hideColumn").toInt();
        if (column == index.column()) {
            if (role == Qt::DisplayRole) {
                QString letter = property("hideLetter").toString();
                int start = property("hideStart").toInt();
                int end = property("hideEnd").toInt();

                //过滤为空的时候
                letter = (letter.isEmpty() ? "*" : letter);
                end = (end == 0 ? 1000 : end);

                QStringList list;
                QString str = value.toString();
                for (int i = 0; i < str.length(); ++i) {
                    if (i >= start && i <= end) {
                        list << letter;
                    } else {
                        list << str.at(i);
                    }
                }

                value = list.join("");
            }
        }
    }

    return value;
}

void DbPageModel::setTableView(QTableView *tableView)
{
    //切记这里是 viewport 去识别不然坐标不准
    tableView->viewport()->installEventFilter(this);
}

void DbPageModel::setAllCenter(bool allCenter)
{
    this->allCenter = allCenter;
}

void DbPageModel::setAlignCenterColumn(const QList<int> &alignCenterColumn)
{
    this->alignCenterColumn = alignCenterColumn;
}

void DbPageModel::setAlignRightColumn(const QList<int> &alignRightColumn)
{
    this->alignRightColumn = alignRightColumn;
}
