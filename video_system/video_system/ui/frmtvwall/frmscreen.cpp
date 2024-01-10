#include "frmscreen.h"
#include <QPainter>
#include <QMenu>
#include <QMimeData>
#include <QDebug>

frmScreen::frmScreen(QWidget* parent)
{
	setAcceptDrops(true);
	setAlignment(Qt::AlignCenter);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &frmScreen::customContextMenuRequested,
		this, &frmScreen::showContextMenu);
	setStyleSheet("background-color: rgb(68, 73, 79);");
}

frmScreen::~frmScreen()
{

}

void frmScreen::showContextMenu(const QPoint&pos)
{
	QMenu contextMenu(this);
	QAction* splitAction = contextMenu.addAction(QString::fromLocal8Bit("切割画面"));
	QAction* undoAction = contextMenu.addAction(QString::fromLocal8Bit("撤销切割"));
	if (m_originalScreens.size() > 1) {
		contextMenu.addSeparator();
		QAction* restoreAction = contextMenu.addAction(QString::fromLocal8Bit("恢复"));
		connect(restoreAction, &QAction::triggered, [this]()
			{
				emit restore(this);
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
	connect(undoAction, &QAction::triggered, this, &frmScreen::revert);
	contextMenu.exec(mapToGlobal(pos));
}

void frmScreen::cutScreen(int row, int col)
{
	if (row <= 0 && col <= 0) 
	{
		return;
	}

	m_cutRow = row;
	m_cutCol = col;
	update();
}

void frmScreen::revert()
{
	m_cutRow = -1;
	m_cutCol = -1;
	update();
}

void frmScreen::setIndex(int index)
{
	m_index = index;
	setText("current index: " + QString::number(index));
}

int frmScreen::index()
{
	return m_index;
}

Coordinate frmScreen::coordinate()
{
	return m_originalScreens.at(0);
}

QVector<Coordinate> frmScreen::originalScreens()
{
	return m_originalScreens;
}

void frmScreen::setOriginalScreens(QVector<Coordinate> coords)
{
	m_originalScreens = coords;
}

void frmScreen::appendScreenCoordinate(int x, int y)
{
	Coordinate cor;
	cor.x = x;
	cor.y = y;
	m_originalScreens.push_back(cor);
}

void frmScreen::paintEvent(QPaintEvent* event)
{
	if (m_cutRow > 0 || m_cutCol > 0) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setPen(QPen(Qt::black, 2));
		int cellWidth = width() / m_cutCol;
		int cellHeight = height() / m_cutRow;
		// 绘制睡直分割线
		for (int i = 1; i < m_cutCol; ++i) {
			int x = i * cellWidth;
			painter.drawLine(x, 0, x, height());
		}
		// 绘制水平分线
		for (int i = 1; i < m_cutRow; ++i) {
			int y = i * cellHeight;
			painter.drawLine(0, y, width(), y);
		}
	}
	return QLabel::paintEvent(event);
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