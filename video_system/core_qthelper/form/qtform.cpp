#include "qtform.h"
#include "ui_qtform.h"
#include "qthelper.h"

QtForm::QtForm(QWidget *parent) : QDialog(parent), ui(new Ui::QtForm)
{
    ui->setupUi(this);
    this->initForm();
}

QtForm::~QtForm()
{
    delete ui;
}

bool QtForm::eventFilter(QObject *watched, QEvent *event)
{
    //处理无边框窗体可移动
    static QPoint mousePoint;
    static bool mousePressed = false;

    if (watched == this) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->type() == QEvent::MouseButtonPress) {
            if (mouseEvent->button() == Qt::LeftButton) {
                mousePressed = true;
                mousePoint = mouseEvent->globalPos() - this->pos();
                return true;
            }
        } else if (mouseEvent->type() == QEvent::MouseButtonRelease) {
            mousePressed = false;
            return true;
        } else if (mouseEvent->type() == QEvent::MouseMove) {
            if (mousePressed) {
                this->move(mouseEvent->globalPos() - mousePoint);
                return true;
            }
        }
    } else if (watched == ui->QtFormTitle) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            //只有当最大化按钮可见才说明可以最大化
            if (ui->btnMenu_Max->isVisible()) {
                this->on_btnMenu_Max_clicked();
            }
        }
    } else if (watched == mainWidget) {
        if (event->type() == QEvent::Hide) {
            this->hide();
        } else if (event->type() == QEvent::Close) {
            this->close();
        }
    }

    return QDialog::eventFilter(watched, event);
}

QSize QtForm::sizeHint() const
{
    return QSize(600, 450);
}

QSize QtForm::minimumSizeHint() const
{
    return QSize(200, 150);
}

void QtForm::initForm()
{
    //绑定事件过滤器监听鼠标移动
    this->installEventFilter(this);
    //设置阴影
    //QtHelper::setFormShadow(this, ui->verticalLayout, GlobalConfig::HighColor, 5, 15);
    //设置无边框
    QtHelper::setFramelessForm(this, ui->QtFormTitle, ui->QtLabIco, ui->btnMenu_Close, false, false, true);

    max = false;
    location = this->geometry();
    mainWidget = 0;

    //双击标题栏最大化切换
    ui->QtFormTitle->installEventFilter(this);

    //添加换肤菜单
    QStringList styleNames, styleFiles;
    GlobalStyle::getStyle(styleNames, styleFiles);

    //添加到动作分组中形成互斥效果
    actionGroup = new QActionGroup(this);
    int count = styleNames.count();
    for (int i = 0; i < count; ++i) {
        QAction *action = new QAction(this);
        //设置可选中前面有个勾勾
        action->setCheckable(true);
        action->setText(styleNames.at(i));
        action->setData(styleFiles.at(i));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle()));
        ui->btnMenu->addAction(action);
        actionGroup->addAction(action);
    }

    //默认选择一种样式
    setQssChecked(":/qss/lightblue.css");
    //设置个默认值
    setFormData(QtFormData());
}

void QtForm::changeStyle()
{
    QAction *action = (QAction *)sender();
    QString qssFile = action->data().toString();

    //有些应用可能只需要发送个换肤的信号给他就行
    if (formData.changedStyle) {
        GlobalStyle::setStyleFile(qssFile);
    }

    emit changeStyle(qssFile);
}

void QtForm::on_btnMenu_Min_clicked()
{
    if (formData.minHide) {
        this->hide();
    } else {
        this->showMinimized();
    }
}

void QtForm::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        IconHelper::setIcon(ui->btnMenu_Max, GlobalConfig::IconNormal, formData.iconSize);
    } else {
        location = this->geometry();
        this->setGeometry(QtHelper::getScreenRect());
        IconHelper::setIcon(ui->btnMenu_Max, GlobalConfig::IconMax, formData.iconSize);
    }

    max = !max;
    this->setProperty("canMove", !max);
}

void QtForm::on_btnMenu_Close_clicked()
{
    //先发送关闭信号
    emit closing();
    if (formData.exitAll) {
        mainWidget->close();
        this->close();
    }
}

void QtForm::setMainWidget(QWidget *mainWidget)
{
    //一个窗体对象只能设置一个主窗体
    if (this->mainWidget == 0) {
        //将子窗体添加到布局
        ui->QtFormMain->layout()->addWidget(mainWidget);
        //自动设置大小
        this->resize(mainWidget->width(), mainWidget->height() + ui->QtFormTitle->height());
        this->mainWidget = mainWidget;
        //安装事件过滤器识别关闭隐藏等
        //mainWidget->installEventFilter(this);
        QtHelper::setFormInCenter(this);
    }
}

void QtForm::setFormData(const QtFormData &formData)
{
    this->formData = formData;

    ui->QtLabTitle->setText(formData.title);
    this->setWindowTitle(ui->QtLabTitle->text());
    ui->QtFormTitle->setFixedHeight(formData.titleHeight);
    ui->QtLabTitle->setAlignment(formData.alignment);

    //设置置顶显示
    if (formData.onTop) {
        this->setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        this->setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }

    //设置窗口阻塞模式
    if (formData.windowModal) {
        this->setWindowModality(Qt::WindowModal);
    } else {
        this->setWindowModality(Qt::NonModal);
    }

    //设置右下角可拉伸
    this->setSizeGripEnabled(formData.sizeGripEnabled);

    //设置按钮宽度
    int btnWidth = formData.btnWidth;
    ui->QtLabIco->setFixedWidth(btnWidth + 5);
    ui->btnMenu->setFixedWidth(btnWidth);
    ui->btnMenu_Min->setFixedWidth(btnWidth);
    ui->btnMenu_Max->setFixedWidth(btnWidth);
    ui->btnMenu_Close->setFixedWidth(btnWidth);

    //设置按钮图标
    int iconSize = formData.iconSize;
    IconHelper::setIcon(ui->QtLabIco, GlobalConfig::IconMain, iconSize + 2);
    IconHelper::setIcon(ui->btnMenu, GlobalConfig::IconMenu, iconSize);
    IconHelper::setIcon(ui->btnMenu_Min, GlobalConfig::IconMin, iconSize);
    IconHelper::setIcon(ui->btnMenu_Max, GlobalConfig::IconNormal, iconSize);
    IconHelper::setIcon(ui->btnMenu_Close, GlobalConfig::IconClose, iconSize);

    //设置对应按钮是否可见
    ui->btnMenu->setVisible(formData.visibleMenu);
    ui->btnMenu_Min->setVisible(formData.visibleMin);
    ui->btnMenu_Max->setVisible(formData.visibleMax);
    ui->btnMenu_Close->setVisible(formData.visibleClose);
}

void QtForm::setQssChecked(const QString &qssFile)
{
    //选中默认的样式
    QList<QAction *> actions = actionGroup->actions();
    foreach (QAction *action, actions) {
        if (action->data().toString() == qssFile) {
            action->setChecked(true);
            break;
        }
    }
}
