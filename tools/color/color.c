#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[])
{
	char buf[2048] = {0};
	fgets(buf, 2048, stdin);
	char* str = strstr(buf, "ERROR");
	if(str != NULL)
	{
		printf("\033[31;1m%s\033[0m", buf);
		return 0;
	}

	str = strstr(buf, "INFO");
	if(str != NULL)
	{
		printf("\033[32;1m%s\033[0m", buf);
		return 0;
	}

	str = strstr(buf, "WARN");
	if(str != NULL)
	{
		printf("\033[33;1m%s\033[0m", buf);
		return 0;
	}
	printf("%s", buf);
	return 0;
}
