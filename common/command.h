#ifndef __COMMAND_H__
#define __COMMAND_H__

enum UserMessageId
{
	//-------------------CLIENT--------------------------------------
	CLIENT_CMD_START = 10000,			//客户端消息开始

	CLIENT_ACCOUT_VERIFY_REQUEST,		//客户端请求确认账号
	CLIENT_ACCOUT_VERIFY_RETURN,		//客户端返回确认账号

	CLIENT_ACCOUT_REG_REQUEST,			//客户端账号注册请求
	CLIENT_ACCOUT_REG_RETURN,			//客户端账号注册返回


	CLIENT_ZONE_LIST_REQUEST,			//区服列表请求
	CLIENT_ZONE_LIST_RETURN,			//区服列表返回

	CLIENT_ZONE_INFO_REQUEST,			//区服信息请求
	CLIENT_ZONE_INFO_RETURN,			//区服信息返回

	CLIENT_ROLE_LIST_REQUEST,			//角色列表请求
	CLIENT_ROLE_LIST_RETURN,			//角色列表返回

	CLIENT_ROLE_CREATE_REQUEST,			//角色创建请求
	CLIENT_ROLE_CREATE_RETURN,			//角色创建返回

	CLIENT_ZONE_LOGIN_REQUEST,			//区服登陆请求
	CLIENT_ZONE_LOGIN_RETURN,			//区服登陆返回


	CLIENT_CMD_END = 200000,			//客户端消息结束

	//-------------------ACCOUNT--------------------------------------
	ACCOUNT_SERVER_CMD_START = 200001,



	ACCOUNT_SERVER_CMD_END = 300000,

	//-------------------LOGIN--------------------------------------
	LOGIN_SERVER_CMD_START = 300001,

	LS_ACCOUT_VERIFY_REQUEST,		//账号验证请求
	LS_ACCOUT_VERIFY_RETURN,		//账号验证返回

	LS_ACCOUT_REG_REQUEST,			//账号注册请求
	LS_ACCOUT_REG_RETURN,			//账号注册返回

	LS_ROLE_LIST_REQUEST,			//角色列表请求
	LS_ROLE_LIST_RETURN,			//角色列表返回

	LS_AS_ROLE_CREATE_REQUEST,			//角色创建请求
	LS_AS_ROLE_CREATE_RETURN,			//角色创建返回 account服发过来

	LS_GS_ROLE_CREATE_REQUEST,			//角色创建通知 通知Game
	LS_GS_ROLE_CREATE_RETURN,			//角色创建返回 Game通知

	LOGIN_SERVER_CMD_END = 400000,

	//-------------------GATE--------------------------------------
	GATE_SERVER_CMD_START = 400001,



	GATE_SERVER_CMD_END = 500000,

	//-------------------GAME--------------------------------------
	GAME_SERVER_CMD_START = 500001,


	GS_GATE_URL_SENDTO_LOGIN,		//GS 发送址到Login

	GAME_SERVER_CMD_END = 600000,
};





#endif
