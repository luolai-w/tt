#pragma once
#include <functional>
#include <vector>
#pragma warning(disable : 4996)
char* U2G(const char* utf8);
char* G2U(const char* gb2312);
void ParseResource(const char* path, std::function<bool(const char * section, const char *left, const char * right)> func);
void ParseIni(const char * data, std::function<bool(const char * section, const char * left, const char * right)> func);
void ParseLine(const char* data, std::function<bool(const char * section, const char * line)> func);
void ParseCodeBlock(const char* data, std::function<bool(const char * section, const char * ftype, std::vector < std::string >& lines) > func);
bool ParseIniFile(const char * filename, std::function<bool(const char * section, const char *left, const char * right)> func);