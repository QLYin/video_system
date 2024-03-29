﻿#ifndef FRMTVWALLWIDGET_H
#define FRMTVWALLWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QList>
#include <QPoint>
#include "frmscreen.h"

class QGridLayout;
class frmTVWallWidget : public QWidget
{
	Q_OBJECT
public:
	frmTVWallWidget(QWidget* parent = nullptr);
	void createTVWall(int row = 3, int col = 3);
	void updateIndex(int firstX, int firstY, int index);
	void updateScreenIpc(int index, QString ip);
	int rows();
	int cols();
	frmScreen* findScreen(int row, int col);
	bool hasMergeScreen();
signals:
	void wallSetSig();
	void wallScreenJoinSig(QVector<ScreenInfo> infos, QVector<int> indexs, bool join);
	void wallScreenCutSig(int r, int c, int splitNum);
	void wallScreenTextUpdate(int r, int c);
	void wallCallVideoSig(int r, int c, int chnIndex, int id, QString ip);
	void closeVideoSig(int chnIndex);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event)  override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private slots:
	void restorScreens(frmScreen*);

private:
	void showMergeDialog();

	void mergeWidgets(const QList<QWidget*> widgets);

private:
	QPoint startPos;
	QPoint endPos;
	QList<QWidget*> childWidgets;
	QList<QWidget*> selectedWidgets;
	QGridLayout* m_gridLayout = nullptr;
	bool isPressed = false;
	bool isDrawing = false;
	int m_rows;
	int m_cols;
	QWidget* m_rectWidget = nullptr;
};

#endif // FRMTVWALLWIDGET_H
