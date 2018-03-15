#include "regex_parse.h"

int TestRegex()
{

	Regex re;
	//int port = 3306;
	string player;
	string passwd;
	string host;
	string port_str;
	string dbname;
	string url = "mysql://zqgame:zqgame@183.61.85.89:3306/zqgame_001";
	if (re.Compile("mysql://(.+):(.+)@(.+):(.+)/(.+)") && re.Match(url.c_str()))
	{
		std::string port_str;
		re.GetSub(player, 1);
		re.GetSub(passwd, 2);
		re.GetSub(host, 3);
		re.GetSub(port_str, 4);
		//port = atoi(port_str.c_str());
		re.GetSub(dbname, 5);
		printf("%s, %s, %s, %s, %s\n", player.c_str(), passwd.c_str(), host.c_str(), port_str.c_str(), dbname.c_str());
	}
	return 0;
}

int main(int argc, char const* argv[])
{
	TestRegex();
	return 0;
}