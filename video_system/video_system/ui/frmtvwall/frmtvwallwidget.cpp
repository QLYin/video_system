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

		auto customCompare = [](const QWidget* widget1, const QWidget* widget2)
		{
			return widget1->pos().x() * widget1->pos().y() < widget2->pos().x() * widget2->pos().y();
		};

		std::sort(selectedWidgets.begin(), selectedWidgets.end(), customCompare);
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
		QVector<int> indexs;
		if (infos.size() > 0)
		{
			for (auto& info : infos)
			{
				auto screenItem = new frmScreen(this);
				screenItem->setIndex(info.x * m_cols + info.y + 1);
				indexs.push_back(info.x * m_cols + info.y + 1);
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

		mergeScreen->setParent(nullptr);
		mergeScreen->deleteLater();
		
		emit wallScreenJoinSig(infos, indexs, false);
		updateIndex(firstX, firstY, currentIndex);
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
			connect(childWidget, &frmScreen::screenCut, this, [i, j, this](int splitNum) 
				{
					emit wallScreenCutSig(i, j, splitNum);
				});
			connect(childWidget, &frmScreen::dropInfo, this, [i, j, this](int index, QString ip)
				{
					emit wallCallVideoSig(i, j, index, ip);
				});
			childWidget->setIndex(i * m_cols + j + 1);
			childWidget->appendScreenInfo(i, j, -1, -1);
			m_gridLayout->addWidget(childWidget, i , j);
			childWidgets.append(childWidget);
		}
	}

	emit wallSetSig();

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

	QVector<int> indexs;
	for (QWidget* wdiget : widgets) {
		auto screenItem = qobject_cast<frmScreen*>(wdiget);
		infos.push_back(screenItem->screenInfo());
		indexs.push_back(screenItem->index());
		boundingRect = boundingRect.united(screenItem->geometry());
		childWidgets.removeOne(screenItem);
		delete screenItem;
		screenItem = nullptr;
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
	connect(mergeScreen, &frmScreen::screenCut, this, [mergeScreen, this](int splitNum)
		{
			emit wallScreenCutSig(mergeScreen->screenInfo().x, mergeScreen->screenInfo().y, splitNum);
		});
	connect(mergeScreen, &frmScreen::dropInfo, this, [mergeScreen, this](int index, QString ip)
		{
			emit wallCallVideoSig(mergeScreen->screenInfo().x, mergeScreen->screenInfo().y, index, ip);
		});
	int rowSpan = -1, colSpan = -1;
	calculateSpan(infos, rowSpan, colSpan);
	m_gridLayout->addWidget(mergeScreen, mergeScreen->screenInfo().x, mergeScreen->screenInfo().y, rowSpan, colSpan);
	childWidgets.append(mergeScreen);
	emit wallScreenJoinSig(infos, indexs, true);
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

void frmTVWallWidget::updateScreenIpc(int index, QString ip)
{
	if (ip.isEmpty())
	{
		return;
	}
	for (auto& widget : childWidgets)
	{
		auto screenItem = qobject_cast<frmScreen*>(widget);
		if (screenItem->index() == index)
		{
			screenItem->setText(ip);
			QFont font("Arial", 8);
			screenItem->setFont(font);

		}
	}
}

int frmTVWallWidget::rows()
{
	return m_rows;
}

int frmTVWallWidget::cols()
{
	return m_cols;
}

frmScreen* frmTVWallWidget::findScreen(int row, int col)
{
	frmScreen* screen = nullptr;
	auto item = m_gridLayout->itemAtPosition(row, col);
	if (item)
	{
		screen =  qobject_cast<frmScreen*>(item->widget());
	}
	
	return screen;
}

bool frmTVWallWidget::hasMergeScreen()
{
	bool has = false;
	for (auto& widget : childWidgets)
	{
		auto screenItem = qobject_cast<frmScreen*>(widget);
		if (screenItem->childScreenInfos().size() > 1)
		{
			has = true;
			break;
		}
	}
	return has;
}