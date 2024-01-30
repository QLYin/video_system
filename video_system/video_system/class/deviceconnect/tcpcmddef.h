﻿#pragma once
#include <QString>

struct IpcInfo {
	int init_flag;
	int id;
	int ptz_enable;
	int vda_enable;
	int resolution0;
	int resolution1;
	QString name;
	QString admin;
	QString passwd;
	QString ipaddr;
	QString rtsp_url0;
	QString rtsp_url1;

	IpcInfo() {
		init_flag = -1;
		id = -1;
		ptz_enable = -1;
		vda_enable = -1;
		resolution0 = -1;
		resolution1 = -1;
		name = "";
		admin = "";
		passwd = "";
		ipaddr = "";
		rtsp_url0 = "";
		rtsp_url1 = "";
	}
};

struct DevInfo {
	int dev_id;
	int chn_cnt;
	int floor;
	int upper;
	int width;
	int height;
	int fresh_freq;
	QString ipaddr;
	QString softwareversion;
	QString gateway;
	QString netmask;
	QString mac;
	QVector<int> ipc_indexs;

	DevInfo() {
		dev_id = -1;
		chn_cnt = -1;
		floor = -1;
		upper = -1;
		width = -1;
		height = -1;
		fresh_freq = -1;
		ipaddr = "";
		softwareversion = "";
		gateway = "";
		netmask = "";
		mac = "";
	}
};

enum DATASYNC_E {
	IPCINFO = 1,//IPC 信息 
	SYSSTATUS = 2,  //系统状态 
	DEVINFO = 4, //主从机信息 
	CHNINFO = 8,    //当前各通道上的IPC信息 
	NONE_TYPE,
};

enum SYSTEM_STATS_E
{
	NORMAL = 0, //正常运行 
	SCENE_LOOP = 1, //场景轮巡 
	SEQ_SIGNAL_SWITCH = 2, //单通道序切 
	GROUP_SWITCH = 3 //组切 
};

namespace CommandNS {
	// 解码卡
	extern const QString kCmdDevSearch;
	extern const QString kCmdDevAdd;
	extern const QString kCmdDevDel;
	extern const QString kCmdDevModify;
	extern const QString kCmdDevNumSet;

	// ipc
	extern const QString kCmdIPCSearch;
	extern const QString kCmdIPCDefaultUserPwd;
	extern const QString kCmdIPCAdd;
	extern const QString kCmdIPCAutoAdd;
	extern const QString kCmdIPCDel;
	extern const QString kCmdDIPCModify;
	extern const QString kCmdDIPCClear;

	// 电视墙
	extern const QString kCmdDWallCutScreen;
	extern const QString kCmdDWallCallVideo;
	extern const QString kCmdDWallCloseVideo;
	extern const QString kCmdDWallJoint;
	extern const QString kCmdDWallJointExit;
	extern const QString kCmdDWallJointSet;

	// 场景
	extern const QString kCmdSceneInfo;

    //其他
	extern const QString kCmdDataSync;
	extern const QString kCmdSyncIpcInfoR;
	extern const QString kCmdSyncDevInfoR;
	extern const QString kCmdDataSyncFinish;
	extern const QString kCmdSceneInfo;
	extern const QString kCmdUnlockDevice;

};