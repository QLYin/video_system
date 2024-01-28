#include "tcpcmddef.h"

namespace CommandNS {
	// 解码卡
	const QString kCmdDevSearch = "SysSetSearch"; // 搜索从卡
	const QString kCmdDevAdd = "SysSetAddDev"; // 添加解码卡
	const QString kCmdDevDel = "SysSetDelDev"; // 删除解码卡
	const QString kCmdDevModify = "SysSetModifyDev"; // 修改解码卡参数
	const QString kCmdDevNumSet = "SysSetModifyDevNum"; //设置从卡个数

	// ipc
	const QString kCmdIPCSearch = "SearchIpc"; // 搜索IPC
	const QString kCmdIPCDefaultUserPwd = "SetDefaultUserPasswdIpc"; //设置默认账号密码
	const QString kCmdIPCAdd = "AddIpc"; // 添加ipc
	const QString kCmdIPCAutoAdd = "AutoAddIpc2";
	const QString kCmdIPCDel = "DelIpc"; // 删除IPC
	const QString kCmdDIPCModify = "EditIpc"; // 编辑ipc
	const QString kCmdDIPCClear = "ClearIpc"; // 清空ipc

	// 电视墙
	const QString kCmdDWallCutScreen = "CutScreen";  //切屏
	const QString kCmdDWallCallVideo = "CallVedio";  //调入视频
	const QString kCmdDWallCloseVideo = "CloseVedio"; // 关闭视频
	const QString kCmdDWallJoint = "Joint";    // 拼接
	const QString kCmdDWallJointExit = "JointExit"; //退出拼接
	const QString kCmdDWallJointSet = "JointSet"; // 设置幕墙

	// 场景
    const QString kCmdSceneInfo = "SceneInfo"; // 获取场景信息
	 
	// 其他
	const QString kCmdDataSync = "DataSync";  // 数据同步
	const QString kCmdSyncIpcInfoR = "SyncIpcInfo"; // 同步ipc信息 R : Response
	const QString kCmdSyncDevInfoR = "SyncDevInfo"; //  同步解码卡信息
	const QString kCmdDataSyncFinish = "SyncFinsh";  // 数据同步完成
	const QString kCmdUnlockDevice = "UnlockDevice"; // 解锁设备
};