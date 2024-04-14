#include "frmscenewidget.h"

#include <QPainter>
#include <QtWidgets>
#include <QGridLayout>
#include <QScrollArea>

frmSceneWidget::frmSceneWidget(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);

    // 创建 SceneContainer
    QWidget* SceneContainer = new QWidget(this);

    // // 创建带有水平滚动条的 QScrollArea 控件
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidget(SceneContainer);
    scrollArea->horizontalScrollBar()->setFixedHeight(2);

    m_gridLayout = new QGridLayout(SceneContainer);
    // 添加 SceneContainer 的子元素
    m_gridLayout->setAlignment(Qt::AlignLeft);
    m_gridLayout->setContentsMargins(2, 0 ,2 ,0);
    m_gridLayout->setSpacing(6);

    // 创建 btnAdd
    m_btnAdd = new QPushButton(this);
    m_btnAdd->setFixedSize(64, 64);
    m_btnAdd->setIcon(QIcon(":/image/add-56.png"));
    connect(m_btnAdd, &QPushButton::clicked, this, &frmSceneWidget::btnAddClicked);

    // 创建 btnLoop
    m_btnLoop = new QPushButton(this);
    m_btnLoop->setFixedSize(64, 64);
    m_btnLoop->setCheckable(true);
    m_btnLoop->setChecked(false);
    m_btnLoop->setIcon(m_btnLoop->isChecked() ? QIcon(":/image/loop-56.png") : QIcon(":/image/loopnone-56.png"));
    connect(m_btnLoop, &QPushButton::clicked, this, &frmSceneWidget::btnLoopClicked);

    // 将子元素添加到布局中
    layout->addWidget(scrollArea);
    layout->addWidget(m_btnAdd);
    layout->addWidget(m_btnLoop);

    // 设置布局间隔和对齐方式（可根据需要调整）
    layout->setSpacing(3);
    layout->setMargin(4);
    layout->setAlignment(Qt::AlignLeft);

    setStyleSheet("background-color : rgb(14, 26, 50);");
}

frmSceneWidget::~frmSceneWidget()
{

}

void frmSceneWidget::btnAddClicked()
{
    int count = m_sceneList.size();
    QString name = getNewSceneName();
    int id = getNewSceneId();
    frmScene* item = new frmScene(name, id, this);
    connect(item, &frmScene::sceneDelete, this, [this](frmScene* item)
        {
            qDebug()  << "[frmSceneWidget] delete scene  id: " << item->sceneId() << ", name: " << item->sceneName();
            emit sceneDelSig(item->sceneId());
            item->setParent(nullptr);
            m_sceneList.removeOne(item);
            if (item)
            {
                item->deleteLater();
            }
        });

    connect(item, &frmScene::sceneCall, this, &frmSceneWidget::sceneCallSig);

    item->setFixedSize(48, 56);
    m_gridLayout->addWidget(item, 0, count, 1, 1);
    m_sceneList.append(item);
    qSort(m_sceneList.begin(), m_sceneList.end(), [](frmScene* a, frmScene* b)
        {
            return a->sceneId() < b->sceneId();
        });
    qDebug() << "[frmSceneWidget]  add scene  id: " << item->sceneId() << ", name: " << item->sceneName();
    emit sceneAddSig(id, name);
}

void frmSceneWidget::btnLoopClicked()
{
    m_btnLoop->setIcon(m_btnLoop->isChecked() ? QIcon(":/image/loop-56.png") : QIcon(":/image/loopnone-56.png"));
    QVector<int> ids;
    if (m_btnLoop->isChecked())
    {
        for (auto& scene : m_sceneList)
        {
            ids.push_back(scene->sceneId());
        }

        if (ids.isEmpty())
        {
            qSort(ids.begin(), ids.end());
        }
    }
    emit sceneLoop(m_btnLoop->isChecked(), ids);
}

QString frmSceneWidget::getNewSceneName()
{
    int count = m_sceneList.size();
    if (count == 0)
    {
        return QString::fromLocal8Bit("场景1");
    }
    else
    {
        auto lastItemLayout = m_gridLayout->itemAtPosition(0, count - 1);
        frmScene* lastItem = qobject_cast<frmScene*>(lastItemLayout->widget());
        QRegularExpression re("\\d+"); 
        QRegularExpressionMatch match = re.match(lastItem->sceneName());
        if (match.hasMatch()) {
            QString extractedNumber = match.captured(0); 
            int num = extractedNumber.toInt() + 1;
            return QString::fromLocal8Bit("场景%1").arg(num);
        }
        else {
            return QString::fromLocal8Bit("场景X");
        }
    }
}

int frmSceneWidget::getNewSceneId()
{
    int count = m_sceneList.size();
    if (count == 0)
    {
        return 0;
    }
    else
    {
        QSet<int> ids;
        for (auto& scene : m_sceneList)
        {
            ids.insert(scene->sceneId());
        }

        for (int i = 0; i < count; ++i)
        {
            if (!ids.contains(i))
            {
                return i;
            }
        }

        return count;
    }
}

void frmSceneWidget::initScenes(QMap<QString, int> sceneList)
{
    if (!m_gridLayout)
    {
        return;
    }

    if (sceneList.isEmpty())
    {
        return;
    }

    if (!m_sceneList.isEmpty())
    {
        for (auto& item : m_sceneList)
        {
            item->setParent(nullptr);
            if (item)
            {
                delete item;
            }
        }
        m_sceneList.clear();
    }

    int count = 0;
    for (auto it = sceneList.begin(); it != sceneList.end(); ++it)
    {
        QString name = it.key();
        int id = it.value();
        if (!name.isEmpty() && id >= 0)
        {
            frmScene* item = new frmScene(it.key(), it.value(), this);
            connect(item, &frmScene::sceneDelete, this, [this](frmScene* item)
                {
                    qDebug() << "[frmSceneWidget] delete scene  id: " << item->sceneId() << ", name: " << item->sceneName();
                    emit sceneDelSig(item->sceneId());
                    item->setParent(nullptr);
                    m_sceneList.removeOne(item);
                    if (item)
                    {
                        item->deleteLater();
                    }
                });
            connect(item, &frmScene::sceneCall, this, &frmSceneWidget::sceneCallSig);
            item->setFixedSize(48, 56);
            m_gridLayout->addWidget(item, 0, count, 1, 1);
            count++;
            m_sceneList.append(item);
            qDebug() << "[frmSceneWidget]init scene  id: " << item->sceneId() << ", name: " << item->sceneName();
        }
    }

    qSort(m_sceneList.begin(), m_sceneList.end(), [](frmScene* a, frmScene* b)
        {
            return a->sceneId() < b->sceneId();
        });
}