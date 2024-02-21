#pragma execution_character_set("utf-8")

#include "xlineedit.h"
#include "qpushbutton.h"
#include "qboxlayout.h"
#include "qfontdatabase.h"
#include "qevent.h"
#include "qpainter.h"
#include "qdebug.h"

XLineEdit::XLineEdit(QWidget *parent) : QLineEdit(parent)
{
    doubleShowPwd = false;
    showPwdButton = false;

    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("FontAwesome")) {
        int fontId = fontDb.addApplicationFont(":/font/fontawesome-webfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            qDebug() << "load fontawesome-webfont.ttf error";
        }
    }

    if (fontDb.families().contains("FontAwesome")) {
        iconFont = QFont("FontAwesome");
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    //实例化密码明文按钮
    pwdButton = new QPushButton;
    connect(pwdButton, SIGNAL(clicked(bool)), this, SLOT(clicked()));
    pwdButton->setFont(iconFont);
    pwdButton->setText(QChar(0xf070));
    pwdButton->setMaximumWidth(30);
    pwdButton->setVisible(false);
    pwdButton->setStyleSheet("border:0px solid #000000;background:transparent;margin:0px;");

    //加入到布局
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(1, 1, 5, 1);
    layout->addStretch();
    layout->addWidget(pwdButton);

    //安装事件过滤器
    this->installEventFilter(this);
}

bool XLineEdit::eventFilter(QObject *watched, QEvent *event)
{
    if (doubleShowPwd) {
        int type = event->type();
        if (type == QEvent::MouseButtonDblClick) {
            this->setEchoMode(QLineEdit::Normal);
        } else if (type == QEvent::FocusOut) {
            this->setEchoMode(QLineEdit::Password);
        }
    }

    return QLineEdit::eventFilter(watched, event);
}

void XLineEdit::clicked()
{
    if (this->echoMode() == QLineEdit::Password) {
        this->setEchoMode(QLineEdit::Normal);
        pwdButton->setText(QChar(0xf06e));
    } else {
        this->setEchoMode(QLineEdit::Password);
        pwdButton->setText(QChar(0xf070));
    }
}

bool XLineEdit::getDoubleShowPwd() const
{
    return this->doubleShowPwd;
}

void XLineEdit::setDoubleShowPwd(bool doubleShowPwd)
{
    this->doubleShowPwd = doubleShowPwd;
    this->setEchoMode(doubleShowPwd ? QLineEdit::Password : QLineEdit::Normal);
}

bool XLineEdit::getShowPwdButton() const
{
    return this->showPwdButton;
}

void XLineEdit::setShowPwdButton(bool showPwdButton)
{
    this->showPwdButton = showPwdButton;
    this->setEchoMode(QLineEdit::Password);
    pwdButton->setVisible(showPwdButton);
}
