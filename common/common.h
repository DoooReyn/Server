#ifndef __COMMON_H__
#define __COMMON_H__

#define LS "LS"
#define AS "AS"
#define GW "GW"
#define GS "GS"

//链接类型枚举定义
enum CONNECT_TYPE
{
	CONNECT_CLIENT   = 1,	//客户端登录前发起的类型
	CONNECT_ACCOUNT  = 2,	//账号服发起的链接类型
	CONNECT_LOGIN    = 3,	//登陆服发起的链接类型
	CONNECT_GATE     = 4,	//网关服发起的链接类型
	CONNECT_GAME     = 5,	//游戏服发起的链接类型
};

enum ZONE_STATE
{
	ZONE_SHUTDOWN = 0,
	ZONE_NORMAL   = 1,
};

enum CommonMessageId
{
	//通用消息
	CMD_CONNECT_TEST1 = 100,
	CMD_CONNECT_TEST2 = 101,

	CMD_CONNECT_VERIFY_REQUEST = 1111,  //链接确认
	CMD_CONNECT_VERIFY_RETURN = 1112,	//链接确认返回
	CMD_ZONE_STATE_CHANGE = 1113,		//区服状态改变
	CMD_CONNECT_HEART_BEAT = 2222,		//心跳包
};


#define PACK_HEAD 0x01020304
#define PACK_TAIL 0x05060708

#pragma pack(1)
struct PackHead
{
	const unsigned int head = PACK_HEAD;	 //数据包头部
	unsigned int uid = 0;                    //用户id
	unsigned int messageid = 0;              //消息id
	unsigned int size = 0;                   //消息包长度(不包括头部)
	const unsigned int tail = PACK_TAIL;     //数据包尾部
};

const int PACK_HEAD_LEN = sizeof(struct PackHead);

struct CmdConnectVerifyRequest
{
	int id = 0;
	int conntype = 0;
};

//被链接方 处理确认状态
struct CmdConnectVerifyReturn
{
	int result = 0;		//是否链接上
	char src[30] = { 0 }; //源IP
};

//服务器通讯 心跳包 用于检测链接是否正常
struct CmdConnectHeartBeat
{
	int requesttime = 0;	//请求时间
};

#pragma pack()

#endif
