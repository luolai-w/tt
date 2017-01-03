#pragma once
#include <string>
#define  LOG_MAX_LENGTH (3 * 1024 * 1024)
class CLTrace {
public:
	static int CreatDir(const char *pDir);
	static int WriteIntoFile(const char* fname, const char* data);
	static CLTrace * Instance() {
		if (instance == nullptr)
		{
			instance = new CLTrace();
		}
		return instance;
	}
	CLTrace();
	virtual ~CLTrace();
	CLTrace(const char* fname);
	int TraceLog(char* fmt, ...);
	int TraceErr(char *fmt, ...);
	int TraceSql(char *fmt, ...);
	int SetTraceFN(const char* fname);
	char* GetTraceFN();
private:
	static CLTrace * instance;
	char m_szFileDir[100];
	char m_szFileName[100];
};