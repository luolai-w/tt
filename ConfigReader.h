#pragma once
#include <cstdlib>
#include <iostream>
#include <functional>
#include <vector>
#include "CLTrace.h"
#include <stdio.h>
#include <windows.h>
#include "json/json.h"
typedef struct ConfigItem {
	std::string section;
	std::string left;
	std::string right;
};

typedef struct TemplateCodeItem {
	std::string section;
	std::string type;
	std::vector<std::string> lines;
};
typedef std::vector<ConfigItem *> ConfigData;
typedef std::vector<TemplateCodeItem *> TemplateData;
class ConfigReader
{

	static ConfigData skillData;
	static TemplateData skillTemplate;
	static ConfigReader* instance;
	static std::string confingPath;
	static std::string skillFilePath;
	std::string StrReplace(const std::string& orignStr, const std::string& oldStr, const std::string& newStr);
	void InitConfigData();
	void InitTemplateCode();
	const char * GetValue(ConfigData& configData, const char * typeName, const char * propertyName);
	std::string ConfigReader::GetCodeValue(TemplateData& templateData, const char * templateType, const char * fileType);

public:
	class CTemplateData {
	public:
		enum {
			CONFIG,
			NORMAL,
			SKILL,
			BORNBUFF,
			BUFF,
			BULLET
		};
		int type;//0 config,1 nor,3,skill,4 buff,5 bullet
		std::string templateName;
		std::string className;
		std::string classCName;
		CTemplateData() {
			type = -1;
		}
		bool IsConfig() { return type == (int)CONFIG; }
		bool IsNormal() { return type == (int)NORMAL; }
		bool IsSkill() { return type == (int)SKILL; }
		bool IsBornBuff() { return type == (int)BORNBUFF; }
		bool IsBuff() { return type == (int)BUFF; }
		bool IsBullet() { return type == (int)BULLET; }
		bool IsUndefined() { return (type < (int)CONFIG || type >(int)BULLET); }
	};
	class CSkillData {
	public:
		int skillID;
		std::string fileName;
		std::string skillCName;
		std::string skillName;
		std::vector<CTemplateData> skillRecipes;
		CSkillData() {
			skillID = -2;
		}
	};
	typedef Json::Value Value;
	ConfigReader(void);
	~ConfigReader(void);

	static std::string mTmpStr;
	static ConfigReader* Instance() {
		if (instance == nullptr)
			instance = new ConfigReader();
		return instance;
	}
	static char* WToC(wchar_t * wStr)
	{
		if (wStr == nullptr) return nullptr;
		size_t len = WideCharToMultiByte(CP_ACP, 0, wStr, -1, NULL, 0,NULL,NULL);
		char *cStr;
		cStr = (char*)malloc((len + 1) * sizeof(char));
		WideCharToMultiByte(CP_ACP,0,wStr,-1,cStr,len,NULL,NULL);
		cStr[len] = 0;
		return cStr;
	}
	static wchar_t* CToW(char * cStr)
	{
		if (cStr == nullptr) return nullptr;
		size_t len = MultiByteToWideChar(CP_ACP, 0, cStr, -1, NULL, 0);
		wchar_t *wStr;
		wStr = (wchar_t*)malloc(len * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, cStr, -1, wStr, len);
		return wStr;
	}
	static const char* GetConfigPath() {
		if (confingPath.empty())
		{
			TCHAR szFilePath[MAX_PATH];
			memset(szFilePath, 0, MAX_PATH);
			confingPath = __FILE__;
			if(GetModuleFileName(NULL, szFilePath, MAX_PATH) != 0)
			confingPath = WToC(szFilePath);
			//confingPath = __FILE__;
			int pos = confingPath.rfind("\\");
			confingPath.resize(pos);
			confingPath += "\\config\\";
			CLTrace::CreatDir(confingPath.c_str());
		}
		return confingPath.c_str();
	}
	static const char* GetSkillFilePath() {
		if (skillFilePath.empty())
		{
			TCHAR szFilePath[MAX_PATH];
			memset(szFilePath, 0, MAX_PATH);
			skillFilePath = __FILE__;
			if (GetModuleFileName(NULL, szFilePath, MAX_PATH) != 0)
				skillFilePath = WToC(szFilePath);
			int pos = skillFilePath.rfind("\\");
			skillFilePath.resize(pos);
			skillFilePath += "\\file\\";
			CLTrace::CreatDir(skillFilePath.c_str());
		}
		return skillFilePath.c_str();
	}
	static std::string TryCreate(const std::string& src, const char* skillName, std::function<void(const std::vector<std::string>& items)> func);
	static std::string TryCreate(const std::string& src, const char* skillName);
	bool TryGetSkillValue(const char * skillName , const char * sProperty, bool encoding = false, const char** outOrgValue = NULL);//用于临时存放的mTmpStr 等下一次getvalue的时候就会被替换掉 一般的赋值不能将他长期保存   而第四个参数可以直接获得原始数据
	const char* GetSkillCNName(const char * skillName);
	const char* GetSkillDetailDesc(const char * skillName);
	const char* GetSkillAddPointDesc(const char * skillName);

	bool  ConfigReader::TryGetSkillCode(const char* tName, const char * fName, bool encoding, std::function<const char*(const char* tName)> func);
	bool MakeSkillFile(const char* fileName);
	bool MakeSkillFileEx(const char* fileName);
	static bool ParseSkillRecipesFromJson(const Json::Value& obj, std::map<std::string, std::vector<CSkillData>>& data);
private:
	bool lastValueNull;

};

