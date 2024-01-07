#pragma execution_character_set("utf-8")

#include "tasktableview.h"
#include "qpainter.h"
#include "qevent.h"
#include "qmenu.h"
#include "qheaderview.h"
#include "qframe.h"
#include "qdebug.h"

TaskModel::TaskModel(const QStringList &horizontalLabel, const QStringList &verticalLabel, QObject *parent) : QAbstractTableModel(parent)
{
    this->horizontalLabel = horizontalLabel;
    this->verticalLabel = verticalLabel;
}

int TaskModel::rowCount(const QModelIndex &) const
{
    return verticalLabel.count() + 1;
}

int TaskModel::columnCount(const QModelIndex &) const
{
    return horizontalLabel.count() + 1;
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    QVariant data;
    if (!index.isValid()) {
        return data;
    }

    int row = index.row();
    int column = index.column();
    if (role == Qt::ToolTipRole) {
        if (row > 0 && column > 0) {
            //data = verticalLabel.at(row - 1) + " " + horizontalLabel.at(column - 1);
        }
    } else if (role == Qt::TextAlignmentRole) {
        data = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::UserRole) {
        if (column == 0 && row > 0 && row <= verticalLabel.count()) {
            data = verticalLabel.at(row - 1);
        } else if (row == 0 && column > 0 && column <= horizontalLabel.count()) {
            data = horizontalLabel.at(column - 1);
        }
    }

    return data;
}

Qt::ItemFlags TaskModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

TaskDelegate::TaskDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    widget = parent;

    bgColor = QColor(255, 255, 255);
    gridColor = QColor(150, 150, 150);

    headerBgColor = QColor(220, 220, 220);
    headerTextColor = QColor(60, 60, 60);

    normalColor = QColor(140, 143, 145);
    selectColor = QColor(84, 111, 198);

    shadowWidth = 2;
    shadowColor = QColor(50, 50, 50, 100);
}

void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
    initStyleOption(&viewOption, index);
    if (option.state.testFlag(QStyle::State_HasFocus)) {
        viewOption.state = viewOption.state ^ QStyle::State_HasFocus;
    }

    //QStyledItemDelegate::paint(painter, viewOption, index);

    int row = index.row();
    int column = index.column();
    QRect rect = viewOption.rect;

    //第一行第一列作为标题特殊处理
    if (row == 0 || column == 0) {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(headerBgColor);
        painter->drawRect(rect);
        if (row == 0 && column == 0) {
            painter->setPen(gridColor);
            painter->setBrush(Qt::NoBrush);
            painter->drawLine(rect.topLeft(), rect.bottomRight());
        }
        painter->restore();
    } else {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(bgColor);
        painter->drawRect(rect);
        painter->restore();
    }

    //居中绘制行标题列标题
    if ((column == 0 && row > 0) || (row == 0 && column > 0)) {
        painter->save();
        painter->setPen(headerTextColor);
        painter->setBrush(Qt::NoBrush);
        QTextOption option(Qt::AlignHCenter | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAnywhere);
        QString text = index.data(Qt::UserRole).toString();
        QRect textrect = rect.adjusted(0, 0, 0, 0);
        painter->drawText(textrect, text, option);
        painter->restore();
    }

    //绘制表格线条
    if (row > 0) {
        painter->save();
        painter->setPen(gridColor);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(rect.x(), rect.y(), rect.x() + rect.width(), rect.y());
        painter->restore();
    }
    if (column > 0 && column % 2 == 1) {
        painter->save();
        painter->setPen(gridColor);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(rect.x(), rect.y(), rect.x(), rect.y() + rect.height());
        painter->restore();
    }

    //绘制选中单元格方块
    if (column > 0 && row > 0) {
        QRect drawrect;
        if (column % 2 == 1) {
            drawrect = QRect(rect.x() + 1, rect.y() + 1, rect.width() - 2, rect.height() - 2);
        } else {
            drawrect = QRect(rect.x(), rect.y() + 1, rect.width() - 1, rect.height() - 2);
        }

        //通过设置不同的透明度呈现立体感
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        if (option.state.testFlag(QStyle::State_Selected)) {
            QColor color = selectColor;
            color.setAlpha(100);
            painter->setBrush(color);
            painter->drawRect(drawrect.x(), drawrect.y() + drawrect.height() - shadowWidth, drawrect.width() - shadowWidth, shadowWidth);
            painter->drawRect(drawrect.x() + drawrect.width() - shadowWidth, drawrect.y(), shadowWidth, drawrect.height());
            painter->setBrush(shadowColor);
            painter->drawRect(drawrect.x(), drawrect.y(), drawrect.width(), shadowWidth);
            painter->drawRect(drawrect.x(), drawrect.y(), shadowWidth, drawrect.height());
            color.setAlpha(255);
            painter->setBrush(color);
            painter->drawRect(drawrect.x() + shadowWidth, drawrect.y() + shadowWidth, drawrect.width() - shadowWidth * 2, drawrect.height() - shadowWidth * 2);
        } else {
            QColor color = normalColor;
            color.setAlpha(100);
            painter->setBrush(color);
            painter->drawRect(drawrect.x(), drawrect.y(), drawrect.width(), shadowWidth);
            painter->drawRect(drawrect.x(), drawrect.y() + shadowWidth, shadowWidth, drawrect.height() - shadowWidth);
            painter->setBrush(shadowColor);
            painter->drawRect(drawrect.x(), drawrect.y() + drawrect.height() - shadowWidth, drawrect.width(), shadowWidth);
            painter->drawRect(drawrect.x() + drawrect.width() - shadowWidth, drawrect.y(), shadowWidth, drawrect.height());
            color.setAlpha(255);
            painter->setBrush(color);
            painter->drawRect(drawrect.x() + shadowWidth, drawrect.y() + shadowWidth, drawrect.width() - shadowWidth * 2, drawrect.height() - shadowWidth * 2);
        }
        painter->restore();
    }
}

bool TaskDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //过滤当做行列的单元格不响应事件
    int type = event->type();
    if (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonRelease || type == QEvent::MouseMove || type == QEvent::MouseButtonDblClick) {
        if (index.row() == 0 || index.column() == 0) {
            return true;
        } else {
            //测试发现鼠标滑动选择后不会触发单击事件需要这里主动发一个
            if (event->type() == QEvent::MouseButtonRelease) {
                QMetaObject::invokeMethod(widget, "selectChanged");
            }
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QColor TaskDelegate::getBgColor() const
{
    return this->bgColor;
}

void TaskDelegate::setBgColor(const QColor &color)
{
    this->bgColor = color;
    widget->update();
}

QColor TaskDelegate::getGridColor() const
{
    return this->gridColor;
}

void TaskDelegate::setGridColor(const QColor &color)
{
    this->gridColor = color;
    widget->update();
}

QColor TaskDelegate::getHeaderTextColor() const
{
    return this->headerTextColor;
}

void TaskDelegate::setHeaderTextColor(const QColor &color)
{
    this->headerTextColor = color;
    widget->update();
}

QColor TaskDelegate::getHeaderBgColor() const
{
    return this->headerBgColor;
}

void TaskDelegate::setHeaderBgColor(const QColor &color)
{
    this->headerBgColor = color;
    widget->update();
}

QColor TaskDelegate::getNormalColor() const
{
    return this->normalColor;
}

void TaskDelegate::setNormalColor(const QColor &color)
{
    this->normalColor = color;
    widget->update();
}

QColor TaskDelegate::getSelectColor() const
{
    return this->selectColor;
}

void TaskDelegate::setSelectColor(const QColor &color)
{
    this->selectColor = color;
    widget->update();
}

int TaskDelegate::getShadowWidth() const
{
    return this->shadowWidth;
}

void TaskDelegate::setShadowWidth(int width)
{
    this->shadowWidth = width;
    widget->update();
}

QColor TaskDelegate::getShadowColor() const
{
    return this->shadowColor;
}

void TaskDelegate::setShadowColor(const QColor &color)
{
    this->shadowColor = color;
    widget->update();
}

TaskTableView::TaskTableView(QWidget *parent) : QTableView(parent)
{
    this->initForm();
    this->initMenu();
}

void TaskTableView::resizeEvent(QResizeEvent *)
{
    //平均设置行高
    int toalHeight = this->height() - this->rowHeight(0) - 4;
    double offsetHeight = toalHeight * 1.0 / rowCount;
    double spaceHeight = (offsetHeight - (int)offsetHeight) * rowCount;
    for (int row = 1; row <= rowCount; ++row) {
        int tmpheight = 0;
        if (spaceHeight >= 0) {
            tmpheight = 1;
            spaceHeight -= 1;
        }
        this->setRowHeight(row, offsetHeight + tmpheight);
    }

    //平均设置列宽
    int totalWidth = this->width() - this->columnWidth(0) - 3;
    double offsetWidth = totalWidth * 1.0 / columnCount;
    double spaceWidth = (offsetWidth - (int)offsetWidth) * columnCount;
    for (int column = 1; column <= columnCount; ++column) {
        int tmpWidth = 0;
        if (spaceWidth >= 0) {
            tmpWidth = 1;
            spaceWidth -= 1;
        }
        this->setColumnWidth(column, offsetWidth + tmpWidth);
    }
}

void TaskTableView::contextMenuEvent(QContextMenuEvent *)
{
    menu->exec(QCursor::pos());
}

void TaskTableView::initForm()
{
    //横向表头标题
    for (int i = 0; i < 24; ++i) {
        horizontalLabel << QString::number(i);
        horizontalLabel << QString::number((float)i + 0.5);
    }

    //纵向表头标题
    verticalLabel << "星期一" << "星期二" << "星期三" << "星期四" << "星期五" << "星期六" << "星期日";

    //行数列数
    rowCount = verticalLabel.count();
    columnCount = horizontalLabel.count();

    //设置数据模型
    TaskModel *taskModel = new TaskModel(horizontalLabel, verticalLabel, this);
    this->setModel(taskModel);

    //设置自定义委托控制样式
    taskDelegate = new TaskDelegate(this);
    this->setItemDelegate(taskDelegate);

    //设置表格的一些属性
    this->setShowGrid(false);
    this->setWordWrap(true);
    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setVisible(false);
    this->verticalHeader()->setStretchLastSection(true);
    this->horizontalHeader()->setStretchLastSection(true);
    this->setFrameShape(QFrame::NoFrame);
    this->setSelectionMode(QAbstractItemView::MultiSelection);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
    this->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    this->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

    //设置默认尺寸
    this->horizontalHeader()->setDefaultSectionSize(12);
    this->horizontalHeader()->setMinimumSectionSize(12);
    this->setRowHeight(0, 25);
    this->setColumnWidth(0, 60);

    //合并第一行单元格当做标题
    for (int column = 1; column < columnCount; column += 2) {
        this->setSpan(0, column, 1, 2);
    }
}

void TaskTableView::initMenu()
{
    menu = new QMenu(this);
    QAction *actionAll = menu->addAction("全部选中");
    QAction *actionInvert = menu->addAction("反向选中");
    QAction *actionClear = menu->addAction("清空选中");
    connect(actionAll, SIGNAL(triggered(bool)), this, SLOT(doAction()));
    connect(actionInvert, SIGNAL(triggered(bool)), this, SLOT(doAction()));
    connect(actionClear, SIGNAL(triggered(bool)), this, SLOT(doAction()));
}

void TaskTableView::doAction()
{
    //整个范围
    QItemSelection selectionAll;
    QModelIndex tl = model()->index(1, 1);
    QModelIndex br = model()->index(rowCount, columnCount);
    selectionAll << QItemSelectionRange(tl, br);

    QAction *action = (QAction *)sender();
    QString text = action->text();
    if (text == "全部选中") {
        this->selectionModel()->select(selectionAll, QItemSelectionModel::Select);
    } else if (text == "反向选中") {
        //先对所有的选中然后对之前选中的取消选中就是反向选中效果
        QItemSelection selcetion = selectionModel()->selection();
        this->selectionModel()->select(selectionAll, QItemSelectionModel::Select);
        this->selectionModel()->select(selcetion, QItemSelectionModel::Deselect);
    } else if (text == "清空选中") {
        this->selectionModel()->clear();
    }

    Q_EMIT selectChanged();
}

QSize TaskTableView::sizeHint() const
{
    return QSize(800, 300);
}

QSize TaskTableView::minimumSizeHint() const
{
    return QSize(100, 60);
}

QString TaskTableView::getSelectRow(int row) const
{
    QStringList list;
    QItemSelectionModel *selections = this->selectionModel();
    QModelIndexList indexs = selections->selectedIndexes();
    for (int column = 1; column <= columnCount; ++column) {
        QModelIndex index = model()->index(row, column);
        list << (indexs.contains(index) ? "1" : "0");
    }

    return list.join(",");
}

void TaskTableView::setSelectRow(int row, const QString &flag)
{
    //先记住之前选中的(不包括新设置的行)
    QItemSelection selection;
    QItemSelectionModel *selections = this->selectionModel();
    QModelIndexList indexs = selections->selectedIndexes();
    foreach (QModelIndex index, indexs) {
        if (index.row() != row) {
            selection << QItemSelectionRange(index, index);
        }
    }

    //每个单元格存入选中范围
    QStringList list = flag.split(",");
    int count = list.count();
    for (int i = 0; i < count; ++i) {
        if (list.at(i) == "1") {
            QModelIndex index = model()->index(row, i + 1);
            selection << QItemSelectionRange(index, index);
        }
    }

    //最后将整个选中范围设置(先清空之前的再设置)
    this->selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
}

QColor TaskTableView::getBgColor() const
{
    return taskDelegate->getBgColor();
}

void TaskTableView::setBgColor(const QColor &color)
{
    taskDelegate->setBgColor(color);
}

QColor TaskTableView::getGridColor() const
{
    return taskDelegate->getGridColor();
}

void TaskTableView::setGridColor(const QColor &color)
{
    taskDelegate->setGridColor(color);
}

QColor TaskTableView::getHeaderBgColor() const
{
    return taskDelegate->getHeaderBgColor();
}

void TaskTableView::setHeaderBgColor(const QColor &color)
{
    taskDelegate->setHeaderBgColor(color);
}

QColor TaskTableView::getHeaderTextColor() const
{
    return taskDelegate->getHeaderTextColor();
}

void TaskTableView::setHeaderTextColor(const QColor &color)
{
    taskDelegate->setHeaderTextColor(color);
}

QColor TaskTableView::getNormalColor() const
{
    return taskDelegate->getNormalColor();
}

void TaskTableView::setNormalColor(const QColor &color)
{
    taskDelegate->setNormalColor(color);
}

QColor TaskTableView::getSelectColor() const
{
    return taskDelegate->getSelectColor();
}

void TaskTableView::setSelectColor(const QColor &color)
{
    taskDelegate->setSelectColor(color);
}

int TaskTableView::getShadowWidth() const
{
    return taskDelegate->getShadowWidth();
}

void TaskTableView::setShadowWidth(int width)
{
    taskDelegate->setShadowWidth(width);
}

QColor TaskTableView::getShadowColor() const
{
    return taskDelegate->getShadowColor();
}

void TaskTableView::setShadowColor(const QColor &color)
{
    taskDelegate->setShadowColor(color);
}
