#ifndef FRMVIDEOWIDGETVOLUME_H
#define FRMVIDEOWIDGETVOLUME_H

#include <QWidget>
class VideoWidget;

namespace Ui {
class frmVideoWidgetVolume;
}

class frmVideoWidgetVolume : public QWidget
{
    Q_OBJECT

public:
    explicit frmVideoWidgetVolume(VideoWidget *videoWidget, QWidget *parent = 0);
    ~frmVideoWidgetVolume();

protected:
    //尺寸发生变化
    void resizeEvent(QResizeEvent *);

private:
    Ui::frmVideoWidgetVolume *ui;
    VideoWidget *videoWidget;

private slots:
    //音频数据振幅
    void receiveLevel(qreal leftLevel, qreal rightLevel);
};

#endif // FRMVIDEOWIDGETVOLUME_H
