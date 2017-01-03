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
#include <assert.h>
#include <string>
#include "SkillAttribute.h"
#include <map>
#include <algorithm>

#pragma warning(disable : 4996)
#define IS_SPACE(x) ((x)==' ' || (x)=='\t' || (x) == '\r' || (x) == '\n')


SkillAttr * SkillAttr::instance = nullptr;
SkillAttr::SkillAttr()
{
}
//UTF-8到GB2312的转换
char* SkillAttr::U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* SkillAttr::G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
int SkillAttr::writeFile(const char* fname, const char* data)
{
	std::string fpath = fname;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		fpath = ConfigReader::GetConfigPath() + fpath;
	}
	int ret = -1;
	int fp;
	if ((fp = _open(fpath.c_str(), O_CREAT | O_RDWR | O_APPEND | O_BINARY, S_IREAD | S_IWRITE | _S_IEXEC)) < 0) {
		CLTrace::Instance()->TraceErr("open file %s failed %d", fpath, fp);
		return ret;
	}
	char title[200] = { 0 };
	struct tm *now;
	time_t     nowtime;
	time(&nowtime);
	now = localtime(&nowtime);
	ret = sprintf(title, "\nDate: %04d%02d%02d\n",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	ret = _write(fp, title, strlen(title));
	ret = _write(fp, data, strlen(data));
	_close(fp);
	return ret;
}
std::string SkillAttr::coding(const std::vector<std::string>& attrs, int indent)
{
#define MAKE_INDENT()	i = indent;\
while (i > 0){ codeStr += '\t'; i--;}
	std::string codeStr;
	std::vector < std::string >::const_iterator itr;
	int i = 0;
	for (itr = attrs.begin(); itr != attrs.end(); itr++)
	{
		codeStr += '\r\n';
		if ((*itr).back() == '{')
		{
			MAKE_INDENT()
				indent++;
		}
		else if ((*itr).front() == '}')
		{
			indent--;
			MAKE_INDENT()
		}
		else
		{
			MAKE_INDENT()
		}
		codeStr += *itr;

	}
	return codeStr;
}
int SkillAttr::clacBlockSize(const std::string& data, size_t srcPos)
{
	BlockFlag bflag = BlockFlag::UNDEFINE;
	const char* p = data.c_str();
	const char *sPos, *ePos;
	p += srcPos;
	sPos = ePos = p;
	int startFlag = 0;
	while (bflag != BlockFlag::END)
	{
		if (startFlag == 0 && bflag != BlockFlag::UNDEFINE)
		{
			ePos = p;
			bflag = BlockFlag::END;
			continue;
		}
		if (!*p) break;
		if (*p == '{')
		{
			if (bflag == BlockFlag::UNDEFINE)
			{
				bflag = BlockFlag::START;
				sPos = p;
			}
			startFlag += 1;
		}
		if (*p == '}')
		{
			if (bflag == BlockFlag::START)
			{
				startFlag -= 1;
			}
		}
		p++;
	}
	if (bflag == BlockFlag::END)
		return (ePos - sPos - 2);
	return -1;
}
std::string SkillAttr::GetMethodContent(const char* data, const char* skillName, const char* method)
{
	std::string searchStr = skillName;
	std::string dataStr;
	searchStr += "::";
	searchStr += method;
	dataStr = std::string(data, strlen(data));
	size_t pos = dataStr.find(searchStr);
	if (pos == std::string::npos) {
		return "";
	}
	int _size = clacBlockSize(dataStr, pos);
	if (_size < 0)
	{
		return "";
	}
	pos = dataStr.find('{', pos);
	if (pos == std::string::npos)
	{
		return "";
	}
	dataStr = dataStr.substr(pos);
	dataStr.resize(_size + 2);
	return dataStr;
}
std::string SkillAttr::GetMethodContentFromFile(const char* fname, const char* skillName,const char* method)
{
	std::string fpath = fname;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		//fpath = ConfigReader::GetConfigPath() + fpath;
		fpath = ConfigReader::GetSkillFilePath() + fpath;
	}
	std::string dataStr;
	int fp = 0;
	if ((fp = _open(fpath.c_str(), O_BINARY | O_RDONLY)) < 0) return "";
	long len = _filelength(fp);
	if (len <= 0) return "";
	char *data = (char*)malloc(len * sizeof(char));
	int n = _read(fp, data, len);
	memset(data + n, 0, len - n);
	dataStr = std::string(data,n);
	dataStr = GetMethodContent(dataStr.c_str(), skillName, method);
	_close(fp);
	delete data;
	return dataStr;
}
std::vector<std::string> SkillAttr::ParseAttr(const char* data)
{
#define  SKIP_SPACE(s) while (!s.empty() && IS_SPACE(s.back())) s.pop_back();\
	while (!s.empty() && IS_SPACE(s.front())) s = s.substr(1);
	std::string line;
	std::string left, right;
	std::vector<std::string> nameList;
	const char *p = data;
	while (1)
	{
		line.clear();
		//skip space
		while (IS_SPACE(*p)) ++p;
		if (!*p) break;
		//read line
		while (*p && *p != '\r' && *p != '\n')
		{
			line += *p;
			++p;
		}
		//remove comment
		std::string::size_type pos = line.find("//");
		if (pos != std::string::npos)
		{
			line.resize(pos);
		}
		pos = line.find("strcmp(");
		if (pos == std::string::npos)
		{
			continue;
		}
		line = line.substr(pos + 7);
		if (line.empty()) continue;
		pos = line.find(')');
		if (pos == std::string::npos)
		{
			continue;
		}
		line.resize(pos);
		if (line.empty()) continue;
		//parse line
		pos = line.find(",");
		if (pos == std::string::npos) continue;
		left = line.substr(0, pos);
		right = line.substr(pos + 1);
		SKIP_SPACE(left)
		SKIP_SPACE(right)

		if (left.size() >= 2 && left.front() == '\"' && left.back() == '\"')
		{
			left = left.substr(1, left.size() - 2);
			//skip back space
			SKIP_SPACE(left);
			if (!left.empty())
			nameList.push_back(left);
		}
		if (right.size() >= 2 && right.front() == '\"' && right.back() == '\"')
		{
			right = right.substr(1, right.size() - 2);
			//skip back space
			SKIP_SPACE(right);
			if (!right.empty())
				nameList.push_back(right);
		}
	}

	return nameList;	
}
std::string SkillAttr::AppendMethodContent(const char* data, const char* skillName, const char* method, const char* appendData)
{
	size_t pos = 0;
	std::string searchStr;
	std::string buff = data;
	if (appendData == nullptr || strlen(appendData) == 0)
		return buff;
	searchStr = skillName;
	searchStr += "::";
	searchStr += method;
	pos = buff.find(searchStr);
	if (pos == std::string::npos) {
		CLTrace::Instance()->TraceLog("AppendMethodContent: not find %s::%s", skillName, method);
		return buff;
	}
	pos = buff.find('{', pos);
	if (pos == std::string::npos)
	{
		CLTrace::Instance()->TraceLog("AppendMethodContent: not find %s::%s format not match", skillName, method);
		return buff;
	}
	pos += 1;
	//while (pos < buff.size())
	//{
	//	char c = buff[pos];
	//	if (c == ' ' || c == '\r' || c == '\n') pos += 1;
	//	else break;
	//}
	buff.replace(pos, 0, appendData);
	return buff;
}
int SkillAttr::AppendMethodContentFromFile(const char* fname, const char* skillName, const char* method, const char* data)
{
	std::string fpath = fname;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		//fpath = ConfigReader::GetConfigPath() + fpath;
		fpath = ConfigReader::GetSkillFilePath() + fpath;
	}
	size_t pos = 0;
	std::string buffStr;
	std::string searchStr;
	int fp = 0;
	int ret = -1;
	if (data == nullptr || strlen(data) == 0)
		return 0;
	if ((fp = _open(fpath.c_str(), O_BINARY | O_RDONLY)) < 0) {
		CLTrace::Instance()->TraceErr("AppendMethodContent:open file %s failed %d", fname, fp);
		//assert(0);
		return ret;
	}
	long len = _filelength(fp);
	if (len <= 0) return ret;
	char *buff = (char*)malloc((len) * sizeof(char));
	ret = _read(fp, buff, len);
	_close(fp);
	memset(buff + ret, 0, len - ret);
	buffStr = std::string(buff,ret);
	buffStr = AppendMethodContent(buffStr.c_str(), skillName, method, data);
	if ((fp = _open(fpath.c_str(), O_BINARY | O_RDWR | O_TRUNC)) < 0) {
		CLTrace::Instance()->TraceErr("open file %s failed %d", fname, fp);
		assert(0);
		return -1;
	}
	ret = _write(fp, buffStr.c_str(), buffStr.length());
	_close(fp);
	delete buff;
	return ret;
}
std::string SkillAttr::ReplaceMethodContent(const char* data, const char* skillName, const char* method, const char* replaceData)
{
	
	size_t pos = 0;
	std::string buffStr = std::string(data,strlen(data));
	std::string searchStr;
	if (replaceData == nullptr || strlen(replaceData) == 0)
		return buffStr;
	searchStr = skillName;
	searchStr += "::";
	searchStr += method;
	pos = buffStr.find(searchStr);
	if (pos == std::string::npos) {
		CLTrace::Instance()->TraceLog("ReplaceMethodContent: not find %s::%s ", skillName, method);
		return buffStr;
	}
	pos = buffStr.find('{', pos);
	if (pos == std::string::npos)
	{
		CLTrace::Instance()->TraceLog("ReplaceMethodContent: not find %s::%s format not match", skillName, method);
		return buffStr;
	}
	int _size = clacBlockSize(buffStr, pos);
	if (_size < 0) _size = 0;
	buffStr.replace(pos + 1, _size, data);

	return buffStr;
}
int SkillAttr::ReplaceMethodContentFromFile(const char* fname, const char* skillName, const char* method, const char* data)
{
	std::string fpath = fname;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		//fpath = ConfigReader::GetConfigPath() + fpath;
		fpath = ConfigReader::GetSkillFilePath() + fpath;
	}
	size_t pos = 0;
	std::string buffStr;
	std::string searchStr;
	int fp = 0;
	int ret = -1;
	if ((fp = _open(fpath.c_str(), O_BINARY | O_RDONLY)) < 0) {
		CLTrace::Instance()->TraceErr("open file %s failed %d", fname, fp);
		assert(0);
		return ret;
	}
	long len = _filelength(fp);
	if (len <= 0) return ret;
	char *buff = (char*)malloc((len) * sizeof(char));
	ret = _read(fp, buff, len);
	_close(fp);
	memset(buff + ret, 0, len - ret);
	buffStr = std::string(buff, ret);
	buffStr = ReplaceMethodContent(buffStr.c_str(), skillName, method, data);
	if ((fp = _open(fpath.c_str(), O_BINARY | O_RDWR | O_TRUNC)) < 0) {
		CLTrace::Instance()->TraceErr("open file %s failed %d", fname, fp);
		assert(0);
		return -1;
	}
	ret = _write(fp, buffStr.c_str(), buffStr.length());
	_close(fp);
	delete buff;
	return ret;
}
int SkillAttr::MakeSkillAttr(const char* fname, const char* skillName)
{
	std::vector<std::string> exceptName;
	std::string attrContent;
	std::vector<std::string> lines;
	attrContent = GetMethodContent(fname, skillName, "GetAttribute");
	exceptName = ParseAttr(attrContent.c_str());
	ConfigReader::TryCreate(ConfigReader::Instance()->GetSkillDetailDesc(skillName), skillName, [&](const std::vector<std::string>& attrs) {
		std::vector < std::string >::const_iterator itr;
		std::string line;
		for (itr = attrs.begin(); itr != attrs.end(); itr++)
		{
			if (itr->size() < 2) continue;
			if (!exceptName.empty() && std::find_if(exceptName.begin(), exceptName.end(), [&](std::string s)->bool {
				if (strcmp(s.c_str(), itr->c_str()) == 0) return true;
				return false;
			}) != exceptName.end())continue;
			exceptName.push_back(*itr);
			line.clear();
			line += "if(std::strcmp(name, \"";
			line += *itr;
			line += "\") == 0)";
			lines.push_back(line);
			lines.push_back("{");
			lines.push_back("}");
		}

	});
	ConfigReader::TryCreate(ConfigReader::Instance()->GetSkillAddPointDesc(skillName), skillName, [&](const std::vector<std::string>& attrs) {
		std::vector < std::string >::const_iterator itr;
		std::string line;
		for (itr = attrs.begin(); itr != attrs.end(); itr++)
		{
			if (itr->size() < 2) continue;
			if (!exceptName.empty() && std::find_if(exceptName.begin(), exceptName.end(), [&](std::string s)->bool {
				if (strcmp(s.c_str(), itr->c_str()) == 0) return true;
				return false;
			}) != exceptName.end())continue;
			exceptName.push_back(*itr);
			line.clear();
			line += "if(std::strcmp(name, \"";
			line += *itr;
			line += "\") == 0)";
			lines.push_back(line);
			lines.push_back("{");
			lines.push_back("}");
		}

	});
	
	std::string content = coding(lines, 1);
	int ret = AppendMethodContentFromFile(fname, skillName, "GetAttribute", content.c_str());
	return ret;
}
bool SkillAttr::MakeAttrByConfig(const char* fileName)
{
	std::map<std::string, std::vector<std::pair<std::string, std::string>>> data;
	std::function<bool(const char * section, const char *left, const char * right)> parseConfig = [&](const char * section, const char * left, const char * right)->bool {
		data[section].push_back(std::make_pair(left, right));
		return true;
	};
	std::string fpath = fileName;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		fpath = ConfigReader::GetConfigPath() + fpath;
	}
	if (!ParseIniFile(fpath.c_str(), parseConfig))
	{
		CLTrace::Instance()->TraceLog("Error:文件 %s 解析失败，检查后再试！", fpath.c_str());
		return false;
	}

	for (std::map<std::string, std::vector<std::pair<std::string, std::string>>>::iterator it = data.begin(); it != data.end(); it++)
	{
		for (std::vector<std::pair<std::string, std::string>>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
		{
			MakeSkillAttr((it->first + ".cpp").c_str(), (*vit).second.c_str());
		}
	}
	return true;
}

bool SkillAttr::ModifyOrAppendAttr(std::string& data, const char* skillName, const char* descSkillName)
{
	if (data.empty()) return false;
	std::vector<std::string> exceptName;
	std::string attrContent;
	std::vector<std::string> lines;
	if (descSkillName == nullptr)
		descSkillName = skillName;
	attrContent = GetMethodContent(data.c_str(), skillName, "GetAttribute");
	exceptName = ParseAttr(attrContent.c_str());
	ConfigReader::TryCreate(ConfigReader::Instance()->GetSkillDetailDesc(descSkillName), skillName, [&](const std::vector<std::string>& attrs) {
		std::vector < std::string >::const_iterator itr;
		std::string line;
		for (itr = attrs.begin(); itr != attrs.end(); itr++)
		{
			if (itr->size() < 2) continue;
			if (!exceptName.empty() && std::find_if(exceptName.begin(), exceptName.end(), [&](std::string s)->bool {
				if (strcmp(s.c_str(), itr->c_str()) == 0) return true;
				return false;
			}) != exceptName.end())continue;
			exceptName.push_back(*itr);
			line.clear();
			line += "if(std::strcmp(name, \"";
			line += *itr;
			line += "\") == 0)";
			lines.push_back(line);
			lines.push_back("{");
			lines.push_back("}");
		}

	});
	ConfigReader::TryCreate(ConfigReader::Instance()->GetSkillAddPointDesc(descSkillName), skillName, [&](const std::vector<std::string>& attrs) {
		std::vector < std::string >::const_iterator itr;
		std::string line;
		for (itr = attrs.begin(); itr != attrs.end(); itr++)
		{
			if (itr->size() < 2) continue;
			if (!exceptName.empty() && std::find_if(exceptName.begin(), exceptName.end(), [&](std::string s)->bool {
				if (strcmp(s.c_str(), itr->c_str()) == 0) return true;
				return false;
			}) != exceptName.end())continue;
			exceptName.push_back(*itr);
			line.clear();
			line += "if(std::strcmp(name, \"";
			line += *itr;
			line += "\") == 0)";
			lines.push_back(line);
			lines.push_back("{");
			lines.push_back("}");
		}

	});
	std::string content = coding(lines, 1);
	//std::string newData = AppendMethodContent(data.c_str(), skillName, "GetAttribute", content.c_str());
	//data.resize(newData.size());
	//data.assign(newData);
	data = AppendMethodContent(data.c_str(), skillName, "GetAttribute", content.c_str());
	return true;
}