#ifndef FRMTVWALL_H
#define FRMTVWALL_H

#include <QWidget>
#include <QPushButton>

class frmTVWallWidget;
class frmTVWall : public QWidget
{
    Q_OBJECT

public:
    explicit frmTVWall(QWidget* parent = 0);
    ~frmTVWall();


private slots:
    void initForm();
    void on_btnRereshClicked();
    void on_btnCreateWallClicked();
    void on_btnCallAllClicked();
    void on_btnConnectClicked();

private:
    QPushButton* btnReresh;
    QPushButton* btnCreateWall;
    QPushButton* btnCallAll;
    QPushButton* btnConnect;
    frmTVWallWidget* m_tvWallWidget;
};

#endif // FRMTVWALL_H