#include "widgetsound.h"
#include "xslider.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "appconfig.h"
#include "globalconfig.h"

WidgetSound::WidgetSound(QWidget *parent) : QWidget(parent)
{
    this->setObjectName("widgetSound");
    this->setFocusPolicy(Qt::StrongFocus);
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    this->setStyleSheet(QString("#widgetSound{border:1px solid %1;}").arg(GlobalConfig::BorderColor));

    //顶部音量条
    sliderSound = new XSlider;
    sliderSound->setFocusPolicy(Qt::NoFocus);
    sliderSound->setOrientation(Qt::Vertical);
    sliderSound->setValue(AppConfig::SoundValue);
    connect(sliderSound, SIGNAL(valueChanged(int)), this, SIGNAL(soundValueChanged(int)));

    //底部静音按钮
    btnMute = new QPushButton;
    btnMute->setObjectName("btnMute");
    connect(btnMute, SIGNAL(clicked(bool)), this, SIGNAL(soundMutedChanged()));
    btnMute->setFocusPolicy(Qt::NoFocus);
    btnMute->setStyleSheet(QString("QPushButton{background:none;border:none;}"));

    //表格布局放入控件
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(sliderSound, 0, 1, 1, 1);
    QSpacerItem *spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(spacer1, 0, 0, 1, 1);
    QSpacerItem *spacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(spacer2, 0, 2, 1, 1);
    gridLayout->addWidget(btnMute, 1, 0, 1, 3);
    this->setLayout(gridLayout);
}

XSlider *WidgetSound::getSliderSound()
{
    return this->sliderSound;
}

QPushButton *WidgetSound::getBtnMute()
{
    return this->btnMute;
}

void WidgetSound::setSoundValue(int soundValue)
{
    sliderSound->setValue(soundValue);
}

void WidgetSound::setSoundMuted(bool soundMuted)
{

}
