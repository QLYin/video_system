#include "frmtvwallwidget.h"

#include <QPainter>
#include <QtWidgets>
#include <QDebug>
#include <algorithm>
#include "frmscreen.h"

frmTVWallWidget::frmTVWallWidget(QWidget* parent)
{
	//resize(1000, 840);
	createTVWall(3, 3);

	setContextMenuPolicy(Qt::NoContextMenu);
	setStyleSheet("border: 1px, solid, red");
}

void frmTVWallWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		isPressed = true;
		startPos = event->pos();
		selectedWidgets.clear();
	}
}

void frmTVWallWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!isPressed)
	{
		return;
	}

	QRect slectedRect = QRect(startPos, event->pos()).normalized();
	for (QWidget* widget : childWidgets)
	{
		if (slectedRect.intersects(widget->geometry()))
		{
			if (!selectedWidgets.contains(widget))
			{
				selectedWidgets.append(widget);
			}
			widget->setStyleSheet("background-color: rgb(71, 140, 182);");
		}
	}
	update();
}

void frmTVWallWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		isPressed = false;
		showMergeDialog();
	}
}


void frmTVWallWidget::restorScreens(frmScreen* mergeScreen)
{
	if (!m_gridLayout) return;

	if (mergeScreen) {
		int firstX = mergeScreen->screenInfo().x;
		int firstY = mergeScreen->screenInfo().y;
		int currentIndex = mergeScreen->index();
		childWidgets.removeOne(mergeScreen);
		QVector<ScreenInfo> infos = mergeScreen->childScreenInfos();
		bool needUpdate = false;
		if (infos.size() > 0)
		{
			for (auto& info : infos)
			{
				auto screenItem = new frmScreen(this);
				screenItem->setIndex(info.x * m_cols + info.y);
				screenItem->appendScreenInfo(info.x, info.y, info.row, info.col);
				if (info.row > 1 || info.col > 1)
				{
					screenItem->cutScreen(info.row, info.col, false);
					needUpdate = true;
				}

				m_gridLayout->addWidget(screenItem, info.x, info.y, 1, 1);
				childWidgets.append(screenItem);
			}
		}
		mergeScreen->deleteLater();
		if (needUpdate)
		{
			updateIndex(firstX, firstY, currentIndex);
		}
	}
}


void frmTVWallWidget::createTVWall(int row, int col)
{
	m_rows = row;
	m_cols = col;
	
	if (!m_gridLayout) {
		m_gridLayout = new QGridLayout(this);
		setLayout(m_gridLayout);
		m_gridLayout->setSpacing(5);
	}
	else
	{
		/*QLayoutItem* child;
		while ((child = m_gridLayout->takeAt(0)) != 0) {
			if (child->widget()) {
				child->widget()->setParent(NULL);
				m_gridLayout->removeWidget(child->widget());
				delete child;
			}
		}*/
		if (childWidgets.size() > 0)
		{
			for (auto& widget : childWidgets)
			{
				widget->setParent(NULL);
				delete widget;
			}
			childWidgets.clear();
		}
		
	}
		
	for (int i = 0; i < m_rows; ++i) 
	{
		for (int j = 0; j < m_cols; ++j)
		{
			frmScreen* childWidget = new frmScreen(this);
			connect(childWidget, &frmScreen::indexUpdate, this, &frmTVWallWidget::updateIndex);
			childWidget->setIndex(i * m_cols + j);
			childWidget->appendScreenInfo(i, j, -1, -1);
			m_gridLayout->addWidget(childWidget, i , j);
			childWidgets.append(childWidget);
		}
	}

}

void frmTVWallWidget::showMergeDialog()
{
	if (selectedWidgets.size() < 2) return;

	QDialog dialog(this);
	QVBoxLayout* layout = new QVBoxLayout(&dialog);
	dialog.setLayout(layout);

	QLabel* label = new QLabel("Selected Widgets:", &dialog);
	layout->addWidget(label);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	QPushButton* mergeButton = new QPushButton("Merge Display", &dialog);
	QPushButton* cancelButton = new QPushButton("Cancel", &dialog);

	connect(mergeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
	buttonLayout->addWidget(mergeButton);
	buttonLayout->addWidget(cancelButton);
	layout->addLayout(buttonLayout);

	if (dialog.exec() == QDialog::Accepted) 
	{
		mergeWidgets(selectedWidgets);
	}
	else
	{
		for (auto& widget : selectedWidgets)
		{
			widget->setStyleSheet(" background-color: rgb(68, 73, 79);");
		}
	}
}


void frmTVWallWidget::mergeWidgets(const QList<QWidget*> widgets)
{
	if (!m_gridLayout) return;

	QRect boundingRect = widgets.first()->geometry();
	auto firstScreen = qobject_cast<frmScreen*>(widgets.at(0));
	int index = firstScreen->index();
	int w = firstScreen->width();
	int h = firstScreen->height();
	QVector<ScreenInfo> infos;

	for (QWidget* wdiget : widgets) {
		auto screenItem = qobject_cast<frmScreen*>(wdiget);
		infos.push_back(screenItem->screenInfo());
		boundingRect = boundingRect.united(screenItem->geometry());
		childWidgets.removeOne(screenItem);
		screenItem->deleteLater();
	}

	auto calculateSpan = [](QVector<ScreenInfo> cords, int& rowSpan, int& colSpan)
	{
		QVector<int> rows, cols;
		for (auto& item : cords) {
			rows.push_back(item.x);
			cols.push_back(item.y);
		}

		int minRow = *std::min_element(rows.begin(), rows.end());
		int maxRow = *std::max_element(rows.begin(), rows.end());
		rowSpan = maxRow - minRow + 1;

		int minCol = *std::min_element(cols.begin(), cols.end());
		int maxCol = *std::max_element(cols.begin(), cols.end());
		colSpan = maxCol - minCol + 1;

	};

	auto mergeScreen = new frmScreen(this);
	mergeScreen->setIndex(index);
	mergeScreen->setChildScreenInfos(infos);
	connect(mergeScreen, &frmScreen::screenMergeRestore, this, &frmTVWallWidget::restorScreens);
	connect(mergeScreen, &frmScreen::indexUpdate, this, &frmTVWallWidget::updateIndex);

	int rowSpan = -1, colSpan = -1;
	calculateSpan(infos, rowSpan, colSpan);
	m_gridLayout->addWidget(mergeScreen, mergeScreen->screenInfo().x, mergeScreen->screenInfo().y, rowSpan, colSpan);
	childWidgets.append(mergeScreen);
}

void frmTVWallWidget::updateIndex(int firstX, int firstY, int index)
{
	if (firstX < 0 || firstY < 0 || index < 0)
	{
		return;
	}
	if (!m_gridLayout) return;

	QSet<frmScreen*> screenList;
	int currentIndex = index;
	for (int i = firstX; i < m_rows; ++i)
	{
		for (int j = 0; j < m_cols; ++j)
		{
			if (i == firstX && j < firstY)
			{
				continue;
			}
			QLayoutItem* child = m_gridLayout->itemAtPosition(i, j);
			if (!child) 
			{
				return;
			}
			auto screen = qobject_cast<frmScreen*>(child->widget());
			if (screenList.contains(screen))
			{ 
				currentIndex += 1;
			}
			else 
			{
				currentIndex = screen->updateIndex(currentIndex);
				screenList.insert(screen);
			}

		}
	}
}