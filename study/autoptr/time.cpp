
/**
 * @file tt_time.c
 * @brief
 * @author cjx
 * @version
 * @date 2013-04-17
 */
#include <stdio.h>
#include <time.h>

int main(int argc, char* argv[])
{
	/*
	time_t time_utc;
	struct tm tm_local;

	// Get the UTC time
	time(&time_utc);

	// Get the local time
	// Use localtime_r for threads safe
	localtime_r(&time_utc, &tm_local);

	time_t time_local;
	struct tm tm_gmt;

	// Change tm to time_t
	time_local = mktime(&tm_local);

	// Change it to GMT tm
	gmtime_r(&time_utc, &tm_gmt);

	int time_zone = tm_local.tm_hour - tm_gmt.tm_hour;
	if (time_zone < -12)
	{
		time_zone += 24;
	}
	else if (time_zone > 12)
	{
		time_zone -= 24;
	}

	char cur_time[256];
	strftime (cur_time, 256, " %B %A %Y年%m月%d日  %H时%M分%S秒   %c  %x %X", &tm_local);
	printf("LOCAL TIME  :%s\n", cur_time);

	strftime(cur_time, 256, " %B %A %Y年%m月%d日  %H时%M分%S秒   %c  %x %X", &tm_gmt);
	printf("GMT TIME    :%s\n", cur_time);

	printf("Your time zone is +%d. (- is west, + is east)\n", time_zone);

	*/
	//tzset();
	//printf("timezone:%ld\n", timezone);
	time_t t;
	struct tm* gmt, *area;
	tzset(); /*tzset()*/

	t = time(NULL);
	area = localtime(&t);
	printf("Local time is: %s", asctime(area));
	gmt = gmtime(&t);
	printf("GMT is: %s", asctime(gmt));
	return 0;


	return 0;
}
