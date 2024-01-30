#include "Indicator.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QPropertyAnimation>

#include "frmmain.h"
#include "class/appmisc/appmisc.h"

void Indicator::showLoading(bool loading, QWidget* widget)
{
	if (!widget)
	{
		widget = qobject_cast<QWidget*>(AppMisc::Instance()->mainWnd());
	}
	QWidget* loadingWidget = widget->findChild<QWidget*>("loading_widget");
	if (!loadingWidget)
	{
		loadingWidget = new QWidget(widget);
		loadingWidget->setObjectName("loading_widget");
		loadingWidget->setStyleSheet("background-color : #A5A5A5");
		QVBoxLayout* layout = new QVBoxLayout(loadingWidget);
		layout->setMargin(0);
		layout->setSpacing(0);

		QLabel* label = new QLabel(loadingWidget);
		layout->addWidget(label);

		QMovie* movie = new QMovie(":/res/loading.gif");
		label->setMovie(movie);
		movie->start();
	}
	
	if (loading)
	{
		loadingWidget->raise();
		loadingWidget->show();
		loadingWidget->setGeometry(0,0, widget->width(), widget->height());
	}
	else 
	{
		loadingWidget->hide();
	}
}

void Indicator::showTopTip(const QString& text, QWidget* widget)
{
	if (!widget)
	{
		widget = qobject_cast<QWidget*>(AppMisc::Instance()->mainWnd());
	}
	QLabel* tipLabel = new QLabel(text);
	tipLabel->setStyleSheet("background-color : #FFFFFF; padding: 10px; border-radius: 5px");
	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(tipLabel);

	QWidget* tipWidget = new QWidget(widget);
	tipWidget->setLayout(layout);
	tipWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
	tipWidget->setAttribute(Qt::WA_TranslucentBackground);

	QFontMetrics fontMetrics(tipLabel->font());
	int w = fontMetrics.width(text) + 50;

	tipWidget->setFixedWidth(w);

	auto widgetPos = widget->mapToGlobal(QPoint(0, 0));
	int startX = widgetPos.x();
	int startY = widgetPos.y() + 20;

	QPropertyAnimation* ani = new QPropertyAnimation(tipWidget, "pos");
	ani->setDuration(1000);
	ani->setStartValue(QPoint(startX + widget->width() / 2 - w / 2, startY - tipWidget->height()));
	ani->setEndValue(QPoint(startX + widget->width() / 2 - w / 2, startY));
	ani->setEasingCurve(QEasingCurve::OutBounce);

	tipWidget->show();
	ani->start();

	QTimer::singleShot(1500, tipWidget, &QWidget::close);
}