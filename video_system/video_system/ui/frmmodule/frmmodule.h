#ifndef FRMMODULE_H
#define FRMMODULE_H

#include <QMainWindow>

namespace Ui {
class frmModule;
}

class frmModule : public QMainWindow
{
    Q_OBJECT

public:
    explicit frmModule(QWidget *parent = 0);
    ~frmModule();

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

private:
    Ui::frmModule *ui;

    //停靠窗体集合
    QList<QDockWidget *> dockWidgets;
    //切换到其他窗体暂时隐藏的浮动窗体
    QList<QDockWidget *> hideWidgets;
    //所有停靠窗体的默认宽高
    QList<int> dockWidths, dockHeights;

private slots:
    //初始化窗体
    void initForm();
    //设置所有停靠窗体的默认宽高
    void initSize();
    //初始化菜单文本
    void initMenu();

    //初始化所有停靠窗体
    void initWidget();
    //新建一个停靠窗体,传入内容窗体+标题+宽度+高度
    QDockWidget *newWidget(QWidget *widget, const QString &title, int width, int height);

    //将停靠窗体添加到对应的位置
    void addWidget();
    //将指定停靠窗体加入到对应布局位置
    void addWidget(int index, int position);
    void addWidget(QDockWidget *widget, int position);

    //窗体透明度改变自动立即应用
    void changeWindowOpacity();
    //全屏切换自动加载对应的布局
    void fullScreen(bool full);
    void fullScreen();

    //加载+保存+复位 布局配置
    QString getLayoutIni(bool full);
    void initLayout(bool full = false);
    void saveLayout(bool full = false);
    void resetLayout(bool full = false);    

    //执行模块显示隐藏
    void visibilityChanged(bool visible);
    //主界面菜单切换模块隐藏显示
    void visibilityChangedFromMain(const QString &title, bool visible);

signals:
    //停靠窗体显示隐藏信号通知顶部菜单发生变化
    void visibilityChangedFromModule(const QString &title, bool visible);
    //获取所有模块名称集合用于主界面生成菜单
    void loadModuleFinshed(const QList<QString> &titles, const QList<bool> &visibles);
};

#endif // FRMMODULE_H
