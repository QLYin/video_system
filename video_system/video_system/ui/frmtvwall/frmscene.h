#ifndef FRMSCENE_H
#define FRMSCENE_H

#include <QPushButton>
#include <QCheckBox>

class frmScene : public QWidget
{
	Q_OBJECT

public:
	frmScene(QString name, int id = 0, QWidget* parent = nullptr);
	~frmScene();

	QString sceneName();
	int sceneId();
	bool isChecked();

signals:
	void sceneCall(int);
	void sceneDelete(frmScene*);
	void checkStateChange(bool checked);

private slots:
	void onSceneClicked();
	void onSceneChecked(int);

protected:

private:
	QString m_name;
	int m_id;
	QCheckBox* m_ckbox;
	QPushButton* m_imgBtn;
};

#endif