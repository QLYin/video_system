#ifndef AUDIOLEVEL_H
#define AUDIOLEVEL_H

#include <QWidget>

class AudioLevel : public QWidget
{
    Q_OBJECT
public:
    explicit AudioLevel(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

private:
    qreal level;
    int borderWidth;

    QColor borderColor;
    QColor bgColor;

public slots:
    void setLevel(qreal level);
    void clear();
};

#endif // AUDIOLEVEL_H
