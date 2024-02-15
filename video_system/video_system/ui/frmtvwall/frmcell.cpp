﻿#include "frmcell.h"
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
	if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		event->setDropAction(Qt::CopyAction);
		event->accept();
	}
	else if (event->mimeData()->hasFormat("text/uri-list")) {
		event->setDropAction(Qt::LinkAction);
		event->accept();
	}
	else {
		event->ignore();
	}
}

void frmCell::dropEvent(QDropEvent* event)
{
	QString text;
	if (event->mimeData()->hasUrls()) {
		text = event->mimeData()->urls().first().toLocalFile();
	}
	else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
		QTreeWidget* treeWidget = (QTreeWidget*)event->source();
		if (treeWidget) {
			//过滤父节点(那个一般是NVR)
			QTreeWidgetItem* item = treeWidget->currentItem();
			if (item->parent()) {
				text = item->text(0);
				setText(text);
				QFont font("Arial", 8);
				setFont(font);
				emit dropInfo(text);
			}
		}
	}
}