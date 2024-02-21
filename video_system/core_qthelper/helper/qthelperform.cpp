#include "qthelperform.h"
#include "iconhelper.h"
#include "qthelper.h"
#include "qtmessagebox.h"
#include "qttipbox.h"
#include "qtdateselect.h"
#include "qtinputbox.h"
#include "qtabout.h"
#include "qtsplash.h"

bool QtHelperForm::formShadow = true;
void QtHelperForm::setFormShadow(QWidget *widget, QLayout *layout, const QString &color, int margin, int radius)
{
    //在部分linux系统设置了背景透明是黑色的所以限定只在win
#ifndef Q_OS_WIN
    return;
#endif
    if (margin <= 0 || radius <= 0 || !formShadow) {
        return;
    }

    //先判断是否已经存在阴影效果
    QGraphicsDropShadowEffect *shadowEffect = (QGraphicsDropShadowEffect *)widget->graphicsEffect();
    if (shadowEffect == 0) {
        shadowEffect = new QGraphicsDropShadowEffect(widget);
    }

    //采用系统自带的函数设置阴影
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(color);
    shadowEffect->setBlurRadius(radius);
    widget->setGraphicsEffect(shadowEffect);

    //必须设置背景透明
    widget->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置布局边距留出空间给边框阴影
    layout->setContentsMargins(margin, margin, margin, margin);
}

void QtHelperForm::setFormShadow(const QString &color)
{
    if (!formShadow) {
        return;
    }

    //重新应用边框阴影颜色等
    QGraphicsDropShadowEffect *shadowEffect = 0;

    //消息框
    shadowEffect = (QGraphicsDropShadowEffect *) QtMessageBox::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }

    //输入框
    shadowEffect = (QGraphicsDropShadowEffect *) QtInputBox::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }

    //右下角提示框
    shadowEffect = (QGraphicsDropShadowEffect *) QtTipBox::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }

    //日期选择框
    shadowEffect = (QGraphicsDropShadowEffect *) QtDateSelect::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }

    //关于对话框
    shadowEffect = (QGraphicsDropShadowEffect *) QtAbout::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }

    //中间提示信息框
    shadowEffect = (QGraphicsDropShadowEffect *) QtSplash::Instance()->graphicsEffect();
    if (shadowEffect) {
        shadowEffect->setColor(color);
    }
}

void QtHelperForm::setFramelessForm(QWidget *widgetMain, bool tool, bool top, bool menu, bool x11)
{
    //设置弱属性 form 会产生边框
    widgetMain->setProperty("form", true);
    //设置弱属性 camMove 表示当前窗体可以移动
    widgetMain->setProperty("canMove", true);

    //根据设定逐个追加属性
    widgetMain->setWindowFlags(Qt::FramelessWindowHint);
    if (tool) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::Tool);
    }
    if (top) {
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowStaysOnTopHint);
    }
    if (menu) {
        //如果是其他系统比如neokylin会产生系统边框
#ifdef Q_OS_WIN
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
#endif
    }
    if (x11) {
        //开启后忽略系统的窗口管理,窗体可以拖动到屏幕外
#ifdef __arm__
        widgetMain->setWindowFlags(widgetMain->windowFlags() | Qt::X11BypassWindowManagerHint);
#endif
    }
}

void QtHelperForm::setFramelessForm(QWidget *widgetMain, QWidget *widgetTitle,
                                     QLabel *labIco, QPushButton *btnClose,
                                     bool tool, bool top, bool menu, bool x11)
{
    //设置固定尺寸
    labIco->setFixedWidth(QtTitleMinSize);
    btnClose->setFixedWidth(QtTitleMinSize);
    widgetTitle->setFixedHeight(QtTitleMinSize);
    widgetTitle->setProperty("form", "title");

    //设置无边框属性
    setFramelessForm(widgetMain, tool, top, menu, x11);

    //设置图标
    IconHelper::setIcon(labIco, GlobalConfig::IconMain, GlobalConfig::FontSize + 2);
    IconHelper::setIcon(btnClose, GlobalConfig::IconClose, GlobalConfig::FontSize);
}

bool QtHelperForm::isCustomUI = false;
int QtHelperForm::showFormExec(QWidget *widget, const QString &title, bool windowModal)
{
    QtForm form;
    QtFormData formData;
    formData.title = title;
    formData.visibleMin = false;
    formData.visibleMax = false;
    formData.onTop = true;
    formData.windowModal = windowModal;
    form.setFormData(formData);
    form.setMainWidget(widget);
    QtHelper::setFormInCenter(&form);
    return form.exec();
}

int QtHelperForm::showMessageBox(const QString &text, int type, int timeout, bool exec)
{
    int result = 0;
    if (type == 0) {
        showMessageBoxInfo(text, timeout, exec);
    } else if (type == 1) {
        showMessageBoxError(text, timeout, exec);
    } else if (type == 2) {
        result = showMessageBoxQuestion(text);
    }

    return result;
}

void QtHelperForm::showMessageBoxInfo(const QString &text, int timeout, bool exec)
{
    if (isCustomUI) {
        if (exec) {
            QtMessageBox msg;
            msg.setMessage(text, 0, timeout);
            msg.exec();
        } else {
            QtMessageBox::Instance()->setMessage(text, 0, timeout);
            QtMessageBox::Instance()->show();
        }
    } else {
        QMessageBox box(QMessageBox::Information, "提示", text);
        box.setStandardButtons(QMessageBox::Yes);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.exec();
        //QMessageBox::information(0, "提示", info, QMessageBox::Yes);
    }
}

void QtHelperForm::showMessageBoxError(const QString &text, int timeout, bool exec)
{
    if (isCustomUI) {
        if (exec) {
            QtMessageBox msg;
            msg.setMessage(text, 2, timeout);
            msg.exec();
        } else {
            QtMessageBox::Instance()->setMessage(text, 2, timeout);
            QtMessageBox::Instance()->show();
        }
    } else {
        QMessageBox box(QMessageBox::Critical, "错误", text);
        box.setStandardButtons(QMessageBox::Yes);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.exec();
        //QMessageBox::critical(0, "错误", info, QMessageBox::Yes);
    }
}

int QtHelperForm::showMessageBoxQuestion(const QString &text)
{
    if (isCustomUI) {
        QtMessageBox msg;
        msg.setMessage(text, 1);
        msg.update();
        return msg.exec();
    } else {
        QMessageBox box(QMessageBox::Question, "询问", text);
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setButtonText(QMessageBox::Yes, QString("确 定"));
        box.setButtonText(QMessageBox::No, QString("取 消"));
        return box.exec();
        //return QMessageBox::question(0, "询问", info, QMessageBox::Yes | QMessageBox::No);
    }
}

void QtHelperForm::showTipBox(const QString &title, const QString &tip, bool fullScreen, bool center, int timeout)
{
    QtTipBox::Instance()->setTip(title, tip, fullScreen, center, timeout);
    QtTipBox::Instance()->show();
}

void QtHelperForm::hideTipBox()
{
    QtTipBox::Instance()->hide();
}

QString QtHelperForm::showInputBox(const QString &title, int type, int timeout,
                                    const QString &placeholderText, bool pwd,
                                    const QString &defaultValue)
{
    QString result;
    if (isCustomUI) {
        QtInputBox input;
        input.setParameter(title, type, timeout, placeholderText, pwd, defaultValue);
        if (input.exec() == QMessageBox::Ok) {
            result = input.getValue();
        }
    } else {
        result = QInputDialog::getText(0, "输入框", title);
    }

    return result;
}

int QtHelperForm::showDateSelect(QString &dateStart, QString &dateEnd, const QString &format)
{
    QtDateSelect dateSelect;
    dateSelect.setFormat(format);
    int result = dateSelect.exec();
    dateStart = dateSelect.getDateTimeStart();
    dateEnd = dateSelect.getDateTimeEnd();
    return result;
}

void QtHelperForm::showAboutInfo(const AboutInfo &info, int timeout, bool exec)
{
    if (exec) {
        QtAbout about;
        about.setInfo(info, timeout);
        about.exec();
    } else {
        QtAbout::Instance()->setInfo(info, timeout);
        QtAbout::Instance()->show();
    }
}

void QtHelperForm::showSplashInfo(const QString &text, int fontSizeMain, int fontSizeSub, int timeout, bool exec)
{
    if (exec) {
        //阻塞模式还有个小问题,尺寸不会自适应
        QtSplash splash;
        splash.setInfo(text, fontSizeMain, fontSizeSub, timeout);
        splash.exec();
    } else {
        QtSplash::Instance()->setInfo(text, fontSizeMain, fontSizeSub,timeout);
        QtSplash::Instance()->show();
    }
}

void QtHelperForm::hideSplashInfo()
{
    QtSplash::Instance()->hide();
}
