#include "tcpclienthelper.h"
#include "tcpcmddef.h"

namespace {
	const QString kCmdStr = "cmd : ";
	const QString kSplitStr = "\r\n";
};

// 解码卡相关指令发送
void TcpClientHelper::sendDevCmd(const QString& cmd, const QVariantMap& param)
{
	QString data = kCmdStr + cmd;
	data += kSplitStr;
	if (!param.isEmpty())
	{
		if (cmd == CommandNS::kCmdDevNumSet)
		{
			data += "dev_id : ";
			data += param["dev_id"].toInt();
			data += kSplitStr;
		}
		else
		{

			data += "dev_id : ";
			data += param["dev_id"].toInt();
			data += kSplitStr;
			data += "ipaddr : ";
			data += param["ipaddr"].toString();
			data += kSplitStr;
			data += "chn_cnt : ";
			data += param["chn_cnt"].toInt();
			data += kSplitStr;
			data += "floor : ";
			data += param["floor"].toInt();
			data += kSplitStr;
			data += "upper : ";
			data += param["upper"].toInt();
			data += kSplitStr;
			data += "width : ";
			data += param["width"].toInt();
			data += kSplitStr;
			data += "heigth : ";
			data += param["heigth"].toInt();
			data += kSplitStr;
			data += "fresh_freq : ";
			data += param["fresh_freq"].toInt();
			data += kSplitStr;
			data += "separate : ";
			data += kSplitStr;
		}
	}
	TcpClient::Instance()->sendData(data);
}

// ipc相关指令发送
void TcpClientHelper::sendIPCCmd(const QString& cmd, const QVariantMap& param)
{
	QString data = kCmdStr + cmd;
	data += kSplitStr;
	if (cmd == CommandNS::kCmdDevNumSet)
	{
		data += "dev_id : ";
		data += param["dev_id"].toInt();
		data += kSplitStr;
	}
	else if (cmd == CommandNS::kCmdIPCAutoAdd || cmd == CommandNS::kCmdDIPCModify)
	{
	}
	else if (cmd == CommandNS::kCmdIPCAdd || cmd == CommandNS::kCmdIPCDel)
	{
	}
	else if (cmd == CommandNS::kCmdIPCAdd || cmd == CommandNS::kCmdIPCDel)
	{
	}


	TcpClient::Instance()->sendData(data);
}

void TcpClientHelper::sendWallCmd(const QString& cmd, const QVariantMap& param)
{
	QString data = kCmdStr + cmd;
	data += kSplitStr;
	if (cmd == CommandNS::kCmdDWallJoint)
	{
		data += "row : ";
		data += param["row"].toInt();
		data += kSplitStr;
		data += "col : ";
		data += param["col"].toInt();
		data += kSplitStr;
		data += "sig_src : ";
		data += param["sig_src"].toInt();
		data += kSplitStr;
		data += "protocol : ";
		data += param["protocol"].toInt();
		data += kSplitStr;
	}

	TcpClient::Instance()->sendData(data);
}

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
