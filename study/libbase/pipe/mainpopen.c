#include "base.h"

int main()
{
	size_t n = 0;
	char buff[MAXLINE] = {0};
	char command[MAXLINE] = {0};

	FILE* fp;
	Fgets(buff, MAXLINE, stdin);
	n = strlen(buff);
	if (buff[n - 1] == '\n')
	{
		n--;
	}

	snprintf(command, sizeof(command), "cat %s", buff);
	fp = Popen(command, "r");

	while(Fgets(buff, MAXLINE, fp) != NULL)
	{
		Fputs(buff, stdout);
	}

	Pclose(fp);
	return 0;
}
