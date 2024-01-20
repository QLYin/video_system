#ifndef FRMCELL_H
#define FRMCELL_H

#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>

class frmCell : public QLabel
{
	Q_OBJECT

public:
	frmCell(QWidget* parent = nullptr);
	~frmCell();

	void setIndex(int index);
	int index();

protected:
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);

private:
	QLabel* m_indexLabel = nullptr;
};

#endif