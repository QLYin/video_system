#ifndef FRMIPCPTZ_H
#define FRMIPCPTZ_H

#include <QWidget>
class QPushButton;

namespace Ui {
class frmIpcPtz;
}

class frmIpcPtz : public QWidget
{
    Q_OBJECT

public:
    explicit frmIpcPtz(QWidget *parent = 0);
    ~frmIpcPtz();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmIpcPtz *ui;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化按钮图标
    void initIcon();

private slots:
    //鼠标按下的区域,共9个,从0-8依次表示底部/左下角/左侧/左上角/顶部/右上角/右侧/右下角/中间
    void mousePressed(int position, const QString &strPosition);
    void mouseReleased(int position, const QString &strPosition);
    void ptzControl(int position);
};

#endif // FRMIPCPTZ_H
