#include "frmscreen.h"
#include <QPainter>
#include <QMenu>
#include <QMimeData>
#include <QDebug>

#include "frmcell.h"

frmScreen::frmScreen(QWidget* parent)
{
	setAcceptDrops(true);
	setAlignment(Qt::AlignCenter);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &frmScreen::customContextMenuRequested,
		this, &frmScreen::showContextMenu);
	setStyleSheet("background-color: rgb(68, 73, 79);");
	m_topLabel = new QLabel(this);
	m_topLabel->setAlignment(Qt::AlignLeft);
	m_topLabel->setGeometry(0, 0, width(), 20);
}

frmScreen::~frmScreen()
{

}

void frmScreen::showContextMenu(const QPoint& pos)
{
	QMenu contextMenu(this);
	QAction* splitAction = contextMenu.addAction(QString::fromLocal8Bit("切割画面"));
	QAction* undoAction = contextMenu.addAction(QString::fromLocal8Bit("撤销切割"));
	undoAction->setEnabled(m_cutRow > 1 || m_cutCol > 1);
	if (m_ChildScreens.size() > 1) {
		contextMenu.addSeparator();
		QAction* restoreAction = contextMenu.addAction(QString::fromLocal8Bit("恢复"));
		connect(restoreAction, &QAction::triggered, [this]()
			{
				emit screenMergeRestore(this);
			});
	}
	QMenu splitMenu(this);
	splitMenu.addAction("2x2", [this]()
	{
		cutScreen(2, 2);
	});
	splitMenu.addAction("3x3", [this]()
	{
		cutScreen(3, 3);
	});
	splitMenu.addAction("4x4", [this]()
	{
		cutScreen(4, 4);
	});

	splitAction->setMenu(&splitMenu);
	connect(undoAction, &QAction::triggered, this, [this]()
		{
			revert(true);
		});
	contextMenu.exec(mapToGlobal(pos));
}

void frmScreen::cutScreen(int row, int col, bool needUpdate)
{
	if (row <= 0 && col <= 0) 
	{
		return;
	}

	setText("");
	m_cutRow = row;
	m_cutCol = col;
	
	if (!m_gridLayout)
	{
		m_gridLayout = new QGridLayout(this);
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
		m_gridLayout->deleteLater();
		m_gridLayout = nullptr;
	}

	for (int i = 0; i < m_cutRow; ++i)
	{
		for (int j = 0; j < m_cutCol; ++j)
		{
			frmCell* cell = new frmCell(this);
			//cell->setIndex(m_index + i * row + col);
			m_gridLayout->addWidget(cell, i, j);
		}
	}

	if (needUpdate)
	{
		emit indexUpdate(screenInfo().x, screenInfo().y, m_index);
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
	m_gridLayout->deleteLater();
	m_gridLayout = nullptr;

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

ScreenInfo frmScreen::screenInfo()
{
	return m_ChildScreens.at(0);
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
	if (event->mimeData()->hasText()) {
		event->acceptProposedAction();
	}
}

void frmScreen::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasText()) {
		QString text = event->mimeData()->text();
		setText(text);
		QFont font("Arial", 30);
		setFont(font);
		event->acceptProposedAction();
	}
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