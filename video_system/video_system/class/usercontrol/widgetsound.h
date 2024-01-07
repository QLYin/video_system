#ifndef WIDGETSOUND_H
#define WIDGETSOUND_H

#include <QWidget>

class XSlider;
class QPushButton;

class WidgetSound : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetSound(QWidget *parent = 0);
    XSlider *getSliderSound();
    QPushButton *getBtnMute();

private:
    XSlider *sliderSound;
    QPushButton *btnMute;

public slots:
    void setSoundValue(int soundValue);
    void setSoundMuted(bool soundMuted);

signals:
    //音量改变
    void soundValueChanged(int value);
    //静音切换
    void soundMutedChanged();
};

#endif // WIDGETSOUND_H
