#include "scenemanager.h"
#include <QMap>
#include <QMetaType>

#include "../frmtvwall/frmscenewidget.h"
#include "class/appmisc/appmisc.h"

SINGLETON_IMPL(SceneManager)
SceneManager::SceneManager(QObject *parent) : QObject(parent)
{
	CmdHandlerMgr::Instance()->registHandler(this);
}

void SceneManager::handle(const QVariantMap& data)
{
	auto cmd = data["cmd"].toString();
	if (cmd == CommandNS::kCmdSceneInfo)
	{
		QVector<QVariantMap> vecData;
		QVariant variant = data["cmdDataArrary"];
		vecData = variant.value<QVector<QVariantMap>>();
		QMap<QString, int> sceneList;
		for (auto& item : vecData)
		{
			if (!item.isEmpty())
			{
				sceneList.insert(item["name"].toString(), item["scene_id"].toInt());
			}
		}

		if (!sceneList.isEmpty())
		{
			m_sceneWidget->initScenes(sceneList);
		}
	}
}

void SceneManager::initSceneWidget(frmSceneWidget* widget)
{
	m_sceneWidget = widget;
	connect(m_sceneWidget, &frmSceneWidget::sceneAddSig, this, &SceneManager::onSceneAdd);
	connect(m_sceneWidget, &frmSceneWidget::sceneDelSig, this, &SceneManager::onSceneDelete);
	connect(m_sceneWidget, &frmSceneWidget::sceneCallSig, this, &SceneManager::onSceneCall);
	connect(m_sceneWidget, &frmSceneWidget::sceneLoop, this, &SceneManager::onSceneLoop);
}

void SceneManager::onSceneAdd(int id, QString name)
{
	QVariantMap param;
	param["scene_id"] = id;
	param["name"] = name;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdSceneRecord, param);
}

void SceneManager::onSceneDelete(int id)
{
	QVariantMap param;
	param["scene_id"] = id;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdSceneDel, param);
}

void SceneManager::onSceneCall(int id)
{
	QVariantMap param;
	param["scene_id"] = id;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdSceneCall, param);

	param.clear();
	param["type"] = 4;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
}

int SceneManager::calculateLoopScene(const QVector<int>& arr)
{
	int result = 0;
	for (int num : arr) {
		result |= (1 << (num - 1));
	}
	return result;
}

void SceneManager::onSceneLoop(bool loop, QVector<int> ids)
{
	QVariantMap param;
	if (loop)
	{
		param["loop_scene"] = calculateLoopScene(ids);
		param["time"] = 60;
		param["reboot_time"] = 0;
		CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdSceneLoop, param);
	}
	else
	{
		CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdSceneLoopStop, param);

		param.clear();
		param["type"] = 4;
		CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdDataSync, param);
	}
}