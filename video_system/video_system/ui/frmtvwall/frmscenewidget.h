#ifndef FRMSCENEWIDGET_H
#define FRMSCENEWIDGET_H

#include <QWidget>
#include <QList>
#include "frmscene.h"

class QGridLayout;
class frmSceneWidget : public QWidget
{
	Q_OBJECT
public:
	frmSceneWidget(QWidget* parent = nullptr);
	~frmSceneWidget();

	void initScenes(QMap<QString, int> sceneList);

signals:
	void sceneAddSig(int id, QString name);
	void sceneDelSig(int id);
	void sceneCallSig(int id);
	void sceneLoop(bool loop, QVector<int> ids);

protected:

private slots:
	void btnAddClicked();
	void btnLoopClicked();

private:
	QString getNewSceneName();
	int getNewSceneId();

private:
	QPushButton* m_btnAdd;
	QPushButton* m_btnLoop;
	QList<frmScene*> m_sceneList;
	QGridLayout* m_gridLayout;
};

#endif // FRMSCENEWIDGET_H
