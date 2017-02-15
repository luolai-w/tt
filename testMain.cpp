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

template<typename T>
void quicksort(T a[], int low, int high)
{
	int l = low;
	int h = high;
	if (l >= h) return;
	T tmp = a[l];
	while (l < h)
	{
		while (l < h && a[h] >= tmp) h--;
		a[l] = a[h];
		while (l < h && a[l] < tmp) l++;
		a[h] = a[l];
	}
	a[l] = tmp;
	if (l - 1 > low) quicksort(a,low, l - 1);
	if (l + 1 < high) quicksort(a,l + 1, high);
}

#include <sstream>
std::string reverseString(std::string s) {
	std::stringstream sstr;
	if (s.empty())return "";
	std::string::const_iterator itr = s.end();
	itr--;
	for (; itr != s.begin(); itr--)
		sstr<< *itr;
	sstr<<*s.begin();
	return sstr.str();
}

char findTheDifference(std::string s, std::string t) {
	unsigned i = 0;
	char r = 0;
	for (; i < t.size(); i++)
	{
		r ^= t[i];
	}
	for (i = 0; i < s.size(); i++)
	{
		r ^= s[i];
	}
	return r;
}

template<typename T>
struct BinaryTreeNode {
	T value;
	struct BinaryTreeNode * left;
	struct BinaryTreeNode * right;
	BinaryTreeNode(T v) :value(v), left(nullptr), right(nullptr) {}
	BinaryTreeNode() :value((T)0), left(nullptr), right(nullptr) {}
};

template<typename T>
struct BinaryTreeNode<T>* ConstructBTNode(T* preStart, T* preEnd, T* midStart, T* midEnd)
{
	struct BinaryTreeNode<T> * node = (struct BinaryTreeNode<T>*)malloc(sizeof(struct BinaryTreeNode<T>));
	node->value = *preStart;
	node->left = nullptr;
	node->right = nullptr;
	if (preStart == preEnd) {
		if (midStart == midEnd && *midStart == *preStart)
			return node;
		else
		{
			throw std::exception("invalid input!");
			return node;
		}
	}
	T* rootNode = midStart;
	while (rootNode < midEnd)
	{
		if (*rootNode == node->value) break;
		rootNode++;
	}
	if (*rootNode != node->value)
	{
		throw std::exception("invalid input!");
		return node;
	}
	int l_len = rootNode - midStart;
	if (l_len > 0)
		node->left = ConstructBTNode(preStart + 1, preStart + l_len, midStart, midStart + l_len - 1);
	if (midEnd - rootNode > 0)
		node->right = ConstructBTNode(preStart + 1 + l_len, preEnd, midStart + l_len+1, midEnd);
	return node;
}

template<typename T>
struct BinaryTreeNode<T>* CreateBTree(T pre[],T middle[], int length)
{
	if (pre == nullptr || middle == nullptr || length <= 0) return nullptr;
	return ConstructBTNode(pre, pre + length - 1, middle, middle + length - 1);
}

#include <queue>
template<typename T>
void BTDepthFS(struct BinaryTreeNode<T>* bt, std::vector<T>& path)
{
	if (bt == nullptr) return;
	std::queue<struct BinaryTreeNode<T>*> q;
	if(bt != nullptr)
	q.push(bt);
	while (!q.empty())
	{
		unsigned len = q.size();
		for (unsigned i = 0; i < len; i++)
		{
			struct BinaryTreeNode<T>* node = q.front();
			q.pop();
			path.push_back(node->value);
			if (node->left)
				q.push(node->left);
			if (node->right)
				q.push(node->right);
		}
	}
}

template<typename T>
void BTPreOrder(struct BinaryTreeNode<T>* bt, std::vector<T>& path)
{
	if (bt == nullptr) return;
	path.push_back(bt->value);
	if (bt->left)
		BTPreOrder(bt->left,path);
	if (bt->right)
		BTPreOrder(bt->right,path);
}

template<typename T>
void BTInOrder(struct BinaryTreeNode<T>* bt, std::vector<T>& path)
{
	if (bt == nullptr) return;
	std::stack<struct BinaryTreeNode<T>*> s;
	if (bt != nullptr)
		s.push(bt);
	std::stack<struct BinaryTreeNode<T>*>::value_type  p = s.top();
	s.pop();
	while (!s.empty() || p != nullptr)
	{
		while (p != nullptr)
		{
			s.push(p);
			p = p->left;
		}
		if (!s.empty())
		{
			p = s.top();
			s.pop();
			path.push_back(p->value);
			p = p->right;
		}
	}
}

template<typename T>
void BTPostOrder(struct BinaryTreeNode<T>* bt, std::vector<T>& path)
{
	if (bt == nullptr) return;
	std::stack<std::pair<struct BinaryTreeNode<T>*, bool>> s;
	if (bt != nullptr)
		s.push(std::make_pair(bt, false));
	while (!s.empty())
	{
		std::stack<std::pair<struct BinaryTreeNode<T>*, bool>>::value_type  p = s.top();
		s.pop();
		if (p.second)
			path.push_back((p.first)->value);
		else
		{
			s.push(std::make_pair(p.first, true));
			if ((p.first)->right)
				s.push(std::make_pair((p.first)->right, false));
			if ((p.first)->left)
				s.push(std::make_pair((p.first)->left, false));
		}
	}
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
	int number = 0x80000001;
	unsigned int uNum = (unsigned int)number;
	int number2 = 0xFFFFFFFF;
	unsigned int uNum2 = (unsigned int)number;
	int number3 = -1;
	unsigned int uNum3 = (unsigned int)number3;
	int number4 = -0x7FFFFFFF;
	unsigned int uNum4 = (unsigned int)number4;
	
	
	std::cout << number << "->" << (uNum & ~(1<<31))<< std::endl;
	std::cout << number2 << "->" <<( uNum2 & ~(1 << 31) )<< std::endl;
	std::cout << number3 << "->" << uNum3 << std::endl;
	std::cout << number4 << "->" << (uNum4 & ~(1 << 31)) << std::endl;


	typedef struct bb
	{
		
		char c;             //[0]....[0]		有效对齐1
		int id;             //[4]....[7]		有效对齐4
		
		int id2;			//[8]....[11]		有效对齐4
		double weight;      //[16].....[23]　　　　　　原则１ 有效对齐8
		//float height;      //[16]..[19],总长要为８的整数倍,补齐[20]...[23]　　　　　原则３
		char c2;			//[24]....[24]		有效对齐 1
	}BB;		//[0]....[31] 整体有效对齐 8

	std::cout << "BB: " << sizeof(BB) << std::endl;
	int a1[] = { 11,22,13,34,25,6 };
	int a2[] = { 1,2,3,4,5,6 };
	int a3[] = { 61,52,43,34,25,16 };
	quicksort(a1, 0, 5);
	quicksort(a2, 0, 5);
	quicksort(a3, 0, 5);

	std::cout << findTheDifference("abcd","abcfdegh") << std::endl;

	int a[] = { 1,2,3,4,5,6 };
	int *p;
	int *c;
	p = &a[1];
	c = p - 1;
	std::cout << *p << "," << *c << std::endl;
	int d = 100;
	char* s = new char[d];

	{//binary tree
		int pre[] = { 1,2,4,7,6,8,3,5 };
		int mid[] = { 7,4,8,6,2,1,5,3 };
		struct BinaryTreeNode<int>* bTree = CreateBTree(pre, mid, 8);
		std::vector<int> prePath;
		std::vector<int> inPath;
		std::vector<int> postPath;
		std::vector<int> depthPath;
		BTPreOrder(bTree, prePath);
		BTInOrder(bTree, inPath);
		BTPostOrder(bTree, postPath);
		BTDepthFS(bTree, depthPath);

		std::cout << "pre-order: ";
		for (unsigned i = 0; i < prePath.size(); i++)
			std::cout << prePath[i] << ",";
		std::cout << "\nin-order: ";
		for (unsigned i = 0; i < inPath.size(); i++)
			std::cout << inPath[i] << ",";
		std::cout << "\npost-order: ";
		for (unsigned i = 0; i < postPath.size(); i++)
			std::cout << postPath[i] << ",";
		std::cout << "\ndepth-first: ";
		for (unsigned i = 0; i < depthPath.size(); i++)
			std::cout << depthPath[i] << ",";
	}
	

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