#ifndef FRMTVWALLWIDGET_H
#define FRMTVWALLWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QList>
#include <QPoint>

class frmScreen;
class QGridLayout;
class frmTVWallWidget : public QWidget
{
	Q_OBJECT
public:
	frmTVWallWidget(QWidget* parent = nullptr);
	void createTVWall(int row = 3, int col = 3);
	void updateIndex(int firstX, int firstY, int index);
	void updateScreenIpc(int index, QString ip);

signals:
	void wallSetSig();
	void wallScreenJoinSig(QVector<int> indexs, bool join);
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event)  override;
	void mouseReleaseEvent(QMouseEvent* event) override;

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
	int m_rows;
	int m_cols;

};

#endif // FRMTVWALLWIDGET_H
