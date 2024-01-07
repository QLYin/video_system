#include "tcpclienthelper.h"

namespace {
	const QString kCmdStr = "cmd : ";
	const QString kSplitStr = "\r\n";
};

void TcpClientHelper::sendUnlockDevice()
{
	QString data = kCmdStr + "UnlockDevice";
	data += kSplitStr;

	TcpClient::Instance()->sendData(data);
}

void TcpClientHelper::sendDataSync(int type)
{
	QString data = kCmdStr + "DataSync";
	data += kSplitStr;
	data += "type : ";
	data += QString::number(type);
	data += kSplitStr;

	TcpClient::Instance()->sendData(data);
}

void TcpClientHelper::sendSceneInfo()
{
	QString data = kCmdStr + "SceneInfo";
	data += kSplitStr;

	TcpClient::Instance()->sendData(data);
}

void TcpClientHelper::sendWallSet()
{
	QString data = kCmdStr + "WallSet";
	data += kSplitStr;

	TcpClient::Instance()->sendData(data);
}
