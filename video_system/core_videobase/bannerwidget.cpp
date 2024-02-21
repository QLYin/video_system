#include "bannerwidget.h"

BannerWidget::BannerWidget(QWidget *parent) : QWidget(parent)
{
    //设置可以应用样式表
    this->setAttribute(Qt::WA_StyledBackground, true);

    //判断图形字体是否存在,不存在则加入
    QFontDatabase fontDb;
    if (!fontDb.families().contains("iconfont")) {
        int fontId = fontDb.addApplicationFont(":/font/iconfont.ttf");
        QStringList fontName = fontDb.applicationFontFamilies(fontId);
        if (fontName.count() == 0) {
            //qDebug() << "load iconfont.ttf error";
        }
    }

    if (fontDb.families().contains("iconfont")) {
        iconFont = QFont("iconfont");
        iconFont.setPixelSize(17);
#if (QT_VERSION >= QT_VERSION_CHECK(4,8,0))
        iconFont.setHintingPreference(QFont::PreferNoHinting);
#endif
    }

    isRecord = false;
    isCrop = false;

    label = new QLabel;
    //label->setText("xxxxxxxxxxxxxxxxxxxxxxx");
    //label->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    //label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //默认执行一次
    this->initButton();
    this->initStyle();
}

BannerPara BannerWidget::getBannerPara()
{
    return this->bannerPara;
}

bool BannerWidget::getIsRecord() const
{
    return this->isRecord;
}

bool BannerWidget::getIsCrop() const
{
    return this->isCrop;
}

void BannerWidget::btnClicked()
{
    QPushButton *btn = (QPushButton *)sender();
    QString objName = btn->objectName();
    emit btnClicked(objName);

    //根据需要切换图标以及标识
    if (objName.endsWith("btnRecord")) {
        btn->setText((QChar)0xea1c);
        btn->setObjectName(objName.replace("Record", "Stop"));
    } else if (objName.endsWith("btnStop")) {
        btn->setText((QChar)0xea1b);
        btn->setObjectName(objName.replace("Stop", "Record"));
    } else if (objName.endsWith("btnSound")) {
        btn->setText((QChar)0xe667);
        btn->setObjectName(objName.replace("Sound", "Muted"));
    } else if (objName.endsWith("btnMuted")) {
        btn->setText((QChar)0xe674);
        btn->setObjectName(objName.replace("Muted", "Sound"));
    } else if (objName.endsWith("btnCrop")) {
        this->isCrop = true;
        btn->setText((QChar)0xe793);
        btn->setObjectName(objName.replace("Crop", "Reset"));
    } else if (objName.endsWith("btnReset")) {
        this->isCrop = false;
        btn->setText((QChar)0xe703);
        btn->setObjectName(objName.replace("Reset", "Crop"));
    }
}

void BannerWidget::receivePlayFinsh()
{
    this->isRecord = false;
    this->isCrop = false;
    this->changeStatus("Stop", "Record", 0xea1b);
    this->changeStatus("Muted", "Sound", 0xe674);
    this->changeStatus("Reset", "Crop", 0xe703);
}

void BannerWidget::receiveMuted(bool muted)
{
    if (muted) {
        this->changeStatus("Sound", "Muted", 0xe667);
    } else {
        this->changeStatus("Muted", "Sound", 0xe674);
    }
}

void BannerWidget::recorderStateChanged(const RecorderState &state, const QString &file)
{
    if (state == RecorderState_Recording) {
        this->isRecord = true;
        this->changeStatus("Record", "Stop", 0xea1c);
    } else if (state == RecorderState_Stopped) {
        this->isRecord = false;
        this->changeStatus("Stop", "Record", 0xea1b);
    }

    this->showInfo(text);
}

void BannerWidget::setBannerPara(const BannerPara &bannerPara)
{
    this->bannerPara = bannerPara;
}

void BannerWidget::initButton()
{
    //检查数量是否一致
    int iconCount = bannerPara.icons.count();
    int nameCount = bannerPara.names.count();
    if (iconCount == 0 || iconCount != nameCount) {
        return;
    }

    //清空之前的按钮对象
    qDeleteAll(btns);
    btns.clear();

    //如果之前存在布局则删除布局(居然只能用delete而不是deleteLater)
    if (this->layout()) {
        delete this->layout();
    }

    //识别当前用哪个布局
    bool vertical = (bannerPara.position == BannerPosition_Left || bannerPara.position == BannerPosition_Right);
    //实例化布局
    QBoxLayout *layout = 0;
    if (vertical) {
        layout = new QVBoxLayout;
        //插入弹簧并设置布局的边距间距
        layout->addStretch();
    } else {
        layout = new QHBoxLayout;
        //插入标签放置各种信息
        layout->addWidget(label);
    }

    layout->setContentsMargins(bannerPara.margin);
    layout->setSpacing(bannerPara.spacing);
    this->setLayout(layout);

    //有多种办法来设置图片,qt内置的图标+自定义的图标+图形字体
    //既可以设置图标形式,也可以直接图形字体设置文本
#if 0
    QList<QIcon> icons;
    icons << QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    icons << QApplication::style()->standardIcon(QStyle::SP_DialogOkButton);
    icons << QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton);
#endif

    //根据位置设置布局以及添加按钮(如果不需要按钮则只需要加一行 iconCount = 0)
    for (int i = 0; i < iconCount; ++i) {
        QPushButton *btn = new QPushButton;
        //绑定按钮单击事件,用来发出信号通知
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
        //设置标识,用来区别按钮
        btn->setObjectName(bannerPara.names.at(i));
        //设置提示文字信息
        btn->setToolTip(bannerPara.tips.at(i));

        if (vertical) {
            //设置固定高度
            btn->setFixedHeight(20);
            //设置拉伸策略使得填充
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        } else {
            //设置固定宽度
            btn->setFixedWidth(20);
            //设置拉伸策略使得填充
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        }

        //设置焦点策略为无焦点,避免单击后焦点跑到按钮上
        btn->setFocusPolicy(Qt::NoFocus);

#if 0
        //设置图标大小和图标
        btn->setIconSize(QSize(16, 16));
        btn->setIcon(icons.at(i));
#else
        btn->setFont(iconFont);
        btn->setText((QChar)bannerPara.icons.at(i));
#endif

        //将按钮加到布局中
        layout->addWidget(btn);
        btns << btn;
    }
}

void BannerWidget::initStyle()
{
    //应用样式表
    QStringList list;
    QColor bgColor = bannerPara.bgColor;
    QString rgba = QString("rgba(%1,%2,%3,%4)").arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue()).arg(bannerPara.bgAlpha);
    list << QString("#bannerWidget{background:%1;border:none;}").arg(rgba);
    list << QString("QLabel{margin:0px;padding:0px;}");
    list << QString("QPushButton,QLabel{color:%1;}").arg(bannerPara.textColor.name());
    list << QString("QPushButton:pressed{color:%1;}").arg(bannerPara.pressColor.name());
    list << QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}");
    this->setStyleSheet(list.join(""));
}

void BannerWidget::showInfo(const QString &text)
{
    this->text = text;
    if (isRecord) {
        label->setText(text + " 录制中...");
    } else {
        label->setText(text);
    }
}

void BannerWidget::changeStatus(const QString &objNameSrc, const QString &objNameDst, int icon)
{
    foreach (QPushButton *btn, btns) {
        QString objName = btn->objectName();
        if (objName.endsWith(objNameSrc)) {
            objName.replace(objNameSrc, objNameDst);
            btn->setObjectName(objName);
            btn->setText((QChar)icon);
            break;
        }
    }
}
