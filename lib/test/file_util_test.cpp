#include "file_util.h"

int main()
{
	string result;
	int64 size = 0;
	string file("/proc/self");
	int32 err = ReadFile(file, 1024, &result, &size);
	printf("%d %lu %lld\n", err, result.size(), size);
	file = "/proc/self";
	err = ReadFile(file, 1024, &result, NULL);
	printf("%d %lu %lld\n", err, result.size(), size);

	file = "/proc/self/cmdline";
	err = ReadFile(file, 1024, &result, &size);
	printf("%d %lu %lld\n", err, result.size(), size);

	file = "/dev/null";
	err = ReadFile(file, 1024, &result, &size);
	printf("%d %zd %lld\n", err, result.size(), size);
	file = "/dev/zero";
	err = ReadFile(file, 1024, &result, &size);
	printf("%d %zd %lld\n", err, result.size(), size);
	file = "/notexist";
	err = ReadFile(file, 1024, &result, &size);
	printf("%d %zd % lld\n", err, result.size(), size);

	file = "/dev/zero";
	err = ReadFile(file, 102400, &result, &size);
	printf("%d %zd %lld\n", err, result.size(), size);

	file = "/dev/zero";
	err = ReadFile(file, 102400, &result, NULL);
	printf("%d %zd %lld\n", err, result.size(), size);
	return 0;
}
