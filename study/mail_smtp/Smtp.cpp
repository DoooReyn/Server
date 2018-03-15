#include "Smtp.h"
#include <iostream>
#include <fstream>
using namespace std;

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


void Split(const string& src, const string& delim, std::vector<string >& vecStr)
{
	size_t last = 0;
	size_t index = src.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		vecStr.push_back(src.substr(last, index - last));
		last = index + 1;
		index = src.find_first_of(delim, last);

	}
	if (index - last > 0)
	{
		vecStr.push_back(src.substr(last, index - last));

	}
}


static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(const char* bytes_to_encode, unsigned int in_len)
{
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
			{ ret += base64_chars[char_array_4[i]]; }
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
		{ char_array_3[j] = '\0'; }

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		{ ret += base64_chars[char_array_4[j]]; }

		while ((i++ < 3))
		{ ret += '='; }

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string)
{
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
	{
		char_array_4[i++] = encoded_string[in_];
		in_++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
			{ char_array_4[i] = base64_chars.find(char_array_4[i]); }

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
			{ ret += char_array_3[i]; }
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
		{ char_array_4[j] = 0; }

		for (j = 0; j < 4; j++)
		{ char_array_4[j] = base64_chars.find(char_array_4[j]); }

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) { ret += char_array_3[j]; }
	}

	return ret;
}


CSmtp::CSmtp(void)
{
	this->content = "";
	this->port = 25;
	this->user = "";
	this->pass = "";
	this->targetAddr = "";
	this->title = "";
	this->domain = "";
}

CSmtp::~CSmtp(void)
{
	DeleteAllAttachment();
	::close(sockClient);
}


CSmtp::CSmtp(
    int port,
    string srvDomain,
    string userName,
    string password,
    string targetEmail,
    string emailTitle,
    string content
)
{
	this->content = content;
	this->port = port;
	this->user = userName;
	this->pass = password;
	this->targetAddr = targetEmail;
	this->title = emailTitle;
	this->domain = srvDomain;
}

bool CSmtp::CreateConn()
{
	//为建立socket对象做准备，初始化环境
	int sockClient = socket(AF_INET, SOCK_STREAM, 0); //建立socket对象
	sockaddr_in addrSrv;
	struct hostent* pHostent;
	pHostent = gethostbyname(domain.c_str());  //得到有关于域名的信息

	addrSrv.sin_addr = *reinterpret_cast<struct in_addr*>(pHostent->h_addr);

	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);
	int err = connect(sockClient, (struct sockaddr*)&addrSrv, sizeof(struct sockaddr));   //向服务器发送请求
	if (err != 0)
	{
		return false;
		//printf("链接失败\n");
	}
	this->sockClient = sockClient;
	if (false == Recv())
	{
		return false;
	}
	return true;
}

bool CSmtp::Send(string& message)
{
	int err = send(sockClient, message.c_str(), message.length(), 0);
	if (err == -1)
	{
		return false;
	}
	cout << "Send:" << message.c_str() << endl;
	return true;
}

bool CSmtp::Recv()
{
	memset(buff, 0, sizeof(char) * (MAXLEN + 1));
	int err = recv(sockClient, buff, MAXLEN, 0); //接收数据
	if (err == -1)
	{
		return false;
	}
	buff[err] = '\0';

	string strBuf = buff;
	vector<string> vecString;
	Split(strBuf, " ", vecString);
	if (vecString.size() == 2 )
	{
		if (vecString[0] == "334" ||
		        vecString[0] == "550")
		{
			cout << "Recv:" << vecString[0].c_str() << " " << base64_decode(vecString[1]) << endl;
		}
	}
	else
	{
		cout << "Recv:" << buff << endl;
	}
	return true;
}

int CSmtp::Login()
{
	string sendBuff;
	sendBuff = "EHLO ";
	sendBuff += user;
	sendBuff += "\r\n";

	if (false == Send(sendBuff) || false == Recv()) //既接收也发送
	{
		return 1; /*1表示发送失败由于网络错误*/
	}

	sendBuff.clear();
	sendBuff = "AUTH LOGIN\r\n";
	if (false == Send(sendBuff) || false == Recv()) //请求登陆
	{
		return 1; /*1表示发送失败由于网络错误*/
	}

	sendBuff.clear();
	int pos = user.find('@', 0);
	sendBuff = user.substr(0, pos); //得到用户名


	/*在这里顺带扯一句，关于string类的length函数与C语言中的strlen函数的区别,strlen计算出来的长度，只到'\0'字符为止,而string::length()函数实际上返回的是string类中字符数组的大小,你自己可以测试一下，这也是为什么我下面不使用string::length()的原因*/

	cout << "user:" << sendBuff.c_str() << endl;
	string ecode = base64_encode(sendBuff.c_str(), strlen(sendBuff.c_str()));
	sendBuff.clear();
	sendBuff = ecode;
	sendBuff += "\r\n";


	if (false == Send(sendBuff) || false == Recv()) //发送用户名，并接收服务器的返回
	{
		return 1; /*错误码1表示发送失败由于网络错误*/
	}
	ecode.clear();
	sendBuff.clear();
	ecode = base64_encode(pass.c_str(), strlen(pass.c_str()));
	sendBuff = ecode;
	sendBuff += "\r\n";


	cout << "pass:" << pass.c_str() << endl;
	if (false == Send(sendBuff) || false == Recv()) //发送用户密码，并接收服务器的返回
	{
		return 1; /*错误码1表示发送失败由于网络错误*/
	}

	if (NULL != strstr(buff, "550"))
	{
		return 2;/*错误码2表示用户名错误*/
	}

	if (NULL != strstr(buff, "535")) /*535是认证失败的返回*/
	{
		return 3; /*错误码3表示密码错误*/
	}
	return 0;
}

bool CSmtp::SendEmailHead()		//发送邮件头部信息
{
	string sendBuff;
	sendBuff = "MAIL FROM: <" + user + ">\r\n";
	if (false == Send(sendBuff) || false == Recv())
	{
		return false; /*表示发送失败由于网络错误*/
	}


	sendBuff.empty();
	sendBuff = "RCPT TO: <" + targetAddr + ">\r\n";
	if (false == Send(sendBuff) || false == Recv())
	{
		return false; /*表示发送失败由于网络错误*/
	}

	sendBuff.empty();
	sendBuff = "DATA\r\n";
	if (false == Send(sendBuff) || false == Recv())
	{
		return false; //表示发送失败由于网络错误
	}

	sendBuff.empty();
	FormatEmailHead(sendBuff);
	if (false == Send(sendBuff))
		//发送完头部之后不必调用接收函数,因为你没有\r\n.\r\n结尾，服务器认为你没有发完数据，所以不会返回什么值
	{
		return false; /*表示发送失败由于网络错误*/
	}
	return true;
}

void CSmtp::FormatEmailHead(string& email)
{
	/*格式化要发送的内容*/
	email = "From: ";
	email += user;
	email += "\r\n";

	email += "To: ";
	email += targetAddr;
	email += "\r\n";

	email += "Subject: ";
	email += title;
	email += "\r\n";

	email += "MIME-Version: 1.0";
	email += "\r\n";

	email += "Content-Type: multipart/mixed;boundary=qwertyuiop";
	email += "\r\n";
	email += "\r\n";
}

bool CSmtp::SendTextBody()  /*发送邮件文本*/
{
	string sendBuff;
	sendBuff = "--qwertyuiop\r\n";
	sendBuff += "Content-Type: text/plain;";
	sendBuff += "charset=\"gb2312\"\r\n\r\n";
	sendBuff += content;
	sendBuff += "\r\n\r\n";
	return Send(sendBuff);
}

int CSmtp::SendAttachment_Ex() /*发送附件*/
{
	for (list<FILEINFO*>::iterator pIter = listFile.begin(); pIter != listFile.end(); pIter++)
	{
		cout << "Attachment is sending ~~~~~" << endl;
		cout << "Please be patient!" << endl;
		string sendBuff;
		sendBuff = "--qwertyuiop\r\n";
		sendBuff += "Content-Type: application/octet-stream;\r\n";
		sendBuff += " name=\"";
		sendBuff += (*pIter)->fileName;
		sendBuff += "\"";
		sendBuff += "\r\n";

		sendBuff += "Content-Transfer-Encoding: base64\r\n";
		sendBuff += "Content-Disposition: attachment;\r\n";
		sendBuff += " filename=\"";
		sendBuff += (*pIter)->fileName;
		sendBuff += "\"";

		sendBuff += "\r\n";
		sendBuff += "\r\n";
		Send(sendBuff);
		ifstream ifs((*pIter)->filePath, ios::in | ios::binary);
		if (false == ifs.is_open())
		{
			return 4; /*错误码4表示文件打开错误*/
		}
		char fileBuff[MAX_FILE_LEN];
		string chSendBuff;
		memset(fileBuff, 0, sizeof(fileBuff));
		/*文件使用base64加密传送*/
		while (ifs.read(fileBuff, MAX_FILE_LEN))
		{
			//cout << ifs.gcount() << endl;
			chSendBuff = base64_encode(fileBuff, MAX_FILE_LEN);
			chSendBuff += '\r';
			chSendBuff += '\n';
			send(sockClient, chSendBuff.c_str(), strlen(chSendBuff.c_str()), 0);
			chSendBuff.clear();
		}
		//cout << ifs.gcount() << endl;
		chSendBuff = base64_encode(fileBuff, MAX_FILE_LEN);
		chSendBuff += '\r';
		chSendBuff += '\n';
		int err = send(sockClient, chSendBuff.c_str(), strlen(chSendBuff.c_str()), 0);

		if (err != int(chSendBuff.length()))
		{
			cout << "文件传送出错!" << endl;
			return 1;
		}
	}
	return 0;
}

bool CSmtp::SendEnd() /*发送结尾信息*/
{
	string sendBuff;
	sendBuff = "--qwertyuiop--";
	sendBuff += "\r\n.\r\n";
	if (false == Send(sendBuff) || false == Recv())
	{
		return false;
	}
	cout << buff << endl;
	sendBuff.empty();
	sendBuff = "QUIT\r\n";
	return (Send(sendBuff) && Recv());
}

int CSmtp::SendEmail_Ex()
{
	if (false == CreateConn())
	{
		return 1;
	}
	//Recv();
	int err = Login(); //先登录
	if (err != 0)
	{
		return err; //错误代码必须要返回
	}
	if (false == SendEmailHead()) //发送EMAIL头部信息
	{
		return 1; /*错误码1是由于网络的错误*/
	}
	if (false == SendTextBody())
	{
		return 1; /*错误码1是由于网络的错误*/
	}
	err = SendAttachment_Ex();
	if (err != 0)
	{
		return err;
	}
	if (false == SendEnd())
	{
		return 1; /*错误码1是由于网络的错误*/
	}
	return 0; /*0表示没有出错*/
}

void CSmtp::AddAttachment(string& filePath) //添加附件
{
	FILEINFO* pFile = new FILEINFO;
	strcpy(pFile->filePath, filePath.c_str());
	const char* p = filePath.c_str();
	strcpy(pFile->fileName, p + filePath.find_last_of("\\") + 1);
	listFile.push_back(pFile);
}

void CSmtp::DeleteAttachment(string& filePath) //删除附件
{
	list<FILEINFO*>::iterator pIter;
	for (pIter = listFile.begin(); pIter != listFile.end(); pIter++)
	{
		if (strcmp((*pIter)->filePath, filePath.c_str()) == 0)
		{
			FILEINFO* p = *pIter;
			listFile.remove(*pIter);
			delete p;
			break;
		}
	}
}

void CSmtp::DeleteAllAttachment() /*删除所有的文件*/
{
	for (list<FILEINFO*>::iterator pIter = listFile.begin(); pIter != listFile.end();)
	{
		FILEINFO* p = *pIter;
		pIter = listFile.erase(pIter);
		delete p;
	}
}

void CSmtp::SetSrvDomain(string& domain)
{
	this->domain = domain;
}

void CSmtp::SetUserName(string& user)
{
	this->user = user;
}

void CSmtp::SetPass(string& pass)
{
	this->pass = pass;
}
void CSmtp::SetTargetEmail(string& targetAddr)
{
	this->targetAddr = targetAddr;
}
void CSmtp::SetEmailTitle(string& title)
{
	this->title = title;
}
void CSmtp::SetContent(string& content)
{
	this->content = content;
}
void CSmtp::SetPort(int port)
{
	this->port = port;
}