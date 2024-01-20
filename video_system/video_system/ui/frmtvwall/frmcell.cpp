#include "frmcell.h"
#include <QMimeData>
#include <QDebug>

frmCell::frmCell(QWidget* parent)
{
	setAcceptDrops(true);
	setAlignment(Qt::AlignCenter);
	setContextMenuPolicy(Qt::NoContextMenu);
	m_indexLabel = new QLabel(this);
	m_indexLabel->setGeometry(0, 0, 20, 20);
	setStyleSheet("background : gray;");
}

frmCell::~frmCell()
{

}

void frmCell::setIndex(int index)
{
	m_indexLabel->setText(QString::number(index));
}

int frmCell::index()
{
	return m_indexLabel->text().toUInt();
}


void frmCell::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasText()) {
		event->acceptProposedAction();
	}
}

void frmCell::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasText()) {
		QString text = event->mimeData()->text();
		setText(text);
		QFont font("Arial", 30);
		setFont(font);
		event->acceptProposedAction();
	}
}