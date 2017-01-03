#include <string>
#include "parser.h"
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <atlfile.h>
#include <atlstr.h>
#define IS_SPACE(x) ((x)==' ' || (x)=='\t' || (x) == '\r' || (x) == '\n')

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
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
char* G2U(const char* gb2312)
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

void ParseResource(const char* path, std::function<bool(const char * section, const char *left, const char * right)> func)
{
	int fp = 0;
	if ((fp = _open(path, O_BINARY | O_RDONLY)) < 0) return;
	long len = _filelength(fp);
	if (len <= 0) return;
	char *data = (char*)malloc((len + 1) * sizeof(char));
	int n = _read(fp, data, len);
	memset(data + n, 0, len+1- n);
	//char *gStr = U2G(data);
	//ParseIni(gStr, func);
	ParseIni(data, func);
	_close(fp);
	delete data;
	//if (gStr)
	//	free(gStr);
}

void ParseIni(const char * data, std::function<bool(const char * section, const char * left, const char * right)> func)
{
	std::string section, left, right;
	std::string line;
	const char *p = data;
	while (1)
	{
		line.clear();
		//skip space
		while (IS_SPACE(*p)) ++p;
		if (!*p) return;
		//read line
		while (*p && *p != '\r' && *p != '\n')
		{
			line += *p;
			++p;
		}
		//remove comment
		//aaaaa #这里是注释
		//两个##不是注释
		//例如 <color=##ffffff>变成<color=#ffffff>
		for (size_t i = 0; i < line.size();)
		{
			//if ##
			if (line[i] == '#' && i + 1 < line.size() && line[i + 1] == '#')
			{
				line.erase(line.begin() + i);
			}
			else if (line[i] == '#')
			{
				line.resize(i);
				break;
			}
			++i;
		}

		//skip back space
		while (!line.empty() && IS_SPACE(line.back()))
			line.pop_back();
		if (line.empty()) continue;
		//is section
		if (line.size() >= 2 && line.front() == '[' && line.back() == ']')
		{
			section = line.substr(1, line.size() - 2);
			continue;
		}
		//parse line
		size_t pos = line.find("=");
		if (pos != std::string::npos)
		{
			left = line.substr(0, pos);
			right = line.substr(pos + 1);
			//remove space
			while (!left.empty() && IS_SPACE(left.back()))
				left.pop_back();
			while (!right.empty() && IS_SPACE(right.front()))
				right = right.substr(1);
			if (!func(section.c_str(), left.c_str(), right.c_str()))
			{
				return;
			}
		}
	}
}


void ParseLine(const char* data, std::function<bool(const char * section, const char * line)> func)
{
	std::string section;
	std::string line;
	const char *p = data;
	while (1)
	{
		line.clear();
		//skip space
		while (IS_SPACE(*p)) ++p;
		if (!*p) return;
		//read line
		while (*p && *p != '\r' && *p != '\n')
		{
			line += *p;
			++p;
		}
		//remove comment
		std::string::size_type pos = line.find('#');
		if (pos != std::string::npos)
		{
			line.resize(pos);
		}

		//skip back space
		while (!line.empty() && IS_SPACE(line.back()))
			line.pop_back();
		if (line.empty()) continue;
		//is section
		if (line.size() >= 2 && line.front() == '[' && line.back() == ']')
		{
			section = line.substr(1, line.size() - 2);
			continue;
		}
		//parse line

		//while (!line.empty() && IS_SPACE(line.back()))
		//	line.pop_back();
		//while (!line.empty() && IS_SPACE(line.front()))
		//	line = line.substr(1);

		if (!func(section.c_str(), line.c_str()))
		{
			return;
		}

	}
}


void ParseCodeBlock(const char* data, std::function<bool(const char * section, const char* ftype, std::vector < std::string >& lines) > func)
{
	enum FTYPE_FLAG {UNKNOW, START, END };
	std::string section;
	std::string ftype;
	std::string line;
	std::vector<std::string> lines;
	const char *p = data;
	while (1)
	{
		line.clear();
		//skip space
		while (IS_SPACE(*p)) ++p;
		if (!*p) return;
		//read line
		while (*p && *p != '\r' && *p != '\n')
		{
			line += *p;
			++p;
		}
		//remove comment
		//std::string::size_type pos = line.find('#');
		//if (pos != std::string::npos)
		//{
		//	line.resize(pos);
		//}

		//skip back space
		while (!line.empty() && IS_SPACE(line.back()))
			line.pop_back();
		if (line.empty()) continue;
		//is section
		if (line.size() >= 2 && line.front() == '[' && line.back() == ']')
		{
			section = line.substr(1, line.size() - 2);
			continue;
		}
		if (line.size() >= 2 && line.front() == '<' && line.back() == '>')
		{
			FTYPE_FLAG _flag = START;
			std::string _ftype = line.substr(1, line.size() -2);
			if (!_ftype.empty() && _ftype.front() == '/')
			{
				_ftype = _ftype.substr(1);
				_flag = END;
			}
			while (!_ftype.empty() && IS_SPACE(_ftype.back()))
				_ftype.pop_back();
			while (!_ftype.empty() && IS_SPACE(_ftype.front()))
				_ftype = _ftype.substr(1);
			if (_flag == START && !_ftype.empty())
			{
				ftype = _ftype;
				lines.clear();
			}
			else if(_flag == END && !_ftype.empty() && strcmp(_ftype.c_str(), ftype.c_str()) == 0)
			{
				if (!func(section.c_str(), ftype.c_str(), lines))
				{
					return;
				}
				//lines.clear();
			}
			continue;
		}
		lines.push_back(line);

	}
}


bool ParseIniFile(const char * filename, std::function<bool(const char * section, const char *left, const char * right)> func)
{
	//HMODULE hModule = GetModuleHandle(L"DotaSimulation");
	CString str;
	//GetModuleFileName(hModule, str.GetBuffer(1024), 1024 - 1);
	//str.ReleaseBuffer();
	//int idx = str.ReverseFind('\\');
	//if (idx >= 0)
	//{
	//	str = str.Left(idx);
	//}
	//str += "\\";
	str += filename;

	CAtlFile file;
	HRESULT hr = file.Create(str, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
	if (FAILED(hr)) return false;
	ULONGLONG length = 0;
	hr = file.GetSize(length);
	if (FAILED(hr)) return false;
	if (length > 5 * 1024 * 1024) return false;
	char * data = (char*)malloc((int)length + 1);
	data[(int)length] = 0;
	file.Read(data, (DWORD)length);
	file.Close();
	ParseIni(data, func);
	free(data);
	return true;
}