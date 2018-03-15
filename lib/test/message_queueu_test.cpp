#include "message_queue.h"

void TestMessage()
{
	struct Cmd
	{
		int first;
		int second;
	};

	MsgQueue obj;
	Cmd cmd;

	for (int i = 0 ; i < 2048; i++)
	{
		cmd.first = i;
		cmd.second = i + 1;
		obj.Put(sizeof(cmd), (unsigned char*)(&cmd));
	}

	for (int i = 0 ; i < 20480; i++)
	{
		CmdPair* cmd_pair =  obj.Get();
		if (cmd_pair == NULL) { continue; }
		Cmd* cmd = (Cmd*)cmd_pair->second;
		cout << cmd->first << "  " << cmd->second << endl;
		obj.Erase();
	}

	cout << obj.m_cmd_write << endl;
	cout << obj.m_cmd_read << endl;
	cout << obj.m_queue.size() << endl;

	cout << "==============" << endl;

	cmd.first = 0;
	cmd.second = 0 + 1;
	obj.Put(sizeof(cmd), (unsigned char*)(&cmd));
	cout << "==============" << endl;

	cout << obj.m_cmd_write << endl;
	cout << obj.m_cmd_read << endl;
	cout << obj.m_queue.size() << endl;

}

int main(int argc, char const* argv[])
{
	TestMessage();
	return 0;
}