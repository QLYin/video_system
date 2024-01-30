#ifndef INDICATOR_H
#define INDICATOR_H

#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>

class Indicator
{
public:
	void showLoading(bool loading, QWidget* wdiget);
	void showTopTip(const QString& text, QWidget* widget);

};

#endif