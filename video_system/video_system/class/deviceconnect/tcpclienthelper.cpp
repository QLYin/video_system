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
		else if (cmd == CommandNS::kCmdDevNetSet)
		{
			data += "dev_id : ";
			data += param["dev_id"].toInt();
			data += kSplitStr;
			data += "ipaddr : ";
			data += param["ipaddr"].toString();
			data += kSplitStr;
			data += "netmask : ";
			data += param["netmask"].toString();
			data += kSplitStr;
			data += "gateway : ";
			data += param["gateway"].toString();
			data += kSplitStr;
			data += "mac : ";
			data += param["mac"].toString();
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
	else if (cmd == CommandNS::kCmdIPCAutoAdd)
	{
		data += "num : ";
		data += param["num"].toInt();
		data += kSplitStr;

		QList<IpcInfo> ipcList;
		QVariant variant = param["ipcList"];
		ipcList = variant.value<QList<IpcInfo>>();

		for (auto& ipc : ipcList)
		{
			data += "id : ";
			data += QString::number(ipc.id);
			data += kSplitStr;

			data += "ipaddr : ";
			data += ipc.ipaddr;
			data += kSplitStr;

			data += "user : ";
			data += ipc.user;
			data += kSplitStr;

			data += "passwd : ";
			data += ipc.passwd;
			data += kSplitStr;

			data += "name : ";
			data += ipc.name;
			data += kSplitStr;

			data += "url0 : ";
			data += ipc.rtsp_url0;
			data += kSplitStr;

			data += "resolution0 : ";
			data += QString::number(ipc.resolution0);;
			data += kSplitStr;

			data += "url1 : ";
			data += ipc.rtsp_url1;
			data += kSplitStr;

			data += "resolution1 : ";
			data += QString::number(ipc.resolution1);;
			data += kSplitStr;

			data += "ptzState : ";
			data += "0";
			data += kSplitStr;

			data += "ptzAddr : ";
			data += "";
			data += kSplitStr;

			data += "profiletoken : ";
			data += "";
			data += kSplitStr;

			data += "netxaddr : ";
			data += "";
			data += kSplitStr;
		}
	}
	else if (cmd == CommandNS::kCmdIPCDel)
	{
		auto keys = param.keys();
		for (auto& key : keys)
		{
			data += key;
			data += " : ";
			data += param[key].toString();
			data += kSplitStr;
		}
	}
	else if (cmd == CommandNS::kCmdDIPCModify )
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

void TcpClientHelper::sendDataSync(int type)
{
	QString data = kCmdStr + "DataSync";
	data += kSplitStr;
	data += "type : ";
	data += QString::number(type);
	data += kSplitStr;

	TcpClient::Instance()->sendData(data);
}

void TcpClientHelper::sendCmd(const QString& cmd, QVariantMap param)
{
	if (cmd.isEmpty())
	{
		return;
	}

	if (param.isEmpty())
	{
		QString data;
		data = kCmdStr + cmd;
		data += kSplitStr;
		TcpClient::Instance()->sendData(data);
	}
	else
	{
		if (cmd == CommandNS::kCmdDataSync)
		{
			int type = param["type"].toInt();
			sendDataSync(type);

		} else if (cmd == CommandNS::kCmdDevSearch ||
				 cmd == CommandNS::kCmdDevAdd ||
				 cmd == CommandNS::kCmdDevDel ||
				 cmd == CommandNS::kCmdDevModify ||
			     cmd == CommandNS::kCmdDevNetSet ||
				 cmd == CommandNS::kCmdDevNumSet) //解码卡相关命令
		{
			sendDevCmd(cmd, param);

		} else if (cmd == CommandNS::kCmdIPCSearch ||
				 cmd == CommandNS::kCmdIPCDefaultUserPwd ||
				 cmd == CommandNS::kCmdIPCAdd ||
				 cmd == CommandNS::kCmdIPCAutoAdd ||
				 cmd == CommandNS::kCmdDIPCModify ||
				 cmd == CommandNS::kCmdDIPCClear ||
				 cmd == CommandNS::kCmdIPCDel) // ip相关命令
		{
			sendIPCCmd(cmd, param);

		} else if (cmd == CommandNS::kCmdDWallCutScreen ||
			cmd == CommandNS::kCmdDWallCallVideo ||
			cmd == CommandNS::kCmdDWallCloseVideo ||
			cmd == CommandNS::kCmdDWallJoint ||
			cmd == CommandNS::kCmdDWallJointSet ||
			cmd == CommandNS::kCmdDWallJointExit) //电视墙相关命令
		{
			sendWallCmd(cmd, param);
		}
	}
}