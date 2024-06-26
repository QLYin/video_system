﻿#include "frmscreen.h"
#include <QPainter>
#include <QMenu>
#include <QMimeData>
#include <QDebug>

#include "frmcell.h"
#include "ui/frmbase/Indicator.h"
#include "class/devicemanager/tvwallmanager.h"
#include "class/appmisc/appmisc.h"

namespace {
	QVector<int> kSplitNum = {2, 4, 6, 9, 16, 25, 36};
};

frmScreen::frmScreen(QWidget* parent)
{
	setAcceptDrops(true);
	setAlignment(Qt::AlignCenter);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &frmScreen::customContextMenuRequested,
		this, &frmScreen::showContextMenu);
	setStyleSheet("background-color : rgb(14, 26, 50);");
	m_topLabel = new QLabel(this);
	m_topLabel->setAlignment(Qt::AlignLeft);
	m_topLabel->setGeometry(1, 1, width(), 20);
	m_cutRow = 1;
	m_cutCol = 1;
}

frmScreen::~frmScreen()
{

}

void frmScreen::showContextMenu(const QPoint& pos)
{
	if (!m_enableDrop) return;
	QMenu contextMenu(this);
	int mamSpiltNum = AppMisc::Instance()->get("max_split_num").toInt();
	// 多画面
	QAction* splitAction = contextMenu.addAction(QString::fromLocal8Bit("多画面显示"));
	QMenu splitMenu(this);
	auto action = splitMenu.addAction("1分屏", [this]()
	{
		cutScreen(1, 1);
	});
	action->setEnabled(!(m_cutRow == 1 && m_cutCol == 1));

	action = splitMenu.addAction("2分屏", [this]()
	{
		cutScreen(1, 2);
	});
	action->setEnabled(!(m_cutRow == 1 && m_cutCol == 2) && mamSpiltNum >= 2);

	action = splitMenu.addAction("4分屏", [this]()
	{
		cutScreen(2, 2);
	});
	action->setEnabled(!(m_cutRow == 2 && m_cutCol == 2) && mamSpiltNum >= 4);

	action = splitMenu.addAction("9分屏", [this]()
	{
		cutScreen(3, 3);
	});
	action->setEnabled(!(m_cutRow == 3 && m_cutCol == 3) && mamSpiltNum >= 9);

	action = splitMenu.addAction("16分屏", [this]()
	{
		cutScreen(4, 4);
	});
	action->setEnabled(!(m_cutRow == 4 && m_cutCol == 4) && mamSpiltNum >= 16);

	action = splitMenu.addAction("25分屏", [this]()
		{
			cutScreen(5, 5);
		});
	action->setEnabled(!(m_cutRow == 5 && m_cutCol == 5) && mamSpiltNum >= 25);

	action = splitMenu.addAction("36分屏", [this]()
		{
			cutScreen(6, 6);
		});
	action->setEnabled(!(m_cutRow == 6 && m_cutCol == 6) && mamSpiltNum >= 36);

	splitAction->setMenu(&splitMenu);

	// 退出拼接
	if (m_ChildScreens.size() > 1) {
		contextMenu.addSeparator();
		QAction* restoreAction = contextMenu.addAction(QString::fromLocal8Bit("退出拼接"));
		connect(restoreAction, &QAction::triggered, [this]()
			{
				emit screenMergeRestore(this);
			});
	}

	// 关闭视频 + 关闭声音
	QAction* closeVideoAction = contextMenu.addAction(QString::fromLocal8Bit("关闭视频"));
	//int chnIndex = findChnIndexByPos(pos);
	//qDebug() << " pos: " << pos << ", chn index: " << chnIndex;
	connect(closeVideoAction, &QAction::triggered, [this,pos]()
		{
			int chnIndex = findChnIndexByPos(pos);
			qDebug() << " pos: " << pos << ", chn index: " << chnIndex;
			emit closeVideo(chnIndex);
		});


	contextMenu.exec(mapToGlobal(pos));
}

void frmScreen::cutScreen(int row, int col, bool needUpdate, bool notify)
{

	if (TVWallManager::Instance()->hasMergeScreen())
	{
		Indicator::showTopTip(QString::fromLocal8Bit("当前存在合并视频, 不能分屏"), nullptr);
		return;
	}

	if (row <= 0 && col <= 0) 
	{
		return;
	}

	if (m_cutRow == row && m_cutCol == col)
	{
		return;
	}

	setText("");
	m_cutRow = row;
	m_cutCol = col;
	
	if (!m_gridLayout)
	{
		m_gridLayout = new QGridLayout;
		setLayout(m_gridLayout);
		m_gridLayout->setSpacing(1);
		m_gridLayout->setMargin(0);
	}
	else
	{
		QLayoutItem* child = nullptr;
		while (child = m_gridLayout->takeAt(0))
		{
			if (child->widget())
			{
				child->widget()->setParent(nullptr);
			}
			m_gridLayout->removeWidget(child->widget());
			delete child;
		}
		//m_gridLayout->deleteLater();
		//m_gridLayout = nullptr;
	}

	if (m_cutRow > 1 || m_cutCol > 1)
	{
		for (int i = 0; i < m_cutRow; ++i)
		{
			for (int j = 0; j < m_cutCol; ++j)
			{
				frmCell* cell = new frmCell(this);
				connect(cell, &frmCell::dropInfo, this, [=](int id, QString text)
					{
						emit dropInfo(m_index + i * m_cutCol + j, id, text);
					});
				//cell->setIndex(m_index + i * row + col);
				m_gridLayout->addWidget(cell, i, j);
			}
		}

	}

	if (needUpdate)
	{
		emit indexUpdate(screenInfo().x, screenInfo().y, m_index);
	}

	if (notify)
	{
		emit screenCut(m_cutRow * m_cutCol);
	}

	emit screenTextUpdate();
}

void frmScreen::cutScreen(int cnt, bool needUpdateIndex, bool notify)
{
	if (!kSplitNum.contains(cnt))
	{
		return;
	}

	if (cnt == 1)
	{
		cutScreen(1, 1, true, false);
	}
	else if (cnt == 2)
	{
		cutScreen(1, 2, true, false);
	}
	else if (cnt == 4)
	{
		cutScreen(2, 2, true, false);
	}
	else if (cnt == 9) 
	{
		cutScreen(3, 3, true, false);
	}
	else if (cnt == 16)
	{
		cutScreen(4, 4, true, false);
	}
	else if (cnt == 25)
	{
		cutScreen(5, 5, true, false);
	}
	else if (cnt == 36)
	{
		cutScreen(6, 6, true, false);
	}
}

void frmScreen::revert(bool needUpdate)
{
	m_cutRow = -1;
	m_cutCol = -1;

	QLayoutItem* child = nullptr;
	while (child = m_gridLayout->takeAt(0))
	{
		if (child->widget())
		{
			child->widget()->setParent(nullptr);
		}
		m_gridLayout->removeWidget(child->widget());
		delete child;
	}
	//m_gridLayout->deleteLater();
	//m_gridLayout = nullptr;

	if (needUpdate)
	{
		emit indexUpdate(screenInfo().x, screenInfo().y, m_index);
	}
}

void frmScreen::setIndex(int index)
{
	m_index = index;
	m_topLabel->setText(QString::number(index));
}

int frmScreen::index()
{
	return m_index;
}

bool frmScreen::hasCut()
{
	return m_cutRow > 1 || m_cutCol > 1;
}

int frmScreen::cutRow()
{
	return m_cutRow;
}

int frmScreen::cutCol()
{
	return m_cutCol;
}

void frmScreen::setCellText(int index, QString text)
{
	if (!hasCut())
	{
		return;
	}

	int r = index / m_cutCol;
	int c = index % m_cutCol;
	frmCell* cell = nullptr;
	auto item = m_gridLayout->itemAtPosition(r, c);
	if (item)
	{
		cell = qobject_cast<frmCell*>(item->widget());
		cell->setText(text);
	}
}

ScreenInfo frmScreen::screenInfo()
{
	auto screenInfo = m_ChildScreens.at(0);
	screenInfo.row = m_cutRow;
	screenInfo.col = m_cutCol;
	return screenInfo;
}

QVector<ScreenInfo> frmScreen::childScreenInfos()
{
	return m_ChildScreens;
}

void frmScreen::setChildScreenInfos(QVector<ScreenInfo> infos)
{
	m_ChildScreens = infos;
}

void frmScreen::appendScreenInfo(int x, int y, int row, int col)
{
	ScreenInfo info;
	info.x = x;
	info.y = y;
	info.row = row;
	info.col = col;
	m_ChildScreens.push_back(info);
}

void frmScreen::dragEnterEvent(QDragEnterEvent* event)
{
	if (!m_enableDrop) return;
	if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
	else if (event->mimeData()->hasFormat("text/uri-list")) {
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
	else {
		event->ignore();
	}
}

void frmScreen::dropEvent(QDropEvent* event)
{
	if (!m_enableDrop) return;
	QString text;
	if (event->mimeData()->hasUrls()) {
		text = event->mimeData()->urls().first().toLocalFile();
	}
	else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		QTreeWidget* treeWidget = (QTreeWidget*)event->source();
		if (treeWidget) {
			//过滤父节点(那个一般是NVR)
			QTreeWidgetItem* item = treeWidget->currentItem();
			if (item->parent()) {
				text = item->data(0, Qt::UserRole + 2).toString();
				int id = item->data(0, Qt::UserRole + 1).toInt();
				setText(text);
				QFont font("Arial", 8);
				setFont(font);
				emit dropInfo(m_index, id, text);
			}
		}
	}
}

void frmScreen::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		cutScreen(1, 1);
	}

	//QWidget::mouseDoubleClickEvent(event);
}

int frmScreen::updateIndex(int index)
{
	setIndex(index);
	if (!hasCut())
	{
		m_topLabel->setText(QString::number(index));
		index += 1;
	}
	else 
	{
		m_topLabel->setText("");
		for (int i = 0; i < m_cutRow; ++i)
		{
			for (int j = 0; j < m_cutCol; ++j)
			{
				QLayoutItem* child = m_gridLayout->itemAtPosition(i, j);
				if (child)
				{
					auto cell = qobject_cast<frmCell*>(child->widget());
					cell->setIndex(index);
					index += 1;
				}
			}
		}
	}
	return index;
}

void frmScreen::setEnableDrop(bool enable)
{
	m_enableDrop = enable;
}

void frmScreen::setSelected(bool selected)
{
	if (selected)
	{
		setStyleSheet("background-color: rgb(71, 140, 182);");
	}
	else
	{
		setStyleSheet("background-color : rgb(14, 26, 50);");
	}
}

int frmScreen::findChnIndexByPos(const QPoint& pos, bool clear)
{
	if (hasCut())
	{
		QPoint layoutPos = pos;
		int cellWidth = width() / m_cutCol;
		int cellHeight = height() / m_cutRow;
		int layoutX = layoutPos.x() / cellWidth;
		int layoutY = layoutPos.y() / cellHeight;
		QLayoutItem* item = m_gridLayout->itemAtPosition(layoutY, layoutX);
		if (item != nullptr) {
			auto cell = qobject_cast<frmCell*>(item->widget());
			if (clear)
			{
				cell->setText("");
			}
			return cell->index();
		}
	}

	if (clear)
	{
		setText("");
	}
	return index();
}