#include "customtitlebar.h"
#include "qdockwidget.h"
#include "qlabel.h"
#include "qlayout.h"
#include "qevent.h"

CustomTitleBar::CustomTitleBar(QWidget *parent) : QWidget(parent)
{
    //垂直布局,可以自定义右侧增加最大化最小化关闭按钮
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    full = false;
    labTitle = new QLabel;
    labTitle->setObjectName("dockTitle");
    labTitle->setProperty("flag", "title");
    labTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(labTitle);

    //绑定事件过滤器识别双击
    this->installEventFilter(this);
    //设置可以应用父类的样式
    this->setAttribute(Qt::WA_StyledBackground, true);
}

CustomTitleBar::~CustomTitleBar()
{

}

bool CustomTitleBar::eventFilter(QObject *watched, QEvent *event)
{
    //停靠窗体双击默认变成浮动,浮动状态下双击最大化,再次双击则恢复浮动
    if (event->type() == QEvent::MouseButtonDblClick) {
        QDockWidget *dockWidget = (QDockWidget *)parentWidget();
        if (dockWidget) {
            //先判断是否设置了固定尺寸
            if (dockWidget->property("FixedSize").toBool()) {
                return true;
            }

            //限定悬浮的窗体才有这个处理机制
            if (dockWidget->isFloating()) {
                if (full) {
                    if (dockWidget->isFullScreen()) {
                        dockWidget->showNormal();
                    } else {
                        dockWidget->showFullScreen();
                    }
                } else {
                    if (dockWidget->isMaximized()) {
                        dockWidget->showNormal();
                    } else {
                        dockWidget->showMaximized();
                    }
                }

                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

QSize CustomTitleBar::sizeHint() const
{
    return labTitle->sizeHint();
}

QSize CustomTitleBar::minimumSizeHint() const
{
    return labTitle->minimumSizeHint();
}

void CustomTitleBar::setFull(bool full)
{
    this->full = full;
}

void CustomTitleBar::setTitle(const QString &title)
{
    labTitle->setText(title);
}

void CustomTitleBar::setTitleFlag(const QString &flag)
{
    labTitle->setProperty("flag", flag);
}
