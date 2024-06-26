#ifndef FRMTVWALL_H
#define FRMTVWALL_H

#include <QWidget>
#include <QPushButton>

class frmTVWallWidget;
class frmSceneWidget;
class frmTVWall : public QWidget
{
    Q_OBJECT

public:
    explicit frmTVWall(QWidget* parent = 0);
    ~frmTVWall();

private:
    bool reconnect();

public slots:
    void initForm();
    void on_btnRereshClicked();
    void on_btnCreateWallClicked();
    void on_btnCallAllClicked();
    void on_btnConnectClicked();
    void on_btnCloseAllClicked();
    void on_btnSceneClicked();

private:
    QPushButton* btnReresh;
    QPushButton* btnCreateWall;
    QPushButton* btnCallAll;
    QPushButton* btnCloseAll;
    QPushButton* btnConnect;
    QPushButton* btnScene;
    frmTVWallWidget* m_tvWallWidget;
    frmSceneWidget* m_sceneWidget;
};

#endif // FRMTVWALL_H