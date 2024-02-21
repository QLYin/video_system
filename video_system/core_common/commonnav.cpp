#include "commonnav.h"
#include "qthelper.h"

//可以自行更改这两个值切换导航按钮样式
bool CommonNav::NavMainLeft = false;
bool CommonNav::NavSubLeft = false;

void CommonNav::initNavLayout(QWidget *widget, QLayout *layout, bool left)
{
    //不同的样式边距和图标大小不一样,可以自行调整对应值看效果
    int topMargin, otherMargin;
    if (left) {

    } else {
        if (CommonNav::NavMainLeft) {
            topMargin = widget->height() + 6;
            otherMargin = 9;
        } else {
            topMargin = 3;
            otherMargin = 3;
        }
    }

    layout->setContentsMargins(otherMargin, topMargin, otherMargin, otherMargin);
}

void CommonNav::initNavBtn(QToolButton *btn, const QString &name, const QString &text, bool left)
{
    btn->setObjectName(name);
    btn->setText(text);
    btn->setCheckable(true);

    //顶部导航和左侧导航拉伸策略不一样
    if (left) {
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        if (CommonNav::NavSubLeft) {
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setMinimumHeight(30);
        } else {
            btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            btn->setMinimumHeight(80);
        }

    } else {
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        if (CommonNav::NavMainLeft) {
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setMinimumWidth(115);
        } else {
            btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            btn->setMinimumWidth(80);
        }
    }
}

void CommonNav::initNavBtnIcon(QAbstractButton *btn, int icon, bool left, int offset)
{
    //可以自行调整尺寸
    int size = 20;
    int width = 25;
    int height = 20;

    //根据不同的图标位置设置不同的图标尺寸
    if (left) {
        if (!CommonNav::NavSubLeft) {
            size = 35;
            width = 40;
            height = 35;
        }
    } else {
        if (!CommonNav::NavMainLeft) {
            size = 35;
            width = 40;
            height = 35;
        }
    }

    //偏移值 可能部分图标要调整大小
    size += offset;
    setIconBtn(btn, icon, size, width, height);
}

void CommonNav::setIconBtn(QAbstractButton *btn, int icon)
{
    //可以自行调整尺寸
    int size = 16;
    int width = 18;
    int height = 18;
    setIconBtn(btn, icon, size, width, height);
}

void CommonNav::setIconBtn(QAbstractButton *btn, int icon, int size, int width, int height, int fontSize)
{
    //取出正常状态和禁用状态图片
    QPixmap pixNormal = IconHelper::getPixmap(GlobalConfig::TextColor, icon, size, width, height);
    QPixmap pixDisabled = IconHelper::getPixmap(GlobalConfig::BorderColor, icon, size, width, height);

    QIcon ico(pixNormal);
    ico.addPixmap(pixDisabled, QIcon::Disabled, QIcon::On);
    ico.addPixmap(pixDisabled, QIcon::Disabled, QIcon::Off);

    btn->setIconSize(QSize(width, height));
    btn->setIcon(ico);
    if (fontSize > 0) {
        QFont font;
        font.setPixelSize(fontSize);
        btn->setFont(font);
    }
}

void CommonNav::setIconBtn(QWidget *widget, bool like)
{
#ifdef __arm__
    setIconBtn(widget, 20, 22, 22, like);
#else
    setIconBtn(widget, 16, 18, 18, like);
#endif
}

void CommonNav::setIconBtn(QWidget *widget, int size, int width, int height, bool like, int fontSize)
{
    //定义名称和图标对应集合
    QList<QString> names;
    names << "btnAdd" << "btnSave" << "btnDelete" << "btnReturn" << "btnClear";
    names << "btnInput" << "btnOutput" << "btnPrint" << "btnPdf" << "btnXls";
    names << "btnPlot" << "btnSnap" << "btnSelect" << "btnSearch" << "btnPlus";
    names << "btnPlay" << "btnPause" << "btnStop" << "btnUpload" << "btnDownload";
    names << "btnPrevious" << "btnNext" << "btnFirst" << "btnLast";
    names << "btnCompress" << "btnBackward" << "btnForeward";
    names << "btnSelectAll" << "btnSelectNone" << "btnSelectReverse";

    QList<int> icons;
    icons << 0xf067 << 0xf0c7 << 0xf00d << 0xf122 << 0xf1f8;
    icons << 0xf063 << 0xf062 << 0xf02f << 0xf1c1 << 0xf1c3;
    icons << 0xf1fe << 0xf03e << 0xf002 << 0xf002 << 0xf0fe;
    icons << 0xf04b << 0xf04d << 0xf04d << 0xf093 << 0xf019;
    icons << 0xf04a << 0xf04e << 0xf049 << 0xf050;
    icons << 0xf066 << 0xf060 << 0xf061;
    icons << 0xf0a6 << 0xf0a7 << 0xf1da;

    setIconBtn(names, icons, widget, size, width, height, like, fontSize);
}

void CommonNav::setIconBtn(const QList<QString> &names, const QList<int> &icons, QWidget *widget, int size, int width, int height, bool like, int fontSize)
{
    //取出按钮集合,根据对象名称进行图标设置
    QList<QAbstractButton *> btns = widget->findChildren<QAbstractButton *>();
    foreach (QAbstractButton *btn, btns) {
        //后期还可以考虑没有找到图标的默认图标 0xf188
        //切记 QCheckBox QTableCornerButton QPrevNextCalButton 也是继承自 QAbstractButton
        QString objName = btn->objectName();
        if (like) {
            for (int i = 0; i < names.count(); ++i) {
                if (objName.startsWith(names.at(i))) {
                    CommonNav::setIconBtn(btn, icons.at(i), size, width, height, fontSize);
                    break;
                }
            }
        } else {
            int index = names.indexOf(objName);
            if (index >= 0) {
                CommonNav::setIconBtn(btn, icons.at(index), size, width, height, fontSize);
            }
        }
    }
}
