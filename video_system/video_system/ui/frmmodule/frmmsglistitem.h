#ifndef FRMMSGLISTITEM_H
#define FRMMSGLISTITEM_H

#include <QWidget>
#include <QImage>
#include <QLabel>

namespace Ui {
class frmMsgListItem;
}

class frmMsgListItem : public QWidget
{
    Q_OBJECT

public:
    explicit frmMsgListItem(QWidget *parent = 0);
    ~frmMsgListItem();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmMsgListItem *ui;

    //图片对象
    QImage image;
    //显示图片标签
    QLabel labImage;

private slots:
    //初始化窗体数据
    void initForm();

public slots:
    //设置消息
    void setMsg(const QString &msg, const QString &result, const QImage &image, const QString &time);
};

#endif // FRMMSGLISTITEM_H
