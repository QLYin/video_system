#include "abstractvideowidget.h"
#include "widgethelper.h"
#include "bannerwidget.h"
#ifdef openglx
#include "openglinclude.h"
#endif

QString AbstractVideoWidget::recordPath = "./video";
QString AbstractVideoWidget::snapPath = "./snap";
AbstractVideoWidget::AbstractVideoWidget(QWidget *parent) : QWidget(parent)
{
    //设置支持拖放
    this->setAcceptDrops(true);
    //设置强焦点
    this->setFocusPolicy(Qt::StrongFocus);

    //句柄控件对象
    hwndWidget = new QWidget(this);
    hwndWidget->setObjectName("hwndWidget");
    //设置个垂直布局以便句柄模式下放置子控件
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    hwndWidget->setLayout(layout);

#ifdef openglx
    //GPU绘制控件依附在句柄控件上
    rgbWidget = new RgbWidget(hwndWidget);
    rgbWidget->setObjectName("rgbWidget");
    rgbWidget->setVisible(false);

    yuvWidget = new YuvWidget(hwndWidget);
    yuvWidget->setObjectName("yuvWidget");
    yuvWidget->setVisible(false);

    nv12Widget = new Nv12Widget(hwndWidget);
    nv12Widget->setObjectName("nv12Widget");
    nv12Widget->setVisible(false);
#endif

    //主绘制对象
    coverWidget = new QWidget(this);
    coverWidget->setObjectName("coverWidget");
    coverWidget->installEventFilter(this);

    //顶部工具栏(默认隐藏/鼠标进入显示/鼠标离开隐藏)
    bannerWidget = new BannerWidget(this);
    bannerWidget->setObjectName("bannerWidget");
    connect(bannerWidget, SIGNAL(btnClicked(QString)), this, SIGNAL(sig_btnClicked(QString)));
    bannerWidget->setVisible(false);

    preview = false;
    label = NULL;

    videoWidth = 0;
    videoHeight = 0;
    visualHeight = 0;
    rotate = -1;
    onlyAudio = false;
    hardware = "none";

    isRunning = false;
    isShared = false;
    isPressed = false;
    this->installEventFilter(this);

    //定时器刷新标签信息
    timerUpdate = new QTimer(this);
    connect(timerUpdate, SIGNAL(timeout()), coverWidget, SLOT(update()));
    timerUpdate->setInterval(1000);
}

AbstractVideoWidget::~AbstractVideoWidget()
{

}

void AbstractVideoWidget::clearData()
{
#ifdef openglx
    if (widgetPara.videoMode == VideoMode_Opengl) {
        rgbWidget->clear();
        yuvWidget->clear();
        nv12Widget->clear();
    }
#endif
}

void AbstractVideoWidget::showEvent(QShowEvent *)
{
    this->clearData();
}

void AbstractVideoWidget::closeEvent(QCloseEvent *)
{
    if (label) {
        label->deleteLater();
    }
    if (timerUpdate->isActive()) {
        timerUpdate->stop();
    }
}

void AbstractVideoWidget::resizeEvent(QResizeEvent *)
{
    //获取合理的尺寸(如果有旋转角度则宽高对调)
    QSize imageSize = QSize(videoWidth, videoHeight);
    if (rotate == 90 || rotate == 270) {
        imageSize = QSize(videoHeight, videoWidth);
    }

    //频谱区域高度(easyplayer内核用于底部显示可视化频谱)
    imageSize.setHeight(imageSize.height() + visualHeight);
    imageRect = WidgetHelper::getCenterRect(imageSize, this->rect(), widgetPara.borderWidth, widgetPara.scaleMode);
    if (widgetPara.borderWidth == 0 && widgetPara.scaleMode == ScaleMode_Fill) {
        imageRect = this->rect();
    }

    //句柄控件需要根据分辨率大小来调整尺寸
    hwndWidget->setGeometry(imageRect);
    //标签图片窗体永远尺寸一样
    coverWidget->setGeometry(rect());

#ifdef openglx
    //GPU显示控件依附在句柄控件上所以永远和句柄控件尺寸一样
    if (widgetPara.videoMode == VideoMode_Opengl) {
        //先要清空再设置尺寸否则可能会遇到崩溃
        this->clearData();

        QRect rect = QRect(0, 0, hwndWidget->width(), hwndWidget->height());
        if (hardware == "rgb") {
            rgbWidget->setGeometry(rect);
        } else if (hardware == "none") {
            yuvWidget->setGeometry(rect);
        } else {
            nv12Widget->setGeometry(rect);
        }
    }
#endif

    //设置悬浮工具栏的位置和宽高
    int size = widgetPara.bannerSize;
    int offset = widgetPara.borderWidth / 2;
    int offset2 = offset * 2;
    if (widgetPara.bannerPosition == BannerPosition_Top) {
        bannerWidget->setGeometry(offset, offset, this->width() - offset2, size);
    } else if (widgetPara.bannerPosition == BannerPosition_Bottom) {
        bannerWidget->setGeometry(offset, this->height() - size - offset, this->width() - offset2, size);
    } else if (widgetPara.bannerPosition == BannerPosition_Left) {
        bannerWidget->setGeometry(offset, offset, size, this->height() - offset2);
    } else if (widgetPara.bannerPosition == BannerPosition_Right) {
        bannerWidget->setGeometry(this->width() - size - offset, offset, size, this->height() - offset2);
    }
}

#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
void AbstractVideoWidget::enterEvent(QEnterEvent *)
#else
void AbstractVideoWidget::enterEvent(QEvent *)
#endif
{
    //这里可以自行增加判断(是否获取了焦点的或者是否处于预览阶段的才需要显示)
    //if (this->hasFocus()) {}
    if (isRunning && widgetPara.bannerEnable && videoWidth > 0 && videoHeight > 0) {
        bannerWidget->setVisible(true);
        //悬浮条如果宽度不够则不显示文字信息
        int width = bannerWidget->width();
        if (width > 200) {
            if (!onlyAudio && videoWidth > 0) {
                QString info = QString("%1%2 x %3").arg(width > 250 ? "分辨率: " : "").arg(videoWidth).arg(videoHeight);
                //info += QString(" / %1").arg(hardware);
                bannerWidget->showInfo(info);
            }
        } else {
            bannerWidget->showInfo("");
        }
    }
}

void AbstractVideoWidget::leaveEvent(QEvent *)
{
    //这里不用判断其他的反正永远隐藏就对了(防止中途改变过其中的变量导致无法隐藏)
    //增加电子放大期间不隐藏/方便标记当前哪个通道处于电子放大期间
    if (!bannerWidget->getIsCrop()) {
        bannerWidget->setVisible(false);
    }
}

void AbstractVideoWidget::dropEvent(QDropEvent *event)
{
    QString url;
    if (event->mimeData()->hasUrls()) {
        url = event->mimeData()->urls().first().toLocalFile();
    } else if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        QTreeWidget *treeWidget = (QTreeWidget *)event->source();
        if (treeWidget) {
            //过滤父节点(那个一般是NVR)
            QTreeWidgetItem *item = treeWidget->currentItem();
            if (item->parent()) {
                url = item->data(0, Qt::UserRole).toString();
            }
        }
    }

    if (!url.isEmpty()) {
        emit sig_fileDrag(url);
    }
}

void AbstractVideoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //拖曳进来的时候先判断下类型(非法类型则不处理)
    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else if (event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::LinkAction);
        event->accept();
    } else {
        event->ignore();
    }
}

bool AbstractVideoWidget::eventFilter(QObject *watched, QEvent *event)
{
    QEvent::Type type = event->type();
    if (watched == coverWidget) {
        if (type == QEvent::Paint) {
            //矫正颜色
            if (widgetPara.borderColor == Qt::transparent) {
                widgetPara.borderColor = palette().shadow().color();
            }
            if (widgetPara.textColor == Qt::transparent) {
                widgetPara.textColor = palette().windowText().color();
            }

            QPainter painter;
            //开始绘制
            painter.begin(coverWidget);
            //开启抗锯齿
            painter.setRenderHints(QPainter::Antialiasing);

            //绘制边框
            drawBorder(&painter);

            //如果图片不为空则绘制图片否则绘制背景
            if (!image.isNull()) {
                drawImage(&painter);
            } else {
                drawBg(&painter);
            }

            //绘制标签和图形信息
            drawInfo(&painter);

            //结束绘制
            painter.end();
        }
    } else {
        //限定鼠标左键
        if (type == QEvent::MouseButtonPress) {
            if (qApp->mouseButtons() == Qt::LeftButton) {
                isPressed = true;
                this->getPoint(event);
            }
        } else if (type == QEvent::MouseMove) {
            if (isPressed) {
                this->getPoint(event);
            }
        } else if (type == QEvent::MouseButtonRelease) {
            if (isPressed) {
                isPressed = false;
                this->getPoint(event);
            }
        }
    }

    return QWidget::eventFilter(watched, event);
}

void AbstractVideoWidget::getPoint(QEvent *event)
{
    //正在运行中的视频或者设置了图片
    if ((isRunning || !image.isNull()) && !onlyAudio) {
        QMouseEvent *e = (QMouseEvent *)event;
        QPoint p = hwndWidget->mapFromParent(e->pos());
        //过滤在绘图区域里面
        if (hwndWidget->rect().contains(p)) {
            int w = videoWidth;
            int h = videoHeight;
            WidgetHelper::rotateSize(rotate, w, h);
            int x = (double)p.x() / hwndWidget->width() * w;
            int y = (double)p.y() / hwndWidget->height() * h;
            emit sig_receivePoint(event->type(), QPoint(x, y));
        }
    }
}

void AbstractVideoWidget::drawBorder(QPainter *painter)
{
    painter->save();

    QPen pen;
    pen.setWidth(widgetPara.borderWidth);
    pen.setColor(hasFocus() ? widgetPara.focusColor : widgetPara.borderColor);
    //没有边框则不绘制边框
    painter->setPen(widgetPara.borderWidth == 0 ? Qt::NoPen : pen);

    //顺带把背景颜色这里也一并处理(有些场景需要某个控件特殊背景颜色显示)
    if (widgetPara.bgColor != Qt::transparent) {
        painter->setBrush(widgetPara.bgColor);
    }

    painter->drawRect(rect());
    painter->restore();
}

void AbstractVideoWidget::drawBg(QPainter *painter)
{
    //已经获取到了宽高不用绘制背景
    if (videoWidth != 0) {
        return;
    }

#ifdef openglx
    //GPU控件可见不用绘制背景
    if (rgbWidget->isVisible() || yuvWidget->isVisible() || nv12Widget->isVisible()) {
        return;
    }
#endif

    painter->save();

    //背景图片为空则绘制文字否则绘制背景图片
    if (widgetPara.bgImage.isNull()) {
        painter->setPen(widgetPara.textColor);
        QFont font;
        font.setPixelSize(widgetPara.bgTextSize);
        painter->setFont(font);
        painter->drawText(rect(), Qt::AlignCenter, widgetPara.bgText);
    } else {
        QRect rect = WidgetHelper::getCenterRect(widgetPara.bgImage.size(), this->rect());
        painter->drawImage(rect, widgetPara.bgImage);
    }

    painter->restore();
}

void AbstractVideoWidget::drawInfo(QPainter *painter)
{
    //只有音频和句柄模式不用绘制OSD
    if (videoWidth == 0 || onlyAudio || widgetPara.videoMode == VideoMode_Hwnd) {
        return;
    }

    //标签位置尽量偏移多一点避免遮挡
    QRect rect = image.isNull() ? coverWidget->rect() : image.rect();
    int borderWidth = widgetPara.borderWidth + 5;
    rect = QRect(rect.x() + borderWidth, rect.y() + borderWidth, rect.width() - (borderWidth * 2), rect.height() - (borderWidth * 2));

    //将标签信息绘制到遮罩层
    if (widgetPara.osdDrawMode == DrawMode_Cover) {
        foreach (OsdInfo osd, listOsd) {
            if (osd.visible) {
                painter->save();
                WidgetHelper::drawOsd(painter, osd, rect);
                painter->restore();
            }
        }
    }

    //将图形信息绘制到遮罩层
    if (widgetPara.graphDrawMode == DrawMode_Cover) {
        foreach (GraphInfo graph, listGraph) {
            painter->save();
            if (!graph.rect.isEmpty()) {
                WidgetHelper::drawRect(painter, graph.rect, graph.borderWidth, graph.borderColor);
            }
            if (!graph.path.isEmpty()) {
                WidgetHelper::drawPath(painter, graph.path, graph.borderWidth, graph.borderColor);
            }
            if (graph.points.count() > 0) {
                WidgetHelper::drawPoints(painter, graph.points, graph.borderWidth, graph.borderColor);
            }
            painter->restore();
        }
    }
}

void AbstractVideoWidget::drawImage(QPainter *painter)
{
    if (image.isNull()) {
        return;
    }

    //标签位置尽量偏移多一点避免遮挡
    QRect rect = image.isNull() ? coverWidget->rect() : image.rect();
    int borderWidth = widgetPara.borderWidth + 5;
    rect = QRect(rect.x() + borderWidth, rect.y() + borderWidth, rect.width() - (borderWidth * 2), rect.height() - (borderWidth * 2));

    //将标签信息绘制到图片上
    if (widgetPara.osdDrawMode == DrawMode_Image) {
        foreach (OsdInfo osd, listOsd) {
            if (osd.visible) {
                QPainter painter;
                painter.begin(&image);
                //painter.setRenderHints(QPainter::Antialiasing);
                WidgetHelper::drawOsd(&painter, osd, rect);
                painter.end();
            }
        }
    }

    //将图形信息绘制到图片上
    if (widgetPara.graphDrawMode == DrawMode_Image) {
        foreach (GraphInfo graph, listGraph) {
            QPainter painter;
            painter.begin(&image);
            //painter.setRenderHints(QPainter::Antialiasing);
            if (!graph.rect.isEmpty()) {
                WidgetHelper::drawRect(&painter, graph.rect, graph.borderWidth, graph.borderColor);
            }
            if (!graph.path.isEmpty()) {
                WidgetHelper::drawPath(&painter, graph.path, graph.borderWidth, graph.borderColor);
            }
            if (graph.points.count() > 0) {
                WidgetHelper::drawPoints(&painter, graph.points, graph.borderWidth, graph.borderColor);
            }
            painter.end();
        }
    }

    //绘制图片
    painter->save();
    painter->drawImage(imageRect, image);
    painter->restore();
}

int AbstractVideoWidget::getBgTextSize() const
{
    return widgetPara.bgTextSize;
}

void AbstractVideoWidget::setBgTextSize(int bgTextSize)
{
    if (widgetPara.bgTextSize != bgTextSize) {
        widgetPara.bgTextSize = bgTextSize;
        this->update();
    }
}

QString AbstractVideoWidget::getBgText() const
{
    return widgetPara.bgText;
}

void AbstractVideoWidget::setBgText(const QString &bgText)
{
    if (widgetPara.bgText != bgText) {
        widgetPara.bgText = bgText;
        this->update();
    }
}

QColor AbstractVideoWidget::getBgColor() const
{
    return widgetPara.bgColor;
}

void AbstractVideoWidget::setBgColor(const QColor &bgColor)
{
    if (widgetPara.bgColor != bgColor) {
        widgetPara.bgColor = bgColor;
        this->update();
    }
}

QColor AbstractVideoWidget::getTextColor() const
{
    return widgetPara.textColor;
}

void AbstractVideoWidget::setTextColor(const QColor &textColor)
{
    if (widgetPara.textColor != textColor) {
        widgetPara.textColor = textColor;
        this->update();
    }
}

int AbstractVideoWidget::getBorderWidth() const
{
    return widgetPara.borderWidth;
}

void AbstractVideoWidget::setBorderWidth(int borderWidth)
{
    if (widgetPara.borderWidth != borderWidth) {
        widgetPara.borderWidth = borderWidth;
        this->update();
    }
}

QColor AbstractVideoWidget::getBorderColor() const
{
    return widgetPara.borderColor;
}

void AbstractVideoWidget::setBorderColor(const QColor &borderColor)
{
    if (widgetPara.borderColor != borderColor) {
        widgetPara.borderColor = borderColor;
        this->update();
    }
}

QColor AbstractVideoWidget::getFocusColor() const
{
    return widgetPara.focusColor;
}

void AbstractVideoWidget::setFocusColor(const QColor &focusColor)
{
    if (widgetPara.focusColor != focusColor) {
        widgetPara.focusColor = focusColor;
        this->update();
    }
}

bool AbstractVideoWidget::getBannerEnable() const
{
    return widgetPara.bannerEnable;
}

void AbstractVideoWidget::setBannerEnable(bool bannerEnable)
{
    widgetPara.bannerEnable = bannerEnable;
}

int AbstractVideoWidget::getBannerBgAlpha() const
{
    return bannerWidget->getBannerPara().bgAlpha;
}

void AbstractVideoWidget::setBannerBgAlpha(int bannerBgAlpha)
{
    BannerPara para = bannerWidget->getBannerPara();
    if (para.bgAlpha != bannerBgAlpha) {
        para.bgAlpha = bannerBgAlpha;
        bannerWidget->setBannerPara(para);
        bannerWidget->initStyle();
    }
}

QColor AbstractVideoWidget::getBannerBgColor() const
{
    return bannerWidget->getBannerPara().bgColor;
}

void AbstractVideoWidget::setBannerBgColor(const QColor &bannerBgColor)
{
    BannerPara para = bannerWidget->getBannerPara();
    if (para.bgColor != bannerBgColor) {
        para.bgColor = bannerBgColor;
        bannerWidget->setBannerPara(para);
        bannerWidget->initStyle();
    }
}

QColor AbstractVideoWidget::getBannerTextColor() const
{
    return bannerWidget->getBannerPara().textColor;
}

void AbstractVideoWidget::setBannerTextColor(const QColor &bannerTextColor)
{
    BannerPara para = bannerWidget->getBannerPara();
    if (para.textColor != bannerTextColor) {
        para.textColor = bannerTextColor;
        bannerWidget->setBannerPara(para);
        bannerWidget->initStyle();
    }
}

QColor AbstractVideoWidget::getBannerPressColor() const
{
    return bannerWidget->getBannerPara().pressColor;
}

void AbstractVideoWidget::setBannerPressColor(const QColor &bannerPressColor)
{
    BannerPara para = bannerWidget->getBannerPara();
    if (para.pressColor != bannerPressColor) {
        para.pressColor = bannerPressColor;
        bannerWidget->setBannerPara(para);
        bannerWidget->initStyle();
    }
}

void AbstractVideoWidget::setVideoFlag(const QString &videoFlag)
{
    if (widgetPara.videoFlag != videoFlag) {
        widgetPara.videoFlag = videoFlag;
    }
}

void AbstractVideoWidget::setScaleMode(const ScaleMode &scaleMode)
{
    if (widgetPara.scaleMode != scaleMode) {
        widgetPara.scaleMode = scaleMode;
        //主动调用尺寸变化应用
        this->receiveSizeChanged();
    }
}

void AbstractVideoWidget::setVideoMode(const VideoMode &videoMode)
{
    if (widgetPara.videoMode != videoMode) {
        widgetPara.videoMode = videoMode;
    }
}

void AbstractVideoWidget::setSharedData(bool sharedData)
{
    if (widgetPara.sharedData != sharedData) {
        widgetPara.sharedData = sharedData;
    }
}

int AbstractVideoWidget::getVideoWidth() const
{
    return this->videoWidth;
}

int AbstractVideoWidget::getVideoHeight() const
{
    return this->videoHeight;
}

bool AbstractVideoWidget::getIsRunning() const
{
    return this->isRunning;
}

QRect AbstractVideoWidget::getImageRect() const
{
    return this->imageRect;
}

WidgetPara AbstractVideoWidget::getWidgetPara() const
{
    return this->widgetPara;
}

void AbstractVideoWidget::setWidgetPara(const WidgetPara &widgetPara)
{
    //先要获取之前的参数
    BannerPara bannerPara = bannerWidget->getBannerPara();
    //这里过滤下变化了才需要重新加载按钮
    if (bannerPara.position != widgetPara.bannerPosition) {
        bannerPara.position = widgetPara.bannerPosition;
        bannerWidget->setBannerPara(bannerPara);
        bannerWidget->initButton();
    }

    //没有启用悬浮栏则立即隐藏
    if (!widgetPara.bannerEnable) {
        bannerWidget->setVisible(false);
    }

    //拉伸策略变了或者边框大小变了需要出发尺寸变化
    bool needResize = (this->widgetPara.scaleMode != widgetPara.scaleMode || this->widgetPara.borderWidth != widgetPara.borderWidth || this->widgetPara.bannerPosition != widgetPara.bannerPosition);
    this->widgetPara = widgetPara;
    this->update();

    //如果需要应用缩放模式还需要主动发个尺寸变化事件
    if (needResize) {
        receiveSizeChanged();
    }
}

QList<OsdInfo> AbstractVideoWidget::getListOsd() const
{
    return this->listOsd;
}

QList<GraphInfo> AbstractVideoWidget::getListGraph() const
{
    return this->listGraph;
}

void AbstractVideoWidget::setOsd(const OsdInfo &osd)
{
    //存在则替换
    bool exist = false;
    int count = listOsd.count();
    for (int i = 0; i < count; ++i) {
        OsdInfo osdInfo = listOsd.at(i);
        if (osdInfo.name == osd.name) {
            exist = true;
            listOsd[i] = osd;
            emit sig_osdChanged();
            return;
        }
    }

    //不存在则插入
    if (!exist) {
        this->appendOsd(osd);
    }
}

void AbstractVideoWidget::appendOsd(const OsdInfo &osd)
{
    QMutexLocker locker(&mutex);
    listOsd << osd;
    this->update();
    emit sig_osdChanged();
}

void AbstractVideoWidget::removeOsd(const QString &name)
{
    QMutexLocker locker(&mutex);
    int count = listOsd.count();
    for (int i = 0; i < count; ++i) {
        if (listOsd.at(i).name == name) {
            listOsd.removeAt(i);
            break;
        }
    }

    this->update();
    emit sig_osdChanged();
}

void AbstractVideoWidget::clearOsd()
{
    QMutexLocker locker(&mutex);
    listOsd.clear();
    this->update();
    emit sig_osdChanged();
}

void AbstractVideoWidget::setGraph(const GraphInfo &graph)
{
    //存在则替换
    bool exist = false;
    int count = listGraph.count();
    for (int i = 0; i < count; ++i) {
        GraphInfo graphInfo = listGraph.at(i);
        if (graphInfo.name == graph.name) {
            exist = true;
            listGraph[i] = graph;
            emit sig_graphChanged();
            return;
        }
    }

    //不存在则插入
    if (!exist) {
        this->appendGraph(graph);
    }
}

void AbstractVideoWidget::appendGraph(const GraphInfo &graph)
{
    QMutexLocker locker(&mutex);
    listGraph << graph;
    this->update();
    emit sig_graphChanged();
}

void AbstractVideoWidget::removeGraph(const QString &name)
{
    QMutexLocker locker(&mutex);
    int count = listGraph.count();
    for (int i = 0; i < count; ++i) {
        if (listGraph.at(i).name == name) {
            listGraph.removeAt(i);
            break;
        }
    }

    this->update();
    emit sig_graphChanged();
}

void AbstractVideoWidget::clearGraph()
{
    QMutexLocker locker(&mutex);
    listGraph.clear();
    this->update();
    emit sig_graphChanged();
}

void AbstractVideoWidget::setImage(const QImage &image)
{
    if (!image.isNull()) {
        //是否需要触发尺寸变化
        bool sizeChanged = (image.width() != videoWidth || image.height() != videoHeight);
        if (sizeChanged) {
            videoWidth = image.width();
            videoHeight = image.height();
            this->receiveSizeChanged();
        }

        receiveImage(image, 0);
    }
}

void AbstractVideoWidget::clear()
{
    videoWidth = 0;
    videoHeight = 0;
    listOsd.clear();
    listGraph.clear();

    //绘制一张空白图片
    this->image = QImage();
    this->update();
}

void AbstractVideoWidget::setPalettex(bool enabled)
{
    //和大多数播放器一样填充背景颜色(建议用黑色填充)
    if (widgetPara.scaleMode != ScaleMode_Fill && widgetPara.fillColor != Qt::transparent) {
        this->setAutoFillBackground(enabled);
        if (enabled) {
            QPalette palette = this->palette();
            palette.setBrush(QPalette::Window, widgetPara.fillColor);
            this->setPalette(palette);
        }
    }
}

void AbstractVideoWidget::receivePlayStart(int time)
{
    //播放开始的时候调整一次尺寸
    this->resizeEvent(NULL);
    this->update();
    this->setPalettex(true);

#ifdef openglx
    if (widgetPara.videoMode == VideoMode_Opengl) {
        if (widgetPara.osdDrawMode == DrawMode_Cover) {
            timerUpdate->start();
        }

        rgbWidget->setVisible(false);
        yuvWidget->setVisible(false);
        nv12Widget->setVisible(false);

        if (hardware == "rgb") {
            rgbWidget->setVisible(true);
        } else if (hardware == "none") {
            yuvWidget->setVisible(true);
        } else {
            nv12Widget->setVisible(true);
        }

        this->clearData();
    }
#endif
}

void AbstractVideoWidget::receivePlayFinsh()
{
    //分辨率复位下绘制那边需要用到这个值
    videoWidth = 0;
    videoHeight = 0;
    this->clear();
    this->setPalettex(false);
    bannerWidget->setVisible(false);

#ifdef openglx
    if (widgetPara.videoMode == VideoMode_Opengl) {
        if (widgetPara.osdDrawMode == DrawMode_Cover) {
            timerUpdate->stop();
        }

        this->clearData();
        rgbWidget->setVisible(false);
        yuvWidget->setVisible(false);
        nv12Widget->setVisible(false);
    }
#endif
}

void AbstractVideoWidget::receiveImage(const QImage &image, int time)
{
    //qDebug() << TIMEMS << sender() << image.size() << time;
    this->image = widgetPara.copyImage ? image.copy() : image;
    this->update();
}

void AbstractVideoWidget::snapImage(const QImage &image, const QString &snapName)
{
    if (preview && !image.isNull()) {
        preview = false;
        label = WidgetHelper::showImage(label, this, image);
    }
}

void AbstractVideoWidget::receiveSizeChanged()
{
    QResizeEvent *event = new QResizeEvent(size(), size());
    QApplication::postEvent(this, event);
}

void AbstractVideoWidget::receiveFrame(int width, int height, quint8 *dataRGB, int type)
{
#ifdef openglx
    //这里要过滤下可能线程刚好结束了但是信号已经到这里
    if (sender()) {
        rgbWidget->updateFrame(width, height, dataRGB, type);
    }
#endif
}

void AbstractVideoWidget::receiveFrame(int width, int height, quint8 *dataY, quint8 *dataU, quint8 *dataV, quint32 linesizeY, quint32 linesizeU, quint32 linesizeV)
{
#ifdef openglx
    //这里要过滤下可能线程刚好结束了但是信号已经到这里
    if (sender()) {
        yuvWidget->updateFrame(width, height, dataY, dataU, dataV, linesizeY, linesizeU, linesizeV);
    }
#endif
}

void AbstractVideoWidget::receiveFrame(int width, int height, quint8 *dataY, quint8 *dataUV, quint32 linesizeY, quint32 linesizeUV)
{
#ifdef openglx
    //这里要过滤下可能线程刚好结束了但是信号已经到这里
    if (sender()) {
        nv12Widget->updateFrame(width, height, dataY, dataUV, linesizeY, linesizeUV);
    }
#endif
}
