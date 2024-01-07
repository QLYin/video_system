#include "frmtvwall.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QDrag>
#include <QMimeData>

#include "frmtvwallwidget.h"

struct Camera {
    int id;
    QString name;
    QString rtsp_url;
    QString ipaddr;
};

frmTVWall::frmTVWall(QWidget* parent) : QWidget(parent)
{
    this->initForm();
}

frmTVWall::~frmTVWall()
{
}

void frmTVWall::initForm()
{
    Camera item1{1, "test", "rtsp://192.168.1.220:554/11","192.168.1.220"};
    Camera item2{ 2, "haikang", "rtsp://192.168.1.220:554/11","192.168.1.220" };
    QVector<Camera> CameraList;
    CameraList.push_back(item1);
    CameraList.push_back(item2);

    QWidget* pGroup1 = new QWidget;

    QTreeWidget* treeWidget = new QTreeWidget(pGroup1);
    treeWidget->setDragEnabled(true);
    connect(treeWidget, &QTreeWidget::itemPressed, [&](QTreeWidgetItem* item, int column) 
        {
            QDrag* drag = new QDrag(treeWidget);
            QMimeData* mimeData = new QMimeData;
            mimeData->setText(item->text(column));
            drag->setMimeData(mimeData);
            drag->exec();
        });
    QTreeWidgetItem* root = new QTreeWidgetItem(treeWidget);
    root->setText(0, "Root");

    for (auto& item : CameraList) {
        QTreeWidgetItem* child = new QTreeWidgetItem(root);
        child->setText(0, item.name);
    }

    QVBoxLayout* pLayout1 = new QVBoxLayout(pGroup1);
    pLayout1->setMargin(10);
    pLayout1->setAlignment(Qt::AlignmentFlag::AlignLeft);
    pLayout1->addWidget(treeWidget);

    QWidget* pGroup2 = new QWidget;

    QTabWidget* pTab = new QTabWidget(this);
    pTab->setFixedWidth(180);
    pTab->addTab(pGroup1, "ipc");
    pTab->addTab(pGroup2, "telephone");

    frmTVWallWidget* tvWallWidget = new frmTVWallWidget(this);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    setLayout(mainLayout);
    mainLayout->addWidget(pTab);
    mainLayout->addWidget(tvWallWidget);
}
