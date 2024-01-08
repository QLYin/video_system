#ifndef FRMTVWALL_H
#define FRMTVWALL_H

#include <QWidget>
#include <QPushButton>

class frmTVWall : public QWidget
{
    Q_OBJECT

public:
    explicit frmTVWall(QWidget* parent = 0);
    ~frmTVWall();


private slots:
    void initForm();

private:
    QPushButton* btnReresh;
    QPushButton* btnCreateWall;
    QPushButton* btnCallAll;
    QPushButton* btnConnect;
};

#endif // FRMTVWALL_H