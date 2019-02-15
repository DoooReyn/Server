#ifndef __HELPER_H__
#define __HELPER_H__
#include "type_define.h"

extern __thread uint32 seedp;

class Helper
{
public:
	Helper()
	{
		tzset();
		seedp = time(nullptr);
	}
};

extern Helper g_help;

//key 组合两个为数字为一个数字
int32 GetKeyInt32(int16, int16);
uint32 GetKeyUint32(uint16, uint16);
int64 GetKeyInt64(int32, int32);
uint64 GetKeyUint64(uint32, uint32);

//获取一个数子的低16位
int16 GetKeyLowerInt32(int32 key);
uint16 GetKeyLowerUint32(uint32 key);
int32 GetKeyLowerInt64(int64 key);
uint32 GetKeyLowerUint64(uint64 key);

//获取一个数子的高16位
int16 GetKeyHigherInt32(int32 key);
uint16 GetKeyHigherUint32(uint32 key);
int32 GetKeyHigherInt64(int64 key);
uint32 GetKeyHigherUint64(uint64 key);



//时间戳 转字符串时间
void UTCToStr(time_t timestamp,string& strTime);

//字符串时间转时间戳
time_t StrTimeToUTC(string strTime);

//判断两个时间戳是否为同一天
bool IsSameDay(int64 time1, int64 time2);

//判断两个时间戳相隔天数，是否忽略时分秒
float Diffday(int64 time1, int64 time2, bool bignore = true);

//获取随机数随机[min,max]
int32 RandBetween(int32 min, int32 max);

//在region 中随机 num个不重复的数字 num 必须小于等于 region
void RandNumInRegion(int32 region, int32 num, std::vector<int32>& rands);

//读取cate_dir 目录小所有的文件 包括子目录
void ReadDir(string cate_dir, vector<string>& vecFile);

#endif
