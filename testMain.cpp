#include <iostream>
#include "stdio.h"
#include "time.h"
#include "atlbase.h"
#include "atlstr.h"
#include "parser.h"
#include <io.h>
#include <fcntl.h>
#include <vector>    
#include "ConfigReader.h"
#include "SkillAttribute.h"
#include <assert.h>
#include <string>
#include <map>
#include <cmath>
#include "json/json.h"
#include <fstream>


#define FILE_PATH	"D:\\Documents\\Visual Studio 2015\\test_08_20\\Config\\"
#pragma warning(disable : 4996)

int writeFile(const char* fname, const char* data);
int writeFileEx(const char* fname, const char* data);
//注意文件不要是UTF-8格式 不要是！ 不要是！ 不要是！ 重要的事说三遍
void test2()
{
	
	ConfigReader::Instance()->MakeSkillFile("CreateConfig.ini");//ok 根据配制生成技能(B,Q,W,E,R,N)文件
}
void test5()
{
	ConfigReader::Instance()->MakeSkillFileEx("CreateConfigEx.js");//强化版吧
}
void test3()
{
	//std::cout << SkillAttr::GetMethodContent("Younai02_Q.cpp", "Younai02QSkill", "GetAttribute").c_str() << std::endl;//ok
	//SkillAttr::AppendMethodContent("Younai02_Q.cpp", "Younai02QSkill", "GetAttribute", "\n\tthis is a test");//ok
	//SkillAttr::Instance()->MakeSkillAttr("Younai02_Q.cpp", "Younai02QSkill");//ok
	SkillAttr::Instance()->MakeAttrByConfig("/CreateConfig.ini");//ok 根据配制及技能描述生成技能Attribute方法内代码
}

void test4()
{
	static std::map<std::string, std::map<std::string, std::string>> allEquipData;//所有装备的数据
	std::string path = ConfigReader::GetConfigPath();
	path += "EquipConfig.txt";
	ParseResource(path.c_str(), [&](const char * section, const char * left, const char * right)->bool
	{
		allEquipData[section][left] =right;
		return true;
	});
	path = ConfigReader::GetConfigPath();
	path += "EquipConfigII.txt";
	ParseResource(path.c_str(), [&](const char * section, const char * left, const char * right)->bool
	{
		allEquipData[section][left] = right;
		return true;
	});
	std::string dataStr;
	for (auto it = allEquipData.begin(); it != allEquipData.end(); ++it)
	{
		const std::string& name = it->first;
		const std::map<std::string, std::string>& data = it->second;
		if (data.find("iconName") != data.end())
		{
			dataStr += name;
			dataStr += "=";
			dataStr += data.find("iconName")->second;
			dataStr += "\n";
		}
	}
	writeFile("mapping.txt", dataStr.c_str());
}
std::string mTmpStr;
std::string getStr()
{
	std::string sValue = "luolaiwang";
	//mTmpStr = std::move(sValue);
	mTmpStr=std::move( sValue);
	return sValue;
}

void ttest()
{
	char *path = __FILE__;
	std::string spath = path;
	std::string _spath = "d:\\documents\\visual studio 2015\test_08_20\\consoleapplication1";
	size_t pos = spath.rfind('\\');
	spath = spath.substr(0, pos);
	std::cout << spath.c_str() << std::endl;
	bool isEqual = _spath == spath;
	std::cout << isEqual << std::endl;
	isEqual = strcmp(spath.c_str(), _spath.c_str()) == 0;
	std::cout << isEqual << std::endl;

	int a[] = { 1,2,3,4,5,6 };
	int *p;
	int *c;
	p = &a[1];
	c = p - 1;
	std::cout << *p << "," << *c << std::endl;
	int d = 100;
	char* s = new char[d];

	writeFile("TestIconv.log", "this is a test(此内容需要以GB2312编码格式查看)！");//字符串以源文件编码方式（GB2312）解码存入文件
	writeFileEx("TestIconv.log", "this is a test(此内容需要以UTF-8编码格式查看)！");//字符串以UTF-8解码存入文件
	wchar_t  *wstr = L"如来神掌";
	char *str = ConfigReader::WToC(wstr);
	wchar_t *wstr2 = ConfigReader::CToW(str);
	std::cout << str << std::endl;
	std::cout << wstr << std::endl;
	std::cout << wstr2 << std::endl;
	if(str)
	free(str);
	if (wstr2)
		free(wstr2);
}
int ReadJsonFromFile(const char* filename)
{
	Json::Reader reader;// 解析json用Json::Reader   
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         

	std::ifstream is;
	is.open(filename, std::ios::binary);
	if (reader.parse(is, root, FALSE))
	{
		std::string code;
		if (!root["files"].isNull())  // 访问节点，Access an object value by name, create a null member if it does not exist.  
			code = root["uploadid"].asString();

		code = root.get("uploadid", "null").asString();// 访问节点，Return the member named key if it exist, defaultValue otherwise.    

		int file_size = root["files"].size();  // 得到"files"的数组个数  
		for (int i = 0; i < file_size; ++i)  // 遍历数组  
		{
			Json::Value val_image = root["files"][i]["images"];
			int image_size = val_image.size();
			for (int j = 0; j < image_size; ++j)
			{
				std::string type = val_image[j]["type"].asString();
				std::string url = val_image[j]["url"].asString();
				printf("type : %s, url : %s \n", type.c_str(), url.c_str());
			}
		}
	}
	is.close();

	return 0;
}
void WriteJsonData(const char* filename)
{
	Json::Reader reader;
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array        

	std::ifstream is;
	is.open(filename, std::ios::binary);
	if (reader.parse(is, root))
	{
		Json::Value arrayObj;   // 构建对象  
		Json::Value new_item, new_item1;
		new_item["date"] = "2011-11-11";
		new_item1["time"] = "11:11:11";
		arrayObj.append(new_item);  // 插入数组成员  
		arrayObj.append(new_item1); // 插入数组成员  
		int file_size = root["files"].size();
		for (int i = 0; i < file_size; ++i)
			root["files"][i]["exifs"] = arrayObj;   // 插入原json中 
		std::string out = root.toStyledString();
		// 输出无格式json字符串  
		Json::FastWriter writer;
		std::string strWrite = writer.write(root);
		std::ofstream ofs;
		ofs.open("test_write.json");
		ofs << strWrite;
		ofs.close();
	}

	is.close();
}
void test_json()
{
	const char* str = "{\"uploadid\": \"UP000000\",\"code\": 100,\"msg\": \"\",\"files\": \"\"}";

	Json::Reader reader;
	Json::Value root;
	if (reader.parse(str, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	{
		std::string upload_id = root["uploadid"].asString();  // 访问节点，upload_id = "UP000000"  
		int code = root["code"].asInt();    // 访问节点，code = 100 
		int num = root.size();
		if (root.type() == Json::objectValue)
		{
			Json::Value::Members keys = root.getMemberNames();
			for (Json::Value::Members::iterator k = keys.begin(); k != keys.end(); k++)
			{
				Json::Value v = root[*k];
				std::cout << *k << v << std::endl;
			}
		}
		
		for (Json::Value::iterator itr = root.begin();itr != root.end();itr++)
		{
			std::string v = "";
			std::cout <<itr.key().asString();
			if ((*itr).isNull())
				std::cout << "= " << "" << std::endl;
			else if ((*itr).isString())
				std::cout << "= " << (*itr).asString() << std::endl;
			else if((*itr).isInt())
				std::cout << "=" << (*itr).asInt() << std::endl;
			else
				std::cout << "=" << (*itr) << std::endl;
			
		}
	}
}
int main()
{
	char *logfn = new char[1024];
	wchar_t *logfnw = new wchar_t[1024];
	struct tm *now;
	time_t     nowtime;


	time(&nowtime);
	now = localtime(&nowtime);
	
	//int ret = sprintf(logfn, "%s%04d%02d%02d.ddd", "base/data",
	//	now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	//ret = sprintf(logfn, "%04d-%02d-%02d %02d:%02d:%02d> %s\n",
	//	now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
	//	now->tm_hour, now->tm_min, now->tm_sec, "test");
	//int ret2 = swprintf(logfnw, L"%s%s%04d%02d%02d.ddd", L"base/data",L"test",
	//	now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	//std::cout << "ret = " << ret << "log:" << logfn << std::endl;
	//std::cout << "ret2 = " << ret2 << "log:" << CW2A(logfnw) << std::endl;
	//std::cout << "test main!" << std::endl;
	//char buf[80];
	//_getcwd(buf, sizeof(buf));
	//printf("current working directory: %s\n", buf);

	ttest();
	char ch;
	char name[255] = { 0 };
	char *a = "test";
	std::cout << strlen(name) << sizeof(name) <<sizeof(a)<< std::endl;
	while (1)
	{
		std::cout << "/*************************************************/\n"\
			<< "*\t2. make skill file。                     *\n"\
			<< "*\t3. modify the content of attribute。     *\n"\
			<< "*\t4. from equipconfig to buffmapping。     *\n"\
			<< "*\t5. make skill file by json（强化版）。    *\n"\
			<< "*\tn/N. quit。                              *\n"\
			<< "/*************************************************/" << std::endl;
		std::cin>>ch;
		switch (ch)
		{
		case '2':test2();
			break;
		case '3':test3();
			break;
		case '4': test4();
			break;
		case '5': test5();
			break;
		case 'n':
		case 'N':
			return 0;
			break;
		default:
			break;
		}
		std::cin.ignore(1024, '\n');
	}
	return 1;

	int ret = sprintf(logfn, "%04d%02d%02d  log-> %s%s",
			now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, FILE_PATH, "result.txt");
	std::cout << logfn << std::endl;
	std::getchar();
	delete logfn;
	delete logfnw;
	return 0;
}


int writeFile(const char* fname, const char* data)
{
	std::string fn = FILE_PATH;
	fn += fname;
	int ret = -1;
	int fp;
	char * buff = nullptr;
	int len = 0;
	if ((fp = _open(fn.c_str(), O_BINARY|O_RDONLY)) >= 0) {
		len = _filelength(fp);
		buff = (char*)malloc((len + 1) * sizeof(char));
		if(buff != nullptr)
		ret = _read(fp, buff, len);
		memset(buff + ret, 0, len+1- ret);
		len = ret;
		_close(fp);
	}
	
	if ((fp = _open(fn.c_str(), O_CREAT | O_BINARY | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE | _S_IEXEC)) < 0) {
		std::cout << "open file " << fn.c_str() << " failed " << fp << std::endl;
		return ret;
	}
	char title[200] = { 0 };
	struct tm *now;
	time_t     nowtime;
	time(&nowtime);
	now = localtime(&nowtime);
	ret = sprintf(title, "\nDate: %04d%02d%02d\n",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	len += strlen(title) + strlen(data);
	char * nBuff = (char*)malloc((len + 1) * sizeof(char));
	if (nBuff == nullptr) {
		_close(fp);
		return ret;
	}
	strcpy(nBuff, title);
	strcpy(nBuff + strlen(title), data);
	if(buff)
	strcpy(nBuff + strlen(data) + strlen(title), buff);
	nBuff[len] = 0;
	ret = _write(fp, nBuff, len);
	_close(fp);
	if(buff)
	free(buff);
	if(nBuff)
	free(nBuff);
	return ret;
}




int writeFileEx(const char* fname, const char* data)
{
	std::string fn = FILE_PATH;
	fn += fname;
	int ret = -1;
	int fp;
	char * buff = nullptr;
	int len = 0;
	if ((fp = _open(fn.c_str(), O_BINARY|O_RDONLY)) >= 0) {
		len = _filelength(fp);
		buff = (char*)malloc((len + 1) * sizeof(char));
		if (buff != nullptr)
			ret = _read(fp, buff, len);
		memset(buff + ret, 0, len+1 - ret);
		len = ret;
		_close(fp);
	}

	if ((fp = _open(fn.c_str(), O_CREAT | O_BINARY | O_RDWR | O_TRUNC, S_IREAD | S_IWRITE | _S_IEXEC)) < 0) {
		std::cout << "open file " << fn.c_str() << " failed " << fp << std::endl;
		return ret;
	}
	char title[200] = { 0 };
	struct tm *now;
	time_t     nowtime;
	time(&nowtime);
	now = localtime(&nowtime);
	ret = sprintf(title, "\nDate(日期): %04d%02d%02d\n",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	char * p1 = G2U(title);
	char * p2 = G2U(data);
	//char * p3 = G2U(buff);
	//len = strlen(p3);
	len += strlen(p1) + strlen(p2);
	char * nBuff = (char*)malloc((len + 1) * sizeof(char));
	if (!nBuff) {
		_close(fp);
		return ret;
	}
	strcpy(nBuff, p1);
	strcpy(nBuff + strlen(p1), p2);
	//if(buff)
	strcpy(nBuff + strlen(p1) + strlen(p2), buff);
	nBuff[len] = 0;
	ret = _write(fp, nBuff, len);
	_close(fp);
	if(p1)
	delete[] p1;
	if(p2)
	delete[] p2;
	//if (p3) delete[] p3;
	if(buff)
	free(buff);
	if(nBuff)
	free(nBuff);
	return ret;
}