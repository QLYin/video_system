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
			widget->setStyleSheet("background-color: yellow;");
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


void frmTVWallWidget::restorScreens(frmScreen* item)
{
	QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout());
	if (!gridLayout) return;

	if (item) {
		int currentIndex = item->index();
		childWidgets.removeOne(item);
		QVector<Coordinate> cords = item->originalScreens();
		if (cords.size() > 0)
		{
			for (auto& cordItem : cords) 
			{
				auto screenItem = new frmScreen(this);
				screenItem->setIndex(cordItem.x * m_cols + cordItem.y);
				screenItem->appendScreenCoordinate(cordItem.x, cordItem.y);

				gridLayout->addWidget(screenItem, cordItem.x, cordItem.y);
				childWidgets.append(screenItem);
			}
		}
	}
	item->deleteLater();
}


void frmTVWallWidget::createTVWall(int row, int col)
{
	m_rows = row;
	m_cols = col;
	QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout());
	if (!gridLayout)
	{
		gridLayout = new QGridLayout(this);
		gridLayout->setSpacing(5);
		setLayout(gridLayout);
	}
	for (int i = 0; i < m_rows; ++i) 
	{
		for (int j = 0; j < m_cols; ++j)
		{
			frmScreen* childWidget = new frmScreen(this);
			childWidget->setIndex(i * m_cols + j);
			childWidget->appendScreenCoordinate(i, j);
			gridLayout->addWidget(childWidget, i , j);
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
			widget->setStyleSheet("background-color: lightGray; border: 1px, soild, gray;");
		}
	}
}


void frmTVWallWidget::mergeWidgets(const QList<QWidget*> widgets)
{
	QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout());
	if (!gridLayout) return;

	QRect boundingRect = widgets.first()->geometry();
	auto firstScreen = qobject_cast<frmScreen*>(widgets.at(0));
	int index = firstScreen->index();
	int w = firstScreen->width();
	int h = firstScreen->height();
	QVector<Coordinate> cords;

	for (QWidget* wdiget : widgets) {
		auto screenItem = qobject_cast<frmScreen*>(wdiget);
		cords.push_back(screenItem->coordinate());
		boundingRect = boundingRect.united(screenItem->geometry());
		childWidgets.removeOne(screenItem);
		screenItem->deleteLater();
	}

	auto calculateSpan = [](QVector<Coordinate> cords, int& rowSpan, int& colSpan)
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
	mergeScreen->setOriginalScreens(cords);
	connect(mergeScreen, &frmScreen::restore, this, &frmTVWallWidget::restorScreens);

	int rowSpan = -1, colSpan = -1;
	calculateSpan(cords, rowSpan, colSpan);
	gridLayout->addWidget(mergeScreen, mergeScreen->coordinate().x, mergeScreen->coordinate().y);
	childWidgets.append(mergeScreen);
}