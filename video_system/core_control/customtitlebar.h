#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QWidget>

class QLabel;

class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = 0);
    ~CustomTitleBar();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool full;
    QLabel *labTitle;

public:
    QSize sizeHint()        const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    void setFull(bool full);
    void setTitle(const QString &title);
    void setTitleFlag(const QString &flag);
};

#endif // CUSTOMTITLEBAR_H
