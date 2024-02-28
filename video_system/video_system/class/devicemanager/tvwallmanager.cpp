#include "tvwallmanager.h"
#include <QMap>
#include <QMetaType>

#include"devmanager.h"
#include "ipcmanager.h"
#include "../frmtvwall/frmtvwallwidget.h"

SINGLETON_IMPL(TVWallManager)
TVWallManager::TVWallManager(QObject *parent) : QObject(parent)
{
	CmdHandlerMgr::Instance()->registHandler(this);
}

void TVWallManager::handle(const QVariantMap& data)
{
	auto cmd = data["cmd"].toString();
	if (cmd == CommandNS::kCmdWallSet)
	{
		int row = data["row"].toInt();
		int col = data["col"].toInt();
		row = row > 0 ? row : 3;
		col = col > 0 ? col : 3;
		if (m_wallWidget && row > 0 && col > 0)
		{
			m_wallWidget->createTVWall(row, col);
		}
	}
	else if (cmd == CommandNS::kCmdSuccessR)
	{
		int id = data["device_id"].toInt();
		auto screen = findScreen(id);
		if (screen)
		{
			if (screen->hasCut())
			{
				for (int i = 0; i < 16; ++i)
				{
					auto key = QString("chn_%1").arg(i);
					auto ipcId = data[key].toInt();
					QString ip = IPCManager::Instance()->findIp(ipcId);
					if (!ip.isEmpty())
					{
						screen->setCellText(i, ip);
					}
				}
			}
			else {
				auto ipcId = data["chn_0"].toInt();
				QString ip = IPCManager::Instance()->findIp(ipcId);
				if (!ip.isEmpty())
				{
					screen->setText(ip);
				}
			}
		
		}
	}
}

bool TVWallManager::hasMergeScreen()
{
	return m_wallWidget->hasMergeScreen();
}

void TVWallManager::initWallWidget(frmTVWallWidget* widget)
{
	m_wallWidget = widget;
	connect(m_wallWidget, &frmTVWallWidget::wallSetSig, this, &TVWallManager::onWallSet);
	connect(m_wallWidget, &frmTVWallWidget::wallScreenJoinSig, this, &TVWallManager::onWallScreenJoin);
	connect(m_wallWidget, &frmTVWallWidget::wallScreenCutSig, this, &TVWallManager::onWallScreenCut);
	connect(m_wallWidget, &frmTVWallWidget::wallCallVideoSig, this, &TVWallManager::onWallCallVideo);
}

void TVWallManager::sendWallJoint(int row, int col)
{
	QVariantMap param;
	param["row"] = row;
	param["col"] = col;
	param["sig_src"] = 0;
	param["protocol"] = 1;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdWallJointSet, param);
}

void TVWallManager::onWallSet()
{
	auto devInfo = DevManager::Instance()->devListInfo();
	int devSize = devInfo.size();
	if (devSize > 0)
	{
		int wallWidgetRow = m_wallWidget->rows();
		int wallWidgetCol = m_wallWidget->cols();
		for (int r = 0; r < wallWidgetRow; ++r)
		{
			for (int c = 0; c < wallWidgetCol; ++c)
			{
				int devIndex = r * wallWidgetCol + c;
				if (devIndex < devSize)
				{
					auto screen = m_wallWidget->findScreen(r, c);
					if (screen)
					{
						screen->setEnableDrop(true);
						int chnCnt = devInfo.at(devIndex).chn_cnt;
						if (chnCnt > 1)
						{
							screen->cutScreen(chnCnt, true, false);  // 6分屏要单独处理
							auto ipcIndexs = devInfo.at(devIndex).ipc_indexs;
							for (int j = 0; j < 16; ++j)
							{
								auto id = ipcIndexs.at(j);
								QString ip = IPCManager::Instance()->findIp(id);
								if (!ip.isEmpty())
								{
									screen->setCellText(j, ip);
								}
							}
						}
						else
						{
							QString ip = IPCManager::Instance()->findIp(devInfo.at(devIndex).ipc_indexs.at(0));
							screen->setText(ip);
							QFont font("Arial", 8);
							screen->setFont(font);
						}
					}
				}
				else
				{
					auto screen = m_wallWidget->findScreen(r, c);
					if (screen)
					{
						screen->setEnableDrop(false);
						screen->setText("未检测到解码卡");
					}

				}
			}
		}
	}
}

int TVWallManager::calculatePlanMode(const QVector<int>& arr)
{
	int result = 0;
	for (int num : arr) {
		result |= (1 << num);
	}
	return result;
}

void TVWallManager::onWallScreenJoin(QVector<ScreenInfo> infos, QVector<int> indexs, bool join)
{
	if (indexs.isEmpty()) return;
	int firstId = indexs.at(0) - 1;
	int planMode = calculatePlanMode(indexs);

	QVariantMap param;
	param["plan_mode"] = planMode;
	param["src_dev"] = firstId;
	CmdHandlerMgr::Instance()->sendCmd(join ? CommandNS::kCmdWallJoint : CommandNS::kCmdWallJointExit, param);

	auto devInfo = DevManager::Instance()->devListInfo();
	if (join) 
	{
		int firstX = infos.at(0).x;
		int firstY = infos.at(0).y;
		int devIndex = firstX * m_wallWidget->cols() + firstY;
		QString ip = IPCManager::Instance()->findIp(devInfo.at(devIndex).ipc_indexs.at(0));
		auto screen = m_wallWidget->findScreen(firstX, firstY);
		qDebug() << "find screen: " << screen;
		if (screen && !ip.isEmpty())
		{
			screen->setText(ip);
			QFont font("Arial", 8);
			screen->setFont(font);
		}
	}
	else
	{
		for (auto& info : infos)
		{
			int x = info.x;
			int y = info.y;
			int row = info.row;
			int col = info.col;
			int devIndex = x * m_wallWidget->cols() + y;

			auto screen = m_wallWidget->findScreen(x, y);

			if (row > 1 || col > 1)
			{
				screen->cutScreen(row * col, true, false); // 6分屏要单独处理
				auto ipcIndexs = devInfo.at(devIndex).ipc_indexs;
				for (int j = 0; j < 16; ++j)
				{
					auto id = ipcIndexs.at(j);
					QString ip = IPCManager::Instance()->findIp(id);
					if (!ip.isEmpty())
					{
						screen->setCellText(j, ip);
					}
				}
			}
			else 
			{
				QString ip = IPCManager::Instance()->findIp(devInfo.at(devIndex).ipc_indexs.at(0));
				if (screen && !ip.isEmpty())
				{
					screen->setText(ip);
					QFont font("Arial", 8);
					screen->setFont(font);
				}

			}
		}

	}
}

void TVWallManager::onWallScreenCut(int row, int col, int splitNum)
{
	int index = row * m_wallWidget->cols() + col;
	auto devInfo = DevManager::Instance()->devListInfo();
	QVariantMap param;
	param["split_num"] = splitNum;
	param["device_id"] = devInfo.at(index).dev_id;
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdWallCutScreen, param);

	auto screen = m_wallWidget->findScreen(row, col);
	if (screen)
	{
		if (screen->cutRow() > 1 || screen->cutCol() > 1)
		{
			auto ipcIndexs = devInfo.at(index).ipc_indexs;
			for (int j = 0; j < 16; ++j)
			{
				auto id = ipcIndexs.at(j);
				QString ip = IPCManager::Instance()->findIp(id);
				if (!ip.isEmpty())
				{
					screen->setCellText(j, ip);
				}
			}
		}
		else
		{
			QString ip = IPCManager::Instance()->findIp(devInfo.at(index).ipc_indexs.at(0));
			screen->setText(ip);
			QFont font("Arial", 8);
			screen->setFont(font);
		}
		
	}
}

frmScreen* TVWallManager::findScreen(int devId)
{
	auto devInfo = DevManager::Instance()->devListInfo();
	int devSize = devInfo.size();
	if (devSize > 0)
	{
		for (int i = 0; i < devSize; ++i)
		{
			if (devInfo.at(i).dev_id == devId)
			{
				int r = i / m_wallWidget->cols();
				int c = i % m_wallWidget->cols();
				return m_wallWidget->findScreen(r, c);
			}
		}
	}
	return nullptr;
}

void TVWallManager::onWallCallVideo(int row, int col, int chn_index, QString ip)
{
	QVariantMap param;
	param["chn"] = chn_index;
	param["id"] = IPCManager::Instance()->findId(ip);
	CmdHandlerMgr::Instance()->sendCmd(CommandNS::kCmdWallCallVideo, param);
	// 要更新devinfo
	int devIndex = row * m_wallWidget->cols() + col;
	auto& devInfo = DevManager::Instance()->devListInfo();
	QVector<int>& ipcIndexs = devInfo[devIndex].ipc_indexs;
	auto screen = m_wallWidget->findScreen(row, col);
	if (screen->index() == chn_index)
	{
		ipcIndexs[0] = IPCManager::Instance()->findId(ip);
	}
	else 
	{
		auto index = chn_index - screen->index();
		if (index > 0)
		{
			ipcIndexs[index] = IPCManager::Instance()->findId(ip);
		}
	}


}