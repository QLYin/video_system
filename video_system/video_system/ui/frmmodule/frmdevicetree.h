#ifndef FRMDEVICETREE_H
#define FRMDEVICETREE_H

#include <QWidget>

namespace Ui {
class frmDeviceTree;
}

class frmDeviceTree : public QWidget
{
    Q_OBJECT

public:
    explicit frmDeviceTree(QWidget *parent = 0);
    ~frmDeviceTree();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmDeviceTree *ui;

    //树状列表控件是否按下
    bool treePressed;
    //按下的坐标
    QPointF treePos;

private slots:
    //初始化窗体数据
    void initForm();
    //初始化右键菜单
    void initAction();
    //执行右键菜单
    void doAction();

    //应用程序退出
    void exitAll();
    //拖曳过去以后要重新刷新树状控件
    void fileDragOpen();
    //选中通道
    void selectVideo(const QString &url);
};

#endif // FRMDEVICETREE_H
