#include "CLTrace.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>

#pragma warning(disable : 4996)

#define IS_SPACE(x) ((x)==' ' || (x)=='\t' || (x) == '\r' || (x) == '\n')

int CLTrace::CreatDir(const char *pDir)
{
	int i = 0;
	int iRet = -1;
	int iLen = 0;
	char* pszDir;
	const char* p = pDir;
	char* path;
	char* name;

	if (NULL == p)
	{
		return iRet;
	}
	pszDir = strdup(pDir);
	iLen = strlen(pszDir);
	path = new char[iLen + 1];
	name = new char[iLen + 1];
	while (1)
	{
		memset(path, 0, iLen + 1);
		memset(name, 0, iLen + 1);
		while (IS_SPACE(*p)) ++p;
		if (!*p) return iRet;
		i = 0;
		while (*p && *p != '\r' && *p != '\n')
		{
			if (*p == '\\' || *p == '/')
			{
				if (i > 0)
				{
					strcat(path, name);
					if ((iRet = access(path, 0)) != 0)
					{
						if ((iRet = _mkdir(path)) != 0) return iRet;
					}
					path[strlen(path)] = '/';
				}
				p++;
				memset(name,0,i);
				i = 0;
			}
			else
			{
				name[i++] = *p++;
			}
		}
	}
	return iRet;
}
int CLTrace::WriteIntoFile(const char* fname, const char* data)
{
	std::string path = __FILE__;
	std::string newData;
	while (!path.empty() && (path.back() == '\\' || path.back() == '/')) path.pop_back();
	int idx = path.rfind('\\');
	if (idx >= 0)
	{
		path.resize(idx);
	}
	path += "\\file\\";
	CLTrace::CreatDir(path.c_str());
	path += fname;
	int ret = -1, ret2 = -1;
	int fp;
	if ((fp = _open(path.c_str(), O_RDONLY | O_BINARY)) >= 0) {
		long len = _filelength(fp);
		if (len > 0 && len < 10 * 1024 * 1024)
		{
			char *temData = (char*)malloc((len) * sizeof(char));
			ret2 = _read(fp, temData, len);
			if (ret2 < len)
				memset(temData + ret2, 0, len - ret2);
			newData = std::string(temData, ret2);
			delete temData;
		}
		_close(fp);
	}

	newData += data;

	if ((fp = _open(path.c_str(), O_BINARY | O_RDWR | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE | _S_IEXEC)) < 0) {
		CLTrace::Instance()->TraceLog("Error:生成文件 %s 失败，检查后再试！", path.c_str());
		return ret;
	}

	ret = _write(fp, newData.c_str(), newData.length());
	CLTrace::Instance()->TraceLog("write in file %s the data of size %d that expect %d + %d", fname, ret, ret2, strlen(data));
	_close(fp);
	return ret;
}



CLTrace* CLTrace::instance = nullptr;
CLTrace::CLTrace()
{
	strcpy(m_szFileName, "log/trace");
}
CLTrace::~CLTrace()
{

}
CLTrace::CLTrace(const char* fname)
{
	strcpy(m_szFileName, fname);
}
int CLTrace::TraceLog(char *fmt,...)
{
	static char * buf = new char[LOG_MAX_LENGTH];
	static char * logfn = new char[LOG_MAX_LENGTH];
	FILE * fp;
	struct tm *now;
	time_t nowtime;
	va_list args;
	if (strlen(m_szFileName) == 0) return 0;
	time(&nowtime);
	now = localtime(&nowtime);
	memset(buf, 0, sizeof(char) * LOG_MAX_LENGTH);
	memset(logfn, 0, sizeof(char) * LOG_MAX_LENGTH);
	sprintf(logfn, "%s%04d%02d%02d.ddd", m_szFileName, now->tm_yday + 1900, now->tm_mon + 1, now->tm_mday);
	if (CreatDir(logfn) != 0) return -1;
	if ((fp = fopen(logfn, "a")) == NULL) return -1;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d> %s\n",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
		now->tm_hour, now->tm_min, now->tm_sec,
		buf);
	fclose(fp);
	return(0);
}

int CLTrace::TraceErr(char *fmt, ...)
{
	static char * buf = new char[LOG_MAX_LENGTH];
	static char * logfn = new char[LOG_MAX_LENGTH];
	FILE * fp;
	struct tm *now;
	time_t nowtime;
	va_list args;
	if (strlen(m_szFileName) == 0) return 0;
	time(&nowtime);
	now = localtime(&nowtime);
	memset(buf, 0, sizeof(char) * LOG_MAX_LENGTH);
	memset(logfn, 0, sizeof(char) * LOG_MAX_LENGTH);
	sprintf(logfn, "%s%s%04d%02d%02d.ddd", m_szFileName,"Error", now->tm_yday + 1900, now->tm_mon + 1, now->tm_mday);
	if (CreatDir(logfn) != 0) return -1;
	if ((fp = fopen(logfn, "a")) == NULL) return -1;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	fprintf(fp, "%04d-%02d-%02d %02d:%02d:%02d>Error: %s\n",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday,
		now->tm_hour, now->tm_min, now->tm_sec,
		buf);
	fclose(fp);
	return(0);
}

int CLTrace::TraceSql(char* fmt, ...)
{
	static char *buf = new char[LOG_MAX_LENGTH];
	static char *logfn = new char[LOG_MAX_LENGTH];
	FILE* fp;
	struct tm *now;
	time_t     nowtime;
	va_list    args;

	if (strlen(m_szFileName) == 0) return(0);

	time(&nowtime);
	now = localtime(&nowtime);
	memset(buf, 0, sizeof(char) * LOG_MAX_LENGTH);
	memset(logfn, 0, sizeof(char) * LOG_MAX_LENGTH);
	sprintf(logfn, "%s%04d%02d%02d.ddd", "base/data",
		now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
	if (CreatDir(logfn) != 0) return -1;
	if ((fp = fopen(logfn, "a")) == NULL) return(-1);
	va_start(args, fmt);
	(void)vsprintf(buf, fmt, args);
	va_end(args);
	fprintf(fp, "%s\n", buf);
	fclose(fp);
	
	return(0);
}

int CLTrace::SetTraceFN(const char* fname)
{
	if (!strlen(fname))
		return -1;
	strcpy(m_szFileName, fname);
	return 0;
}

char* CLTrace::GetTraceFN()
{
	return m_szFileName;
}