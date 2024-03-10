PRAGMA foreign_keys = off;
BEGIN TRANSACTION;

-- 表：UserInfo
DROP TABLE IF EXISTS UserInfo;
CREATE TABLE UserInfo (UserName VARCHAR (20) NOT NULL, UserPwd VARCHAR (20) NOT NULL, UserType VARCHAR (10) NOT NULL, Permission1 VARCHAR (20), Permission2 VARCHAR (20), Permission3 VARCHAR (20), Permission4 VARCHAR (20), Permission5 VARCHAR (20), Permission6 VARCHAR (20), Permission7 VARCHAR (20), UserMark VARCHAR (30), PRIMARY KEY (UserName));
INSERT INTO UserInfo (UserName, UserPwd, UserType, Permission1, Permission2, Permission3, Permission4, Permission5, Permission6, Permission7, UserMark) VALUES ('admin', 'admin', '管理员', '启用', '启用', '启用', '启用', '启用', '启用', '启用', '');
INSERT INTO UserInfo (UserName, UserPwd, UserType, Permission1, Permission2, Permission3, Permission4, Permission5, Permission6, Permission7, UserMark) VALUES ('feiyang', '123456', '操作员', '启用', '启用', '禁用', '禁用', '禁用', '禁用', '禁用', '');

-- 表：NvrInfo
DROP TABLE IF EXISTS NvrInfo;
CREATE TABLE NvrInfo (NvrID INTEGER PRIMARY KEY, NvrName VARCHAR (30) NOT NULL, NvrType VARCHAR (20), NvrIP VARCHAR (18) NOT NULL, UserName VARCHAR (20), UserPwd VARCHAR (20), NvrEnable VARCHAR (4) NOT NULL, NvrMark VARCHAR (100));
INSERT INTO NvrInfo (NvrID, NvrName, NvrType, NvrIP, UserName, UserPwd, NvrEnable, NvrMark) VALUES (1, '网络视频', '海康', '192.168.0.255', 'admin', 'admin', '启用', '');

-- 表：IpcInfo
DROP TABLE IF EXISTS IpcInfo;
CREATE TABLE IpcInfo (IpcID INTEGER PRIMARY KEY, IpcName VARCHAR (50) NOT NULL, NvrName VARCHAR (30), IpcType VARCHAR (30),IpAddr VARCHAR (30), OnvifAddr VARCHAR (150), ProfileToken VARCHAR (50), VideoSource VARCHAR (50), RtspMain VARCHAR (250), RtspSub VARCHAR (250), MainResolutionRatio VARCHAR (30), SubResolutionRatio VARCHAR (30), IpcX INTEGER, IpcY INTEGER, UserName VARCHAR (20), UserPwd VARCHAR (20), IpcEnable VARCHAR (4), IpcMark VARCHAR (100));

INSERT INTO IpcInfo (IpcID, IpcName, NvrName, IpcType,IpAddr, OnvifAddr, ProfileToken, VideoSource, RtspMain, RtspSub, MainResolutionRatio, SubResolutionRatio, IpcX, IpcY, UserName, UserPwd, IpcEnable, IpcMark) VALUES (14, '网络视频1', '网络视频', 'http', '', '', '', '', 'http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4', 'http://vfx.mtime.cn/Video/2021/11/16/mp4/211116131456748178.mp4', '1080P', 'D1', 308, 281, 'admin', 'admin', '启用', '');
INSERT INTO IpcInfo (IpcID, IpcName, NvrName, IpcType, IpAddr, OnvifAddr, ProfileToken, VideoSource, RtspMain, RtspSub, MainResolutionRatio, SubResolutionRatio, IpcX, IpcY, UserName, UserPwd, IpcEnable, IpcMark) VALUES (15, '网络视频2', '网络视频', 'http', '', '', '', '', 'http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4', 'http://vfx.mtime.cn/Video/2023/03/09/mp4/230309152143524121.mp4', '1080P', 'D1', 544, 216, 'admin', 'admin', '启用', '');
INSERT INTO IpcInfo (IpcID, IpcName, NvrName, IpcType,IpAddr, OnvifAddr, ProfileToken, VideoSource, RtspMain, RtspSub, MainResolutionRatio, SubResolutionRatio, IpcX, IpcY, UserName, UserPwd, IpcEnable, IpcMark) VALUES (16, '网络视频3', '网络视频', 'http', '', '', '', '', 'http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4', 'http://vfx.mtime.cn/Video/2023/03/07/mp4/230307085324679124.mp4', '1080P', 'D1', 753, 149, 'admin', 'admin', '启用', '');
INSERT INTO IpcInfo (IpcID, IpcName, NvrName, IpcType,IpAddr, OnvifAddr, ProfileToken, VideoSource, RtspMain, RtspSub, MainResolutionRatio, SubResolutionRatio, IpcX, IpcY, UserName, UserPwd, IpcEnable, IpcMark) VALUES (17, '网络视频4', '网络视频', 'http', '', '', '', '', 'http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4', 'http://vfx.mtime.cn/Video/2022/07/18/mp4/220718132929585151.mp4', '1080P', 'D1', 727, 310, 'admin', 'admin', '启用', '');
INSERT INTO IpcInfo (IpcID, IpcName, NvrName, IpcType,IpAddr, OnvifAddr, ProfileToken, VideoSource, RtspMain, RtspSub, MainResolutionRatio, SubResolutionRatio, IpcX, IpcY, UserName, UserPwd, IpcEnable, IpcMark) VALUES (18, '网络视频5', '网络视频', 'http','', '', '', '', 'http://vfx.mtime.cn/Video/2022/12/17/mp4/221217153424902164.mp4', 'http://vfx.mtime.cn/Video/2022/12/17/mp4/221217153424902164.mp4', '1080P', 'D1', 947, 245, 'admin', 'admin', '启用', '');

-- 表：LogInfo
DROP TABLE IF EXISTS LogInfo;
CREATE TABLE LogInfo (LogID INTEGER PRIMARY KEY, TriggerTime VARCHAR (19) NOT NULL, UserName VARCHAR (20) NOT NULL, LogCh VARCHAR (50), LogType VARCHAR (10) NOT NULL, LogContent VARCHAR (100) NOT NULL, LogMark VARCHAR (200));

-- 表：PollInfo
DROP TABLE IF EXISTS PollInfo;
CREATE TABLE PollInfo (PollID INTEGER PRIMARY KEY, PollGroup VARCHAR (20) NOT NULL, RtspMain VARCHAR (250) NOT NULL, RtspSub VARCHAR (250) NOT NULL);

-- 表：RecordInfo
DROP TABLE IF EXISTS RecordInfo;
CREATE TABLE RecordInfo (RecordCh INTEGER PRIMARY KEY, RecordWeek1 VARCHAR (255), RecordWeek2 VARCHAR (255), RecordWeek3 VARCHAR (255), RecordWeek4 VARCHAR (255), RecordWeek5 VARCHAR (255), RecordWeek6 VARCHAR (255), RecordWeek7 VARCHAR (255), RecordMark VARCHAR (100));

-- 索引：LogIndex
DROP INDEX IF EXISTS LogIndex;
CREATE INDEX LogIndex ON LogInfo (LogID, TriggerTime, UserName, LogType, LogCh);

COMMIT TRANSACTION;
PRAGMA foreign_keys = on;
