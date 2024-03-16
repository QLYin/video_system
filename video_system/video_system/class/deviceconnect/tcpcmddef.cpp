#include "tcpcmddef.h"

namespace CommandNS {
	// 解码卡
	const QString kCmdDevSearch = "SysSetSearch"; // 搜索从卡
	const QString kCmdDevAdd = "SysSetAddDev"; // 添加解码卡
	const QString kCmdDevDel = "SysSetDelDev"; // 删除解码卡
	const QString kCmdDevModify = "SysSetModifyDev"; // 修改解码卡参数
	const QString kCmdDevNumSet = "SysSetModifyDevNum"; //设置从卡个数
	const QString kCmdDevNetSet = "NetSet"; //设置ip

	// ipc
	const QString kCmdIPCSearch = "SearchIpc"; // 搜索IPC
	const QString kCmdIPCDefaultUserPwd = "SetDefaultUserPasswdIpc"; //设置默认账号密码
	const QString kCmdIPCAdd = "AddIpc"; // 添加ipc
	const QString kCmdIPCAutoAdd = "AutoAddIpc2";
	const QString kCmdIPCDel = "DelIpc"; // 删除IPC
	const QString kCmdDIPCModify = "EditIpc"; // 编辑ipc
	const QString kCmdDIPCClear = "ClearIpc"; // 清空ipc

	// 电视墙
	const QString kCmdWallCutScreen = "CutScreen";  //切屏
	const QString kCmdWallCallVideo = "CallVedio";  //调入视频
	const QString kCmdWallCloseVideo = "CloseVedio"; // 关闭视频
	const QString kCmdWallJoint = "Joint";    // 拼接
	const QString kCmdWallJointExit = "JointExit"; //退出拼接
	const QString kCmdWallJointSet = "JointSet"; // 设置幕墙
	const QString kCmdWallSet = "WallSet"; // 初始化幕墙
	const QString kCmdSuccessR = "Success"; // 切屏后返回
	const QString kCmdCallAllVideo = "CallVideoAll"; // 全部调入

	// 场景
    const QString kCmdSceneInfo = "SceneInfo"; // 获取场景信息
	 
	// 其他
	const QString kCmdDataSync = "DataSync";  // 数据同步
	const QString kCmdSyncIpcInfoR = "SyncIpcInfo"; // 同步ipc信息 R : Response
	const QString kCmdSyncDevInfoR = "SyncDevInfo"; //  同步解码卡信息
	const QString kCmdDataSyncFinish = "SyncFinsh";  // 数据同步完成
	const QString kCmdUnlockDevice = "UnlockDevice"; // 解锁设备
};


namespace IPC {
	//"1200W" << "800W" << "600W" << "500W" << "400W" << "300W" << "1080P" << "960P" << "720P" << "D1" << "CIF";
	QVector<int> kResolutionPixels = {
		1200*10000, 800*10000, 600*10000, 500*10000, 400*10000, 300 * 10000,1920*1080,1280*960,1280*720,720*480,352*288
	};
	QVector<QString> kResolutionNames = {
		"1200W", "800W","600W","500W", "400W","300W", "1080P", "960P","720P","D1" ,"CIF"
	};

	QString index2Name(int index)
	{
		if (index >= 0)
		{
			return kResolutionNames.at(index);
		}
		else
		{
			return QString();
		}
	
	}

	int Name2Index(QString resolution)
	{
		if (resolution >= 0)
		{
			return kResolutionNames.indexOf(resolution);
		}
		else
		{
			return 0;			
		}
		
	}

	int index2Pixel(int index)
	{
		if (index >= 0)
		{
			return kResolutionPixels.at(index);
		}
		else
		{
			return 0;
		}
	}

	int pixel2Index(int pixel)
	{
		if (pixel >= 0)
		{
			return kResolutionPixels.indexOf(pixel);
		}
		else
		{
			return 0;
		}
	}

	int findNearIndex(int pixels)
	{
		int minDifference = std::abs(pixels - kResolutionPixels[0]);
		int nearestIndex = 0;

		for (int i = 1; i < kResolutionPixels.size(); i++) {
			int difference = std::abs(pixels - kResolutionPixels[i]);
			if (difference < minDifference) {
				minDifference = difference;
				nearestIndex = i;
			}
		}

		return nearestIndex;
	}
};