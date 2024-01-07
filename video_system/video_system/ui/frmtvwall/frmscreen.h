#ifndef FRMSCREEN_H
#define FRMSCREEN_H

#include <QLabel>
#include <QVector>
#include <QDragEnterEvent>
#include <QDragEvent>

struct Coordinate {
	int x;
	int y;
};

class frmScreen : public QLabel
{
	Q_OBJECT

public:
	frmScreen(QWidget* parent = nullptr);
	~frmScreen();

	void cutScreen(int row, int col);
	void revert();
	
	void setIndex(int index);
	int index();

	Coordinate coordinate();
	QVector<Coordinate> originalScreens();

	void setOriginalScreens(QVector<Coordinate> coords);
	void appendScreenCoordinate(int x, int y);

signals:
	void restore(frmScreen* item);

private slots:
	void showContextMenu(const QPointer& pos);

protected:
	void paintEvent(QPaintEvent* event);
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);

private:
	int m_cutRow = -1;
	int m_cutCol = -1;
	int m_index = -1;
	QVector<Coordinate> m_originalScreens;

};

#endif