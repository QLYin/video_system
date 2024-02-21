#ifndef INDICATOR_H
#define INDICATOR_H

#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>

class Indicator
{
public:
	static void showLoading(bool loading, QWidget* wdiget);
	static void showTopTip(const QString& text, QWidget* widget);

};

#endif