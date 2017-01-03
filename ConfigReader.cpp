#pragma once
#include "ConfigReader.h"
#include "parser.h"
#include <stdio.h >
#include <limits>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include "fstream"
#include <assert.h>
#include "SkillAttribute.h"

#define STR_MISSING_INFO "信息暂缺"

namespace {
	class Myhelper {
	public:
		static bool IsStringEqual(const char * begin, const char * end, const char * s)
		{
			int len = (int)strlen(s);
			if (len == end - begin)
			{
				if (memcmp(begin, s, len) == 0)
					return true;
			}
			return false;
		}
		//如果[begin,end)以s开头，则返回begin+strlen(s)，否则NULL
		static const char * StringAdvance(const char * begin, const char * end, const char * s)
		{
			int len = strlen(s);
			if (end - begin >= len)
			{
				if (memcmp(begin, s, len) == 0)
					return begin + len;
			}
			return NULL;
		}
		//解析简单整形，只支持正数，返回正数后面的位置
		static const char * ParseSimpleInt(const char * begin, const char * end, int& val)
		{
			val = 0;
			const char * p = begin;
			while (p < end)
			{
				if (*p >= '0' && *p <= '9')
				{
					val = val * 10 + (*p - '0');
				}
				else
				{
					break;
				}
				++p;
			}
			return p;
		}

		static const char * ParseSimpleIntEx(const char * begin, const char * end, int& val)
		{
			val = 0;
			int rate = 1;
			const char * p = begin;
			if (end > p+1 &&(*p == '-' || *p == '+'))
			{
				rate = *p == '-' ? -1 : 1;
				p += 1;
			}
			p = ParseSimpleIntEx(p, end,val);
			val *= rate;
			return p;
		}
	};
}


template<class T>
std::string ValueToStr(T _value)
{
	std::stringstream result;
	result << _value;
	return result.str();
}



static bool _CheckFloatFormat(const char * format)
{
	int len = 0;
	bool after_dot = false;
	for (int i = 0;; ++i)
	{
		wchar_t c = format[i];
		if (c == '\0') break;
		if (c == L'.')
		{
			if (after_dot) return false;
			after_dot = true;
		}
		else if (c >= L'0' && c <= L'9')
		{
			//nothing to do
		}
		else
		{
			return false;
		}
		++len;
		if (len > 5) return false;
	}
	return true;
}

//格式化float
//format 格式：  04.2   -> 01.23
//                4.2   -> _1.23
//                3     -> 1.230000
//                 .2   -> 1.23
//                 .5   -> 1.23400
#define _DEFAULT_FORMAT "\0"      
static
std::string FormatFloat(float val, const char * format)
{
	if (format == NULL)
	{
		format = _DEFAULT_FORMAT;
	}
	else if (!_CheckFloatFormat(format))
	{
		format = _DEFAULT_FORMAT;
	}
	char real_format[128];
	real_format[0] = '%';
	for (int i = 0;; ++i) {
		if (format[i] == '\0')
		{
			real_format[i + 1] = 'f';
			real_format[i + 2] = '\0';
			break;
		}
		real_format[i + 1] = format[i];
	}
	char buf[64] = { 0 };
	sprintf_s(buf, 64, real_format, val);
	return std::string(buf);
}
std::string FormatCoding(const std::vector<std::string>& lines, int indent = 0)
{
#define MAKE_INDENT()	i = indent;\
while (i > 0){ codeStr += '\t'; i--;}
	std::string codeStr;
	std::vector < std::string >::const_iterator itr;
	int i = 0;
	for (itr = lines.begin(); itr != lines.end(); itr++)
	{
		codeStr += "\r\n";
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

std::string ConfigReader::mTmpStr;
ConfigData ConfigReader::skillData;
TemplateData ConfigReader::skillTemplate;
ConfigReader * ConfigReader::instance = nullptr;
std::string ConfigReader::confingPath = "";
std::string ConfigReader::skillFilePath = "";
ConfigReader::ConfigReader()
{
	InitConfigData();
	InitTemplateCode();
}
ConfigReader::~ConfigReader()
{

}

void ConfigReader::InitConfigData()
{
	if (skillData.empty())
	{
		std::string path = GetConfigPath();
		path +="SkillConfig.txt";
		ParseResource(path.c_str(), [&](const char* section, const char* left, const char* right)->bool {
			ConfigItem* item = new ConfigItem();
			item->section = section;
			item->left = left;
			item->right = right;
			skillData.push_back(item);
			return true;
		});
		if (skillData.empty())
			CLTrace::Instance()->TraceErr("技能描述文件 %s 初始化失败，检查后再试！", path.c_str());
	}
}
void ConfigReader::InitTemplateCode()
{
	if (skillTemplate.empty())
	{
		std::string path = GetConfigPath();
		path += "SkillTemplate.txt";
		int fp = 0;
		if ((fp = _open(path.c_str(), O_BINARY | O_RDWR)) < 0)
		{
			CLTrace::Instance()->TraceErr("技能模板文件 %s 打开失败，检查后再试！", path.c_str());
			return;
		}
		long len = _filelength(fp);
		if (len <= 0) return;
		char *data = (char*)malloc(len * sizeof(char));
		int n = _read(fp, data, len);
		_close(fp);
 		if(n < len)
		memset(data + n, 0, len - n);
		ParseCodeBlock(data, [&](const char* section, const char* type, std::vector<std::string>& lines)->bool {
			TemplateCodeItem* item = new TemplateCodeItem();
			item->section = section;
			item->type = type;
			item->lines = lines;
			skillTemplate.push_back(item);
			return true;
		});
		if (skillTemplate.empty())
			CLTrace::Instance()->TraceErr("技能模板文件 %s 初始化失败，检查后再试！", path.c_str());
	}
}
std::string ConfigReader::StrReplace(const std::string& orignStr, const std::string& oldStr, const std::string& newStr)
{
	size_t pos = 0;
	std::string tempStr = orignStr;
	std::string::size_type newStrLen = newStr.length();
	std::string::size_type oldStrLen = oldStr.length();
	while (true)
	{
		pos = tempStr.find(oldStr, pos);
		if (pos == std::string::npos) break;
		tempStr.replace(pos, oldStrLen, newStr);
		pos += newStrLen;
	}
	return tempStr;
}
const char * ConfigReader::GetValue(ConfigData& configData, const char * typeName, const char * propertyName)
{
	for (auto it = configData.begin(); it != configData.end(); it++)
	{
		auto item = *it;
		auto s1 = item->section.c_str();
		auto s2 = item->left.c_str();
		if (std::strcmp(item->section.c_str(), typeName) == 0 && std::strcmp(item->left.c_str(), propertyName) == 0)
		{
			lastValueNull = false;
			return item->right.c_str();
		}
	}
	lastValueNull = true;
	return STR_MISSING_INFO;
}

std::string ConfigReader::GetCodeValue(TemplateData& templateData, const char * templateType, const char * fileType)
{
	std::vector<std::string> ret;
	for (auto it = templateData.begin(); it != templateData.end(); it++)
	{
		auto item = *it;
		auto s1 = item->section.c_str();
		auto s2 = item->type.c_str();
		if (std::strcmp(item->section.c_str(), templateType) == 0 && std::strcmp(item->type.c_str(), fileType) == 0)
		{
			lastValueNull = false;
			return FormatCoding(item->lines);
		}
	}
	lastValueNull = true;
	return STR_MISSING_INFO;
}


template<class T, class U>
static std::string _StrReplaceByVar(const std::string& orignStr, T GetAttr, U GetAttrEx)
{
	std::string replacedStr;
	replacedStr.reserve(orignStr.size());
	std::string varName, formatString;
	bool inVarName = false;
	for (size_t i = 0; i < orignStr.size(); ++i)
	{
		char c = orignStr[i];
		switch (c)
		{
		case '[':
			if (!inVarName)
			{
				inVarName = true;
				varName.clear();
			}
			break;
		case ']':
			if (inVarName)
			{
				inVarName = false;
				char buf[1280];
				buf[0] = 0;
				if (GetAttrEx(varName.c_str(), buf, 1280))
				{
					replacedStr += buf;
				}
				else
				{
					//old method
					size_t pipe_idx = varName.find('|');
					if (pipe_idx != std::string::npos)
					{
						formatString.assign(varName.begin() + pipe_idx + 1, varName.end());
						varName.resize(pipe_idx);
					}
					else
					{
						formatString = _DEFAULT_FORMAT;
					}

					float value = (float)GetAttr(varName.c_str());
					if (isnan(value))
					{
						replacedStr += '[';
						replacedStr += varName;
						replacedStr += ']';
					}
					else
					{
						replacedStr += FormatFloat(value, formatString.c_str());//ValueToStr(value);
					}
				}

			}
			else
			{
				replacedStr += c;
			}
			break;
		default:
			if (inVarName) varName += c;
			else replacedStr += c;
			break;
		}
	}
	return replacedStr;
}

template<class T>
static std::string _StrReplaceByVar(const std::string& orignStr, T GetAttr)
{
	return _StrReplaceByVar(orignStr, GetAttr, [](...) {return false; });
}

template<class T>
static std::string _StrReplaceByStr(const std::string& orignStr, T GetAttr)
{
	std::string replacedStr;
	replacedStr.reserve(orignStr.size());
	std::string varName;
	bool inVarName = false;
	for (size_t i = 0; i < orignStr.size(); ++i)
	{
		char c = orignStr[i];
		switch (c)
		{
		case '$':
			if (!inVarName)
			{
				inVarName = true;
				varName.clear();
			}
			else
			{
				inVarName = false;
				const char* value = (const char*)GetAttr(varName.c_str());
				if (value == nullptr || strlen(value) == 0)
				{
					replacedStr += '$';
					replacedStr += varName;
					replacedStr += '$';
				}
				else
				{
					replacedStr += value;
				}
			}
			break;
		default:
			if (inVarName) varName += c;
			else replacedStr += c;
			break;
		}
	}
	return replacedStr;
}

std::string ConfigReader::TryCreate(const std::string& src,const char* skillName, std::function<void(const std::vector<std::string>& items)> func)
{
	std::string orignStr = src;
	std::vector < std::string > exceptlist;
	std::vector <std::string> attributelist;
	exceptlist.push_back("物理攻击");
	exceptlist.push_back("移动速度");
	exceptlist.push_back("法术强度");
	exceptlist.push_back("小兵击杀数");
	exceptlist.push_back("英雄击杀数");
	exceptlist.push_back("英雄等级");
	exceptlist.push_back("最大生命值");
	exceptlist.push_back("最大魔法值");
	exceptlist.push_back("物理护甲");
	exceptlist.push_back("法术抗性");
	exceptlist.push_back("当前生命值");
	exceptlist.push_back("当前魔法值");
	exceptlist.push_back("CD");
	exceptlist.push_back("CD1");
	exceptlist.push_back("CD2");
	exceptlist.push_back("MP");
	exceptlist.push_back("MP1");
	exceptlist.push_back("MP2");
	exceptlist.push_back("n");
	exceptlist.push_back("autospace");
	exceptlist.push_back("FFFFFF");
	exceptlist.push_back("AAAAFF");
	exceptlist.push_back("FFAA00");
	exceptlist.push_back("00FF00");
	exceptlist.push_back("FF0000");
	exceptlist.push_back("ffffff");
	exceptlist.push_back("aaaaff");
	exceptlist.push_back("ffaa00");
	exceptlist.push_back("00ff00");
	exceptlist.push_back("ff0000");
	exceptlist.push_back("Lv0");
	exceptlist.push_back("Lv1");
	exceptlist.push_back("Lv2");
	exceptlist.push_back("Lv3");
	exceptlist.push_back("Lv4");
	exceptlist.push_back("Lv5");
	exceptlist.push_back("/Lv");
	exceptlist.push_back("key");
	if (skillName != NULL)
	{

		orignStr = _StrReplaceByVar(orignStr, [&](const char * attrName) {
			if(std::find(exceptlist.begin(),exceptlist.end(),attrName) != exceptlist.end()) return std::numeric_limits<float>::quiet_NaN();
			//std::cout<< attrName << std::endl;
			attributelist.push_back(attrName);
			return std::numeric_limits<float>::quiet_NaN();
		//	//return skill->GetAttribute(CW2A(attrName));
		});
		if (!attributelist.empty())
			func(attributelist);
	}

	return orignStr;
}

std::string ConfigReader::TryCreate(const std::string& src, const char* skillName)
{
	return TryCreate(src, skillName, [](...) {});
}

bool ConfigReader::TryGetSkillValue(const char* skillName, const char * sProperty, bool encoding, const char** outNoEncodeValue)
{
	std::string sValue;
	bool bResult = false;
	if (skillName == NULL)
	{
		sValue = "技能为空";
		bResult = false;
	}
	else
	{
		if (outNoEncodeValue)
		{
			sValue = *outNoEncodeValue = GetValue(skillData, skillName, sProperty);
		}
		else
		{
			sValue = GetValue(skillData, skillName, sProperty);
		}
		if (encoding)
			sValue = TryCreate(sValue, skillName);
		bResult = true;
	}


	mTmpStr = std::move(sValue);

	return bResult;
}

const char* ConfigReader::GetSkillCNName(const char * skillName)
{
	const char* ret = NULL;
	TryGetSkillValue(skillName, "名称Ex", false, &ret);
	if (lastValueNull)
	{
		TryGetSkillValue(skillName, "名称", false, &ret);
	}
	return ret;
}

const char* ConfigReader::GetSkillDetailDesc(const char * skillName)
{
	TryGetSkillValue(skillName, "详细描述Ex", true);
	if (lastValueNull)
	{
		TryGetSkillValue(skillName, "详细描述", true);
	}
	return mTmpStr.c_str();
}

const char* ConfigReader::GetSkillAddPointDesc(const char * skillName)
{
	TryGetSkillValue(skillName, "加点描述Ex", true);
	if (lastValueNull)
	{
		TryGetSkillValue(skillName, "加点描述", true);
	}
	return mTmpStr.c_str();
}

bool ConfigReader::TryGetSkillCode(const char* tName, const char * fName, bool encoding, std::function<const char*(const char* tName)> func)
{
	std::string sValue;
	bool bResult = false;
	if (tName == NULL)
	{
		sValue = "技能模板为空";
		bResult = false;
	}
	else
	{
		sValue = GetCodeValue(skillTemplate, tName, fName);
		if (encoding)
		{
			sValue = _StrReplaceByStr(sValue, [&](const char * attrName) {
				return func(attrName);
			});
		}
		bResult = true;
	}
	mTmpStr = std::move(sValue);
	return bResult;
}

int WriteInFile(const char* fname, const char* data)
{
	//std::string path = ConfigReader::GetConfigPath();
	std::string path = ConfigReader::GetSkillFilePath();
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
			newData = std::string(temData,ret2);
			delete temData;
		}
		_close(fp);
	}
	
	newData += data;
	
	if ((fp = _open(path.c_str(), O_BINARY | O_RDWR | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE | _S_IEXEC)) < 0) {
		std::cout << "open file " << path << " failed " << fp << std::endl;
		CLTrace::Instance()->TraceLog("Error:生成文件 %s 失败，检查后再试！", path.c_str());
		return ret;
	}
	
	ret = _write(fp, newData.c_str(), newData.length());
	CLTrace::Instance()->TraceLog("write in file %s the data of size %d that expect %d + %d", fname, ret, ret2, strlen(data));
	if(ret > 0)
		std::cout << "make skill file ok!->" << path.c_str() << std::endl;
	_close(fp);
	return ret;
}

bool ConfigReader::MakeSkillFile(const char* fileName)
{
	std::map<std::string, std::vector<std::pair<std::string,std::string>>> data;
	std::function<bool(const char * section, const char *left, const char * right)> parseConfig = [&](const char * section, const char * left, const char * right)->bool {
		data[section].push_back(std::make_pair(left, right));
		return true;
	};
	std::string fpath = fileName;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		fpath = GetConfigPath() + fpath;
	}
	if (!ParseIniFile(fpath.c_str(), parseConfig))
	{
		CLTrace::Instance()->TraceLog("Error:生成技能文件的配制 %s 解析失败，检查后再试！", fpath.c_str());
		return false;
	}
	std::string hCode;
	std::string cppCode;
	int ret = 0;
	for (std::map<std::string, std::vector<std::pair<std::string, std::string>>>::iterator it = data.begin(); it != data.end(); it++)
	{
		hCode.clear();
		cppCode.clear();
		for (std::vector<std::pair<std::string, std::string>>::iterator vit = it->second.begin(); vit != it->second.end(); vit++)
		{
			 TryGetSkillCode((*vit).first.c_str(), "Header", true, [&](const char* cName)->const char* {
				return (*vit).second.c_str();
			});
			 hCode += mTmpStr;
			TryGetSkillCode((*vit).first.c_str(), "Source", true, [&](const char* cName)->const char* {
				return (*vit).second.c_str();
			});
			cppCode += mTmpStr;
		}
		ret = WriteInFile((it->first + ".h").c_str(), hCode.c_str());
		ret = WriteInFile((it->first + ".cpp").c_str(), cppCode.c_str());
	}
	return true;
}

bool ConfigReader::ParseSkillRecipesFromJson(const Json::Value& obj, std::map<std::string, std::vector<CSkillData>>& data)
{
	if (obj.empty() || !obj.isObject())
	{
		CLTrace::Instance()->TraceLog("英雄技能解析失败，配制文件格式不符！");
		return false;
	}
	Value::Members heros = obj.getMemberNames();
	//hero
	for (size_t i = 0; i < heros.size(); i++)
	{
		std::vector<CSkillData> skills;
		const char* name = heros[i].c_str();
		if (data.find(name) != data.end())
		{
			CLTrace::Instance()->TraceLog("%s 英雄技能重复设置！", name);
			continue;//重复设置
		}
		Value skillArr = obj[name];
		if (skillArr.empty() || !skillArr.isArray()) {
			CLTrace::Instance()->TraceLog("%s 技能列表为空或非数组！",name);
			continue;//技能列表为空或非数组
		}
		//skill/file(q,w,e,r,...)
		for (size_t j = 0; j < skillArr.size(); j++)
		{
			if (skillArr[j].empty() || !skillArr[j].isObject()) {
				CLTrace::Instance()->TraceLog("%s [%d] 技能属性为空或非对象！", name, j);
				continue;//技能属性为空或非对象
			}
			CSkillData s;
			Value v_skillID = skillArr[j]["skillID"];
			Value v_file = skillArr[j]["file"];
			Value v_name = skillArr[j]["name"];
			Value v_cname = skillArr[j]["cname"];
			Value v_recipes = skillArr[j]["recipes"];
			if (v_recipes.empty() || !v_recipes.isArray()) {
				CLTrace::Instance()->TraceLog("%s [%d][\"recipes\"] 技能组成为空或非数组！", name, j);
				continue;//技能组成为空或非数组
			}
			if (v_name.empty() || v_name.type() != Json::stringValue) {
				CLTrace::Instance()->TraceLog("%s [%d][\"name\"] 类名为空或非字符串！", name, j);
				continue;//类名为空
			}
			if (v_file.empty() || v_file.type() != Json::stringValue) {
				CLTrace::Instance()->TraceLog("%s [%d][\"file\"] 文件名为空或非字符串！", name, j);
				continue;//文件名为空
			}

			s.fileName = v_file.asString();
			s.skillName = v_name.asString();
			if (!v_skillID.empty() && (v_skillID.type() == Json::intValue || v_skillID.type() == Json::uintValue))
				s.skillID = v_skillID.asInt();
			if (!v_cname.empty() && v_cname.type() == Json::stringValue)
				s.skillCName = v_cname.asString();
			else
				s.skillCName = s.skillName;
			//class(skill,buff,bullet,...)
			for (size_t k = 0; k < v_recipes.size(); k++)
			{
				if (v_recipes[k].empty() || !v_recipes[k].isObject()) {
					CLTrace::Instance()->TraceLog("%s [%d][\"recipes\"][%d] 模板设置为空或非对象！", name, j, k);
					continue;//模板设置为空
				}
				CTemplateData  c;
				Value v_type = v_recipes[k]["type"];
				Value v_template = v_recipes[k]["template"];
				Value v_className = v_recipes[k]["name"];
				Value v_classCName = v_recipes[k]["cname"];
				if (v_template.empty() || v_template.type() != Json::stringValue)
				{
					CLTrace::Instance()->TraceLog("%s [%d][\"recipes\"][%d][\"template\"] 模板名为空或非字符串！", name, j, k);
					continue;//模板名为空或非字符串
				}
				if (v_className.empty() || v_className.type() != Json::stringValue)
				{
					CLTrace::Instance()->TraceLog("%s [%d][\"recipes\"][%d][\"name\"] 模板设置为空或非对象！", name, j, k);
					continue;//类名为空或非字符串
				}
				c.templateName = v_template.asString();
				c.className = v_className.asString();
				if (!v_type.empty() && (v_type.type() == Json::intValue || v_type.type() == Json::uintValue))
					c.type = v_type.asInt();
				if (!v_classCName.empty() && v_classCName.type() == Json::stringValue)
					c.classCName = v_classCName.asString();
				else if (c.IsSkill() && !s.skillCName.empty())
					c.classCName = s.skillCName;
				else
					c.classCName = c.className;
				s.skillRecipes.push_back(c);
			}
			skills.push_back(s);
		}
		data[name] = skills;
		CLTrace::Instance()->TraceLog("%s 英雄技能解析完毕！", name);
	}
	return true;
}
std::string ModifyIncludeCode(const std::string& context)
{
	std::string replacedStr;
	replacedStr.reserve(context.size());
	std::vector<std::string> includes;
	std::vector<std::string> includes2;
	std::string varName, varName2;
	bool inVarName = false;
	bool inVarName2 = false;
	size_t offset = 0;
	size_t i = 0;
	while (i < context.size() && (offset = context.find("#include",i)) != std::string::npos)
	{
		replacedStr += context.substr(i, offset - i);
		for ( i= offset + 8; i < context.size() && context[i] != '\n'; i++)
		{
			char c = context[i];
			switch (c)
			{
			case '<':
				if (!inVarName)
				{
					inVarName = true;
					varName.clear();
				}
				break;
			case '>':
				if (inVarName)
				{
					inVarName = false;
					if (std::find(includes.begin(), includes.end(), varName) == includes.end())
						includes.push_back(varName);
				}
				break;
			case '"':
				if (!inVarName2)
				{
					inVarName2 = true;
					varName2.clear();
				}
				else {
					inVarName2 = false;
					if (std::find(includes2.begin(), includes2.end(), varName2) == includes2.end())
						includes2.push_back(varName2);
				}
				break;
			default:
				if (inVarName) varName += c;
				if (inVarName2) varName2 += c;
				break;
			}
		}
		if (context[i] == '\n') i += 1;
	}
	if (i < context.size()) replacedStr += context.substr(i);
	std::string includesCode;
	for (std::vector<std::string>::iterator itr = includes.begin(); itr != includes.end(); itr++)
	{
		includesCode += "#include <" + (*itr) + ">\r\n";
	}
	for (std::vector<std::string>::iterator itr = includes2.begin(); itr != includes2.end(); itr++)
	{
		includesCode += "#include \"" + (*itr) + "\"\r\n";
	}
	return includesCode + replacedStr;
}
bool ConfigReader::MakeSkillFileEx(const char* fileName)
{
	std::map<std::string, std::vector<CSkillData>> data;
	std::function<bool(const char * heroName, const std::vector<CSkillData>& skills)> parseConfig = [&](const char * heroName, const std::vector<CSkillData>& skills)->bool {
		data[heroName] = skills;
		return true;
	};
	std::string fpath = fileName;
	if (fpath.find(':') == std::string::npos)
	{
		while (!fpath.empty() && (fpath.front() == '\\' || fpath.front() == '.' || fpath.front() == '/'))	fpath = fpath.substr(1);
		fpath = GetConfigPath() + fpath;
	}
	Json::Reader reader;
	Json::Value root;
	std::ifstream fin;
	fin.open(fpath.c_str(), std::ios::binary);
	bool ret = false;
	if (!fin)
	{
		CLTrace::Instance()->TraceErr("配制文件 %s 打开失败，确定文件存在后再试！", fpath.c_str());
		fin.close();
		return ret;
	}
	CLTrace::Instance()->TraceLog("开始解析英雄技能配制文件 %s 。。。",fpath.c_str());
	if (reader.parse(fin, root, false))
	{
		ret = ParseSkillRecipesFromJson(root, data);
	}
	else
	{
		CLTrace::Instance()->TraceErr("配制文件 %s 解析失败，确认为json格式后再试！\r\ndetail: %s",fpath.c_str(), reader.getFormatedErrorMessages().c_str());
	}
	CLTrace::Instance()->TraceLog("解析英雄技能配制文件 %s %s。。。", fpath.c_str(),ret?"成功":"失败");
	fin.close();
	if (!ret) return ret;

	CLTrace::Instance()->TraceLog("开始生成技能。。。");
	std::map<std::string,std::pair<std::string,std::string>> createdFiles;
	std::string hCode;
	std::string cppCode;
	std::string configFile;
	
	for (std::map<std::string, std::vector<CSkillData>>::iterator hItr = data.begin(); hItr != data.end(); hItr++)
	{
		std::vector<CSkillData> skills = hItr->second;
		std::string heroName = hItr->first;
		for (std::vector<CSkillData>::iterator sItr = skills.begin(); sItr != skills.end(); sItr++)
		{
			std::string fileName = sItr->fileName;
			std::vector<CTemplateData> recipes= sItr->skillRecipes;
			for (std::vector<CTemplateData>::iterator tItr = recipes.begin(); tItr != recipes.end(); tItr++)
			{
				hCode.clear();
				cppCode.clear();
				if (tItr->IsConfig() && configFile.empty())
					configFile = fileName;
				std::function<const char*(const char* tName)> func = ([&](const char* name)->const char* {
					if (std::strcmp(name, "HeroName") == 0)
						return hItr->first.c_str();
					else if (std::strcmp(name, "ClassName") == 0)
						return tItr->className.c_str();
					else if (std::strcmp(name, "ClassCName") == 0)
						return tItr->classCName.c_str();
					else if (std::strcmp(name, "SkillName") == 0)
						return sItr->skillName.c_str();
					else if (std::strcmp(name, "SkillCName") == 0)
						return sItr->skillCName.c_str();
					else if (std::strcmp(name, "SkillADLowerCase") == 0)
					{
						switch (sItr->skillID)
						{
						case 0: return "q";
						case 1: return "w";
						case 2:return "e";
						case 3:return "r";
						case -1:return "n";
						case 98:return "b";
						}
						return "q";
					}
					else if (std::strcmp(name, "SkillADUpperCase") == 0)
					{
						switch (sItr->skillID)
						{
						case 0: return "Q";
						case 1: return "W";
						case 2:return "E";
						case 3:return "R";
						case -1:return "N";
						case 98:return "B";
						}
						return "Q";
					}
					return tItr->className.c_str();
				});
				TryGetSkillCode(tItr->templateName.c_str(), "Header", true, func);
				hCode += mTmpStr;
				TryGetSkillCode(tItr->templateName.c_str(), "Source", true, func);
				cppCode += mTmpStr;
				if (tItr->IsSkill())
				{
					SkillAttr::ModifyOrAppendAttr(cppCode, tItr->className.c_str());
				}else if(tItr->IsBornBuff())
					SkillAttr::ModifyOrAppendAttr(cppCode, tItr->className.c_str(), (heroName + "_DefaultSkill").c_str());
				createdFiles[fileName].first += hCode;
				createdFiles[fileName].second += cppCode;
			}
		}
	}
	for (std::map<std::string, std::pair<std::string, std::string>>::iterator itr = createdFiles.begin(); itr != createdFiles.end(); itr++)
	{
		ret = ret && WriteInFile((itr->first + ".h").c_str(), ("#pragma once\r\n"+ModifyIncludeCode((itr->second).first)).c_str()) > 0;
		std::string appendInc = "\r\n#include \"" + itr->first + ".h\"";
		if(!configFile.empty() && configFile != itr->first)
			appendInc += "\r\n#include \"" + configFile + ".h\"";
		ret = ret && WriteInFile((itr->first + ".cpp").c_str(), ModifyIncludeCode((itr->second).second + appendInc).c_str()) > 0;
	}
	CLTrace::Instance()->TraceLog("生成技能完毕。。。");
	return ret;
}