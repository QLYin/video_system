#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "singleton.h"

#include "../deviceconnect/cmdhandlermgr.h"
#include "../deviceconnect/tcpcmddef.h"
//#include "../frmtvwall/frmscreen.h"

class frmSceneWidget;
class SceneManager : public QObject, public IHandler
{
    Q_OBJECT SINGLETON_DECL(SceneManager)

public:
    explicit SceneManager(QObject *parent = 0);
    void initSceneWidget(frmSceneWidget* widget);
    void handle(const QVariantMap& data) override;

private:
    int calculateLoopScene(const QVector<int>& arr);
private slots:
    void onSceneAdd(int id, QString name);
    void onSceneDelete(int id);
    void onSceneCall(int id);
    void onSceneLoop(bool loop, QVector<int> ids);

private:
    frmSceneWidget* m_sceneWidget = nullptr;
};

#endif // SCENEMANAGER_H
