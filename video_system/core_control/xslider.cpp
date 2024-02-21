#pragma execution_character_set("utf-8")

#include "xslider.h"
#include "qevent.h"
#include "qpainter.h"
#include "qstyle.h"
#include "qstyleoption.h"
#include "qdebug.h"

XSlider::XSlider(QWidget *parent) : QSlider(parent)
{
    this->initForm();
    this->initStyle();
}

void XSlider::paintEvent(QPaintEvent *e)
{
    //先绘制父类
    QSlider::paintEvent(e);
    if (!showText) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    //拿到滑块的区域
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QRect rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    //矫正宽高形成正方形
    int x = rect.x();
    int y = rect.y();
    int width = rect.width();
    int height = rect.height();
    if (width < height) {
        width = height;
    } else {
        height = width;
    }

    //根据长度自动计算宽度
    float offset = 2;
    QString text = QString::number(value());
    int len = text.length();
    if (len == 1) {
        offset = 1.5;
    } else if (len == 2) {
        offset = 2.0;
    } else if (len == 3) {
        offset = 2.3;
    }

    //根据显示区域大小填充字体大小
    QFont font;
    font.setPixelSize(width / offset);
    painter.setFont(font);
    painter.drawText(x, y, width, height, Qt::AlignCenter, text);
}

void XSlider::mousePressEvent(QMouseEvent *e)
{
    //限定必须是鼠标左键按下
    if (e->button() & Qt::RightButton) {
        return;
    }

    //获取鼠标的位置
    double pos, value;
    if (orientation() == Qt::Horizontal) {
        pos = e->pos().x() / (double)width();
        value = pos * (maximum() - minimum()) + minimum();
    } else {
        pos = e->pos().y() / (double)height();
        value = maximum() - pos * (maximum() - minimum()) + minimum();
    }

    //设置当前鼠标按下去的值
    setValue(value + 0.5);

    //发送自定义的鼠标单击信号
    Q_EMIT clicked();
    QSlider::mousePressEvent(e);
}

void XSlider::initForm()
{
    showText = true;
    sliderHeight = 18;

    normalColor = QColor("#A0D7DA");
    grooveColor = QColor("#22A3A9");
    borderColor = QColor("#0E99A0");
    handleColor = QColor("#FFFFFF");
    textColor = QColor("#000000");
    baseColor = QColor("#22A3A9");

    this->setOrientation(Qt::Horizontal);
}

void XSlider::initStyle()
{
    int sliderRadius = sliderHeight / 2;
    int handleSize = (sliderHeight * 3) / 2 + (sliderHeight / 5);
    int handleRadius = handleSize / 2;
    int handleOffset = handleRadius / 2;

    QStringList list;
    int handleWidth = handleSize + sliderHeight / 5 - 1;
    list.append(QString("QSlider::horizontal{min-height:%1px;color:%2;}").arg(sliderHeight * 2).arg(textColor.name()));
    list.append(QString("QSlider::groove:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(normalColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:horizontal{background:%1;height:%2px;border-radius:%3px;}")
                .arg(grooveColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:horizontal{width:%3px;margin-top:-%4px;margin-bottom:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.8 %1,stop:1.0 %2);}")
                .arg(handleColor.name()).arg(borderColor.name()).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    //偏移一个像素
    handleWidth = handleSize + sliderHeight / 5;
    list.append(QString("QSlider::vertical{min-width:%1px;color:%2;}").arg(sliderHeight * 2).arg(textColor.name()));
    list.append(QString("QSlider::groove:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::add-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(grooveColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::sub-page:vertical{background:%1;width:%2px;border-radius:%3px;}")
                .arg(normalColor.name()).arg(sliderHeight).arg(sliderRadius));
    list.append(QString("QSlider::handle:vertical{height:%3px;margin-left:-%4px;margin-right:-%4px;border-radius:%5px;"
                        "background:qradialgradient(spread:pad,cx:0.5,cy:0.5,radius:0.5,fx:0.5,fy:0.5,stop:0.8 %1,stop:1.0 %2);}")
                .arg(handleColor.name()).arg(borderColor.name()).arg(handleWidth).arg(handleOffset).arg(handleRadius));

    QString qss = list.join("");
    this->setStyleSheet(qss);
}

bool XSlider::getShowText() const
{
    return this->showText;
}

void XSlider::setShowText(bool showText)
{
    if (this->showText != showText) {
        this->showText = showText;
        this->update();
    }
}

int XSlider::getSliderHeight() const
{
    return this->sliderHeight;
}

void XSlider::setSliderHeight(int sliderHeight)
{
    if (this->sliderHeight != sliderHeight) {
        this->sliderHeight = sliderHeight;
        this->initStyle();
    }
}

QColor XSlider::getNormalColor() const
{
    return this->normalColor;
}

void XSlider::setNormalColor(const QColor &normalColor)
{
    if (this->normalColor != normalColor) {
        this->normalColor = normalColor;
        this->initStyle();
    }
}

QColor XSlider::getGrooveColor() const
{
    return this->grooveColor;
}

void XSlider::setGrooveColor(const QColor &grooveColor)
{
    if (this->grooveColor != grooveColor) {
        this->grooveColor = grooveColor;
        this->initStyle();
    }
}

QColor XSlider::getBorderColor() const
{
    return this->borderColor;
}

void XSlider::setBorderColor(const QColor &borderColor)
{
    if (this->borderColor != borderColor) {
        this->borderColor = borderColor;
        this->initStyle();
    }
}

QColor XSlider::getHandleColor() const
{
    return this->handleColor;
}

void XSlider::setHandleColor(const QColor &handleColor)
{
    if (this->handleColor != handleColor) {
        this->handleColor = handleColor;
        this->initStyle();
    }
}

QColor XSlider::getTextColor() const
{
    return this->textColor;
}

void XSlider::setTextColor(const QColor &textColor)
{
    if (this->textColor != textColor) {
        this->textColor = textColor;
        this->initStyle();
    }
}

QColor XSlider::getBaseColor() const
{
    return this->baseColor;
}

void XSlider::setBaseColor(const QColor &baseColor)
{
    if (this->baseColor != baseColor) {
        this->baseColor = baseColor;
        //根据基准颜色设置其他颜色
        grooveColor = baseColor;
        borderColor = baseColor;
        normalColor = baseColor.lighter(200);
        this->initStyle();
    }
}
