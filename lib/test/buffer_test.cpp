#include "buffer.h"


void TestBuffer()
{

	typedef struct CmdType
	{
		uint8 first;
		uint8 second;
	} Cmd;

	Cmd cmd1 = {1, 2};
	Cmd cmd2 = {3, 4};



	BufferCmdQueue cmd_buffer;
	cout << cmd_buffer.maxSize() << endl;

	cmd_buffer.put((uint8*)&cmd1, sizeof(Cmd));
	cout << cmd_buffer.rd_size() << "  " << cmd_buffer.wr_size() << endl;
	cmd_buffer.put((uint8*)&cmd2, sizeof(Cmd));
	cout << cmd_buffer.rd_size() << "  " << cmd_buffer.wr_size() << endl;
}


int main(int argc, char const* argv[])
{
	TestBuffer();
	return 0;
}