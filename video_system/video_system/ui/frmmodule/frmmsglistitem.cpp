#include "frmmsglistitem.h"
#include "ui_frmmsglistitem.h"
#include "qthelper.h"

frmMsgListItem::frmMsgListItem(QWidget *parent) : QWidget(parent), ui(new Ui::frmMsgListItem)
{
    ui->setupUi(this);
    this->initForm();
}

frmMsgListItem::~frmMsgListItem()
{
    delete ui;
}

bool frmMsgListItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this) {
        //双击弹出详细画面
        if (event->type() == QEvent::MouseButtonDblClick) {
            if (!image.isNull()) {
                //自适应图片分辨率并限制最大分辨率
                QSize size = image.size();
                if (size.width() > 1280 || size.height() > 720) {
                    size.scale(1280, 720, Qt::KeepAspectRatio);
                    image = image.scaled(size);
                }

                labImage.resize(image.size());
                labImage.setPixmap(QPixmap::fromImage(image));
                labImage.show();
            }
        }
    } else if (watched == &labImage) {
        if (event->type() == QEvent::Show) {
            QtHelper::setFormInCenter(&labImage);
        }
    }

    return QWidget::eventFilter(watched, event);
}

void frmMsgListItem::initForm()
{
    //弹出图片用标签
    labImage.setWindowTitle("告警图片");
    labImage.setAlignment(Qt::AlignCenter);
    labImage.setWindowFlags(labImage.windowFlags() | Qt::WindowStaysOnTopHint);
    labImage.installEventFilter(this);
    this->installEventFilter(this);

    ui->line->setFixedHeight(1);

    //设置字号+字体加粗
    QFont font;
    font.setPixelSize(GlobalConfig::FontSize + 2);
    font.setBold(true);
    ui->labMsgTime->setFont(font);
    font.setBold(false);
    ui->labMsgContent->setFont(font);
    ui->labMsgResult->setFont(font);

    //可以自行调整最小高度
    ui->labMsgImage->setFixedHeight(60);
}

void frmMsgListItem::setMsg(const QString &msg, const QString &result, const QImage &image, const QString &time)
{
    ui->labMsgTime->setText(time);
    ui->labMsgContent->setText(msg);
    ui->labMsgResult->setText(result);

    //不同处理结果不同颜色
    if (result.contains("待处理")) {
        ui->labMsgResult->setStyleSheet(QString("color:%1;").arg(AppConfig::ColorMsgWait));
    } else if (result.contains("已处理")) {
        ui->labMsgResult->setStyleSheet(QString("color:%1;").arg(AppConfig::ColorMsgSolved));
    }

    //等比例拉伸或者填充图像
    if (!image.isNull()) {
        this->image = image;
        QPixmap pixmap = QPixmap::fromImage(image);
        pixmap = pixmap.scaled(ui->labMsgImage->size(), Qt::IgnoreAspectRatio);
        ui->labMsgImage->setPixmap(pixmap);
    }
}
