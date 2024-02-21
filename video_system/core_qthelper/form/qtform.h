#ifndef QTFORM_H
#define QTFORM_H

#pragma execution_character_set("utf-8")
#include <QDialog>
class QActionGroup;

namespace Ui {
class QtForm;
}

struct QtFormData {
    QString title;          //标题
    int titleHeight;        //标题栏高度
    Qt::Alignment alignment;//标题文本对齐

    int btnWidth;           //按钮宽度
    int iconSize;           //图标大小

    bool visibleMenu;       //菜单按钮可见
    bool visibleMin;        //最小化按钮可见
    bool visibleMax;        //最大化按钮可见
    bool visibleClose;      //关闭按钮可见

    bool minHide;           //最小化隐藏
    bool exitAll;           //退出整个程序
    bool changedStyle;      //切换换肤是否立即换肤

    bool onTop;             //是否置顶显示
    bool windowModal;       //窗口阻塞模式
    bool sizeGripEnabled;   //右下角尺寸手柄可用

    QtFormData() {
        title = "自定义窗体";
        titleHeight = 28;
        alignment = Qt::AlignLeft | Qt::AlignVCenter;

        btnWidth = 30;
        iconSize = 12;

        visibleMenu = false;
        visibleMin = true;
        visibleMax = true;
        visibleClose = true;

        minHide = false;
        exitAll = true;
        changedStyle = true;

        onTop = false;
        windowModal = true;
        sizeGripEnabled = false;
    }
};

class QtForm : public QDialog
{
    Q_OBJECT

public:
    explicit QtForm(QWidget *parent = 0);
    ~QtForm();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::QtForm *ui;

    bool max;                   //是否最大化
    QRect location;             //窗体大小位置
    QActionGroup *actionGroup;  //换肤菜单动作组

    QWidget *mainWidget;        //主窗体对象
    QtFormData formData;        //窗体数据

public:
    QSize sizeHint()            const;
    QSize minimumSizeHint()     const;

private slots:
    //初始化窗体数据
    void initForm();
    //更换样式
    void changeStyle();

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();

public:
    //设置主窗体
    void setMainWidget(QWidget *mainWidget);
    //设置窗体数据
    void setFormData(const QtFormData &formData);

    //设置默认选中的换肤菜单
    void setQssChecked(const QString &qssFile);

Q_SIGNALS:
    void changeStyle(const QString &qssFile);
    void closing();
};

#endif // QTFORM_H
