#ifndef FRMTVWALL_H
#define FRMTVWALL_H

#include <QWidget>

class frmTVWall : public QWidget
{
    Q_OBJECT

public:
    explicit frmTVWall(QWidget* parent = 0);
    ~frmTVWall();


private slots:
    void initForm();
};

#endif // FRMTVWALL_H