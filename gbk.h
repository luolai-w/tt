#pragma once
#include <string>
#include <initializer_list>
struct Uni2GbkItem
{
	unsigned short start;
	unsigned short end;
	unsigned short * data;
};

std::string uni2gbk(const std::wstring& wstr);
std::wstring gbk2uni(const std::string& str);