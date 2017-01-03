#pragma once
#include "ConfigReader.h"


class SkillAttr {
public:
	enum class BlockFlag { UNDEFINE, START, END };
	SkillAttr();
	~SkillAttr() {};
	static SkillAttr* Instance() {
		if (instance == nullptr)
			instance = new SkillAttr();
		return instance;
	}
	static char* G2U(const char* gb2312);
	static char* U2G(const char* utf8);
	static int clacBlockSize(const std::string& data, size_t srcPos);
	static std::string coding(const std::vector<std::string>& attrs, int indent);
	static int writeFile(const char* fname, const char* data);
	static std::vector<std::string> SkillAttr::ParseAttr(const char* data);
	static std::string SkillAttr::GetMethodContent(const char* data, const char* skillName, const char* method);
	static std::string SkillAttr::GetMethodContentFromFile(const char* fname, const char* skillName, const char* method);
	static std::string AppendMethodContent(const char* data, const char* skillName, const char* method, const char* append);
	static int AppendMethodContentFromFile(const char* fname, const char* skillName, const char* method, const char* append);
	static std::string ReplaceMethodContent(const char* data, const char* skillName, const char* method, const char* replace);
	static int ReplaceMethodContentFromFile(const char* fname, const char* skillName, const char* method, const char* data);
	static int MakeSkillAttr(const char* fname, const char* skillName);
	static bool MakeAttrByConfig(const char* fileName);
	static bool ModifyOrAppendAttr(std::string& data, const char* skillName, const char* descSkillName = nullptr);
private:
	static SkillAttr* instance;
};