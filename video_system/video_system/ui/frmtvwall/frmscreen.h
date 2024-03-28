#ifndef FRMSCREEN_H
#define FRMSCREEN_H

#include <QLabel>
#include <QVector>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGridLayout>

struct ScreenInfo {
	int x;
	int y;
	int row;
	int col;
};

class frmScreen : public QLabel
{
	Q_OBJECT

public:
	frmScreen(QWidget* parent = nullptr);
	~frmScreen();

	void cutScreen(int cnt, bool needUpdateIndex, bool notify);
	void cutScreen(int row, int col, bool needUpdate = true, bool notify = true);
	void revert(bool needUpdate = true);
	
	void setIndex(int index);
	int index();
	int updateIndex(int index);

	ScreenInfo screenInfo();
	QVector<ScreenInfo> childScreenInfos();

	void setChildScreenInfos(QVector<ScreenInfo> infos);
	void appendScreenInfo(int x, int y, int row, int col);
	bool hasCut();
	int cutRow();
	int cutCol();
	void setCellText(int index, QString text);
	void setEnableDrop(bool enable);
	void setSelected(bool selected);
signals:
	void indexUpdate(int x, int y, int nextIndex);
	void screenMergeRestore(frmScreen* item);
	void screenCut(int spiltNum);
	void dropInfo(int chnIndex, int ipcId, QString text);
	void closeVideo(int chnIndex);
	void screenTextUpdate();

private slots:
	void showContextMenu(const QPoint& pos);

protected:
	void dragEnterEvent(QDragEnterEvent* event);
	void dropEvent(QDropEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);

private:
	int findChnIndexByPos(const QPoint& pos, bool clear = true);

private:
	int m_cutRow = -1;
	int m_cutCol = -1;
	int m_index = -1;
	QVector<ScreenInfo> m_ChildScreens;
	QGridLayout* m_gridLayout = nullptr;
	QLabel* m_topLabel = nullptr;
	bool m_enableDrop = true;
};

#endif