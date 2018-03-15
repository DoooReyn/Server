#include "Smtp.h"


void ReadFileList(string cate_dir, vector<string>& vecFile)
{
	DIR* dir;
	struct dirent* ptr;
	string base;

	if ((dir = opendir(cate_dir.c_str())) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	while ((ptr = readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) ///current dir OR parrent dir
		{ continue; }
		else if(ptr->d_type == 8)    ///file
		{ vecFile.push_back(cate_dir + "/" + ptr->d_name); }
		else if(ptr->d_type == 10)    ///link file
		{ continue; }
		else if(ptr->d_type == 4)    ///dir
		{
			base.clear();
			base += cate_dir + "/";
			base += ptr->d_name;
			ReadFileList(base, vecFile);
		}
	}
	closedir(dir);
}

int main()
{
	string filepath = "/root/oschina/Server/study/mail_smtp";
	vector<string> vecFile;
	ReadFileList(filepath.c_str(), vecFile);



	for (unsigned int i = 0; i < vecFile.size(); i++)
	{
		cout << vecFile[i].c_str() << endl;
		//hxabeapkuqllbbhg
		//ngqjdimkuevxbfbd
		CSmtp smtp(
		    25,										/*smtp端口*/
		    "smtp.qq.com",							/*smtp服务器地址*/
		    "593078263@qq.com",						/*你的邮箱地址*/
		    "ngqjdimkuevxbfbd",						/*邮箱密码*/
		    "levi.li@whshengpeng.com",				/*目的邮箱地址*/
		    "",										/*主题*/
		    ""										/*邮件正文*/
		);

		smtp.SetEmailTitle(vecFile[i]);
		smtp.SetContent(vecFile[i]);
		//smtp.AddAttachment(vecFile[i]);

		int err;
		if ((err = smtp.SendEmail_Ex()) != 0)
		{
			if (err == 1)
			{
				cout << "错误1: 由于网络不畅通，发送失败!" << endl;
			}
			if (err == 2)
			{
				cout << "错误2: 用户名错误,请核对!" << endl;
			}
			if (err == 3)
			{
				cout << "错误3: 用户密码错误，请核对!" << endl;
			}
			if (err == 4)
			{
				cout << "错误4: 请检查附件目录是否正确，以及文件是否存在!" << endl;
			}
		}
		break;
	}
	return 0;
}