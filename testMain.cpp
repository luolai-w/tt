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

#define FALSE 0
#define TRUE 1
#define OK 1
#define ERROR 1
#define OVERFLOW -1
#define INIT_SIZE 10
#define INCREMENT_SIZE 5
typedef int Status;
typedef int ElementType;

int Compare(const ElementType& e1, const ElementType& e2)
{
	if (e1 == e2) return 0;
	else if (e1 > e2) return 1;
	else return -1;
}

namespace sequence_list {
	typedef struct {
		ElementType* data;
		int length;
		int size;
		bool bInited;
	}SeqList;

	Status DestroyList(SeqList& L)
	{
		if (!L.bInited) return ERROR;
		free(L.data);
		L.bInited = false;
		L.size = 0;
		L.length = 0;
		return OK;
	}
	Status InitList(SeqList& L)
	{
		DestroyList(L);
		L.data = (ElementType*)malloc(INIT_SIZE * sizeof(ElementType));
		if (L.data == nullptr) return ERROR;
		L.length = 0;
		L.size = INIT_SIZE;
		L.bInited = TRUE;
	}
	Status ClearList(SeqList& L)
	{
		if (!L.bInited) return ERROR;
		L.length = 0;
		L.size = 0;
		return OK;
	}
	bool IsEmpty(const SeqList& L) {
		if (!L.bInited) return true;
		return L.length > 0;
	}
	size_t GetLength(const SeqList& L)
	{
		if (!L.bInited) return 0;
		return L.length;
	}
	Status GetElement(const SeqList& L, int index, ElementType& e)
	{
		if (!L.bInited) return ERROR;
		if (index < 1 || index > L.length) return ERROR;
		e = *(L.data + index);
		return OK;
	}
	Status InsertElement(SeqList& L, int index, const ElementType& e)
	{
		if (!L.bInited) return ERROR;
		if (index < 1 || index > L.length + 1) return ERROR;
		if (L.length >= L.size)
		{
			ElementType* newPtr = (ElementType*)realloc(L.data, INCREMENT_SIZE * sizeof(ElementType));
			if (newPtr == nullptr) return ERROR;
			L.data = newPtr;
			L.size += INCREMENT_SIZE;
		}
		int j = L.length;
		while (j >= index)
		{
			*(L.data + j) = *(L.data + j - 1);
			j--;
		}
		*(L.data + j) = e;
		L.length += 1;
		return OK;
	}
	int FindElement(const SeqList& L, const ElementType& e, int (*compare)(const ElementType&,const ElementType&))
	{
		if (!L.bInited) return OVERFLOW;
		for (int i = 0; i < L.length; i++)
		{
			if (!compare(*(L.data + i), e)) return i+1;
		}
		return OVERFLOW;
	}
	Status DeleteElement(SeqList& L, const ElementType& e)
	{
		if (!L.bInited) return ERROR;
		int index = FindElement(L, e, Compare);
		if (index == OVERFLOW) return ERROR;
		for (int i = index - 1; i < L.length - 1; i++)
		{
			*(L.data + i) = *(L.data + i + 1);
		}
		L.length -= 1;
		return OK;
	}
	Status PreElement(const SeqList& L, const ElementType& curr, ElementType& e)
	{
		if (!L.bInited) return ERROR;
		int index = FindElement(L, curr, Compare);
		if (index == OVERFLOW) return ERROR;
		if (index <= 1 || index > L.length) return ERROR;
		e = *(L.data + (index - 1) -1);
		return OK;
	}
	Status NextElement(const SeqList& L, const ElementType& curr, ElementType& e)
	{
		if (!L.bInited) return ERROR;
		int index = FindElement(L, curr, Compare);
		if (index == OVERFLOW) return ERROR;
		if (index < 1 || index >= L.length) return ERROR;
		e = *(L.data + (index -1) + 1);
		return OK;
	}
};
namespace link_list{
	typedef struct LNode {
		ElementType data;
		struct LNode* next;
	}LNode, *LinkList;

	Status DestroyList(LinkList* L)
	{
		if (!L) return ERROR;
		LinkList p = *L;
		while (p)
		{
			LinkList q = p;
			p = p->next;
			free(q);
		}
		*L = nullptr;
		return OK;
	}
	Status InitList(LinkList* L)
	{
		if (L == nullptr) return ERROR;
		DestroyList(L);
		LinkList p = (LinkList)malloc(sizeof(LNode));
		p->next = nullptr;
		*L = p;
		return OK;
	}
	Status ClearList(LinkList L)
	{
		if (!L) return ERROR;
		if(L->next)
		DestroyList(&(L->next));
		L->next = nullptr;
		return OK;
	}
	bool IsEmpty(LinkList L)
	{
		if (!L) return true;
		return L->next != nullptr;
	}
	size_t GetLength(LinkList L)
	{
		if (!L) return ERROR;
		LinkList p = L->next;
		size_t i = 0;
		while (p)
		{
			i++;
			p = p->next;
		}
		return i;
	}
	Status GetElement(LinkList L, int index, ElementType& e)
	{
		if (!L || index < 1) return ERROR;
		int i = 0;
		LinkList p = L->next;
		while (p)
		{
			if (++i == index) {
				e = p->data;
				return OK;
			}
			p = p->next;
		}
		return ERROR;
	}
	Status InsertElement(LinkList L, int index, const ElementType& e)
	{
		if (!L) return ERROR;
		LinkList q = (LinkList)malloc(sizeof(LNode));
		q->data = e;
		q->next = nullptr;
		if (!q) return ERROR;
		LinkList p = L;
		int i = 0;
		while (p)
		{
			if (++i == index)
			{
				q->next = p->next;
				p->next = q;
				return OK;
			}
			p = p->next;
		}
		return OK;
	}
	Status DeleteElement(LinkList L, const ElementType& e)
	{
		if (!L) return ERROR;
		LinkList p = L;
		while (p)
		{
			LinkList q = p->next;
			if (q->data == e)
			{
				p->next = q->next;
				free(q);
				return OK;
			}
			p = p->next;
		}
		return ERROR;
	}
	int FindElement(LinkList L, const ElementType& e, int (*compare)(const ElementType&,const ElementType&))
	{
		if (!L) return OVERFLOW;
		LinkList p = L->next;
		int i = 0;
		while (p)
		{
			i += 1;
			if (p->data == e) return i;
			p = p->next;
		}
		return OVERFLOW;
	}
	Status PreElement(LinkList L, const ElementType& curr, ElementType& e)
	{
		if (!L) return ERROR;
		LinkList p = L->next;
		if (p->data == curr) return ERROR;
		while (p->next)
		{
			LinkList q = p->next;
			if (q->data == curr)
			{
				e = p->data;
				return OK;
			}
			p = q;
		}
		return ERROR;
	}
	Status NextElement(LinkList L, const ElementType& curr, ElementType& e)
	{
		if (!L) return ERROR;
		LinkList p = L->next;
		while (p)
		{
			LinkList q = p->next;
			if (p->data == curr)
			{
				if (q != nullptr)
				{
					e = q->data;
					return OK;
				}
				return ERROR;
			}
			p = q;
		}
		return ERROR;
	}
};
/*
namespace sequence_stack {
	typedef struct {
		ElementType* base;
		ElementType* top;
		int size;
	}SeqStack;
	Status InitStack(SeqStack& S)
	{

	}
	Status DestroyStack(const SeqStack& S)
	{

	}
	Status ClearStack(const SeqStack& S)
	{

	}
	bool IsEmpty(const SeqStack& S)
	{

	}
	size_t GetLength(const SeqStack& S)
	{

	}
	Status Top(const SeqStack& S, ElementType& e)
	{

	}
	Status Push(SeqStack& S, const ElementType& e)
	{

	}
	Status Pop(SeqStack& S)
	{

	}
};
namespace link_queue {
	typedef struct QNode {
		ElementType* data;
		struct LNode* next;
	}QNode,*QNodePtr;
	typedef struct {
		QNode* front;
		QNode* tail;
	}LinkQueue;
	Status InitQueue(LinkQueue& Q)
	{

	}
	Status DestroyQueue(const LinkQueue& Q)
	{

	}
	Status ClearQueue(const LinkQueue& Q)
	{

	}
	bool IsEmpty(const LinkQueue& Q)
	{

	}
	size_t GetLength(const LinkQueue& Q)
	{

	}
	Status Front(const LinkQueue& Q)
	{

	}
	Status Push(LinkQueue& Q, const ElementType& e)
	{

	}
	Status Pop(LinkQueue& Q)
	{

	}
};
*/
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

template<typename T>
T* mergesort(T a[], int low, int high)
{
	if (low > high) return nullptr;
	int len = high - low + 1;
	T* tmp = (T*)malloc(sizeof(T)*(high - low + 1));
	if (low == high)
	{
		tmp[0] = a[low];
		return tmp;
	}
	int intvl = (high - low) / 2;
	int len1 = intvl + 1;
	int len2 = high - intvl - low;
	T* c1 = mergesort(a, low, low + intvl);
	T* c2 = mergesort(a, low + intvl + 1, high);
	int i = 0, j = 0, k = 0;
	while (i < len1 && j < len2)
	{
		if (c1[i] <= c2[j]) tmp[k++] = c1[i++];
		else tmp[k++] = c2[j++];
	}
	while (i < len1)	tmp[k++] = c1[i++];
	while (j < len2)	tmp[k++] = c2[j++];
	free((void*)c1);
	free((void*)c2);
	return tmp;
}

template<typename T>
void merge(T* a, int low, int mid, int high)
{
	int len = high - low + 1;
	if (len < 1) return;
	T* tmp = (T*)malloc(len * sizeof(T));
	int i = low, j = mid+1, k = 0;
	while (i <= mid && j <= high)
	{
		if (a[i] <= a[j]) tmp[k++] = a[i++];
		else tmp[k++] = a[j++];
	}
	while(i <= mid) tmp[k++] = a[i++];
	while (j <= high) tmp[k++] = a[j++];
	memcpy(a + low, tmp, len*sizeof(T));
	free(tmp);
}

template<typename T>
void mergesort2(T* a, int len)
{
	if (a == nullptr || len <= 1) return;
	for (int step = 1; step < len; step *= 2)
	{
		int i = 0;
		for (; i + 2 * step - 1 < len; i += 2 * step)
		{
			merge(a, i, i + step - 1, i + 2 * step - 1);
		}
		if (i < len && i + step < len)
			merge(a, i, i + step - 1, len - 1);
	}
}


template<typename T>
void insetsort(T a[], int len)
{
	for (int i = 1; i < len; i++)
	{
		int j = i - 1;
		T key = a[i];
		while (j >= 0 && a[j] > key)
		{
			a[j + 1] = a[j];
			j--;
		}
		a[j + 1] = key;
	}
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

template<typename T>
struct BinaryTreeNode<T>* CreateBTree(T arr[], unsigned length)
{
	if (arr == nullptr || length == 0) return nullptr;
	typedef BinaryTreeNode<T> BTNode;
	BTNode* root = nullptr;
	stack<std::pair<BTNode*,int>> s;
	if (arr[0] == -1) return root;
	BTNode* p = (BTNode*)malloc(sizeof(BTNode));
	p->value = arr[0];
	p->left = nullptr;
	p->right = nullptr;
	root = p;
	s.push(std::make_pair(p, 0));
	int i = 1;
	while (!s.empty() && i < length)
	{
		stack<pair<BTNode*, int>>::value_type e = s.top();
		s.pop();
		p = nullptr;
		if (arr[i] != -1)
		{
			p = (BTNode *)malloc(sizeof(BTNode));
			p->value = arr[i];
			p->left = nullptr;
			p->right = nullptr;
		}
		i++;
		if (e.second == 0) //left child
		{
			e.second += 1;
			e.first->left = p;
			s.push(e);
			if (p) s.push(std::make_pair(p,0));
		}
		else//right child
		{
			e.first->right = p;
			if (p) s.push(std::make_pair(p, 0));
		}
	}
	return root;
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

using namespace std;

vector<int> constructRectangle(int area) {
	vector<int> res;
	if (area <= 0) return res;
	int greater = area;
	int less = 1;
	int mid = less;
	while (greater - less > 1)
	{
		int intvl = greater - less;
		if (intvl % 2 == 0) mid = less + intvl / 2;
		else mid = less + (intvl - 1) / 2;
		if ((long long)mid*mid <= area) less = mid;
		if ((long long)mid*mid >= area) greater = mid;
	}
	while (less <= greater && less * greater != area)
	{
		while (less <= greater && less * greater < area) greater++;
		while (less <= greater && less * greater > area) less--;
	}
	res.push_back(greater);
	res.push_back(less);
	return res;
}

void quickSort2(vector<int>& nums, int low, int high)
{
	unsigned len = nums.size();
	if (high >= len) high = len - 1;
	if (low < 0) low = 0;
	if (high - low < 1) return;
	int s = low -1;
	int tmp = nums[high];
	unsigned i = low;
	for (; i < high; i++)
	{
		if (nums[i] > tmp)
		{
			s++;
			if (s != i)
				swap(nums[i], nums[s]);
		}
	}
	s++;
	swap(nums[s], nums[high]);
	quickSort2(nums, low, s- 1);
	quickSort2(nums, s+ 1, high);
}
vector<string> findRelativeRanks(vector<int>& nums) {
	vector<string> medals = { "Gold Medal","Silver Medal","Bronze Medal" };
	vector<string> ranks(nums.size());
	//quickSort(nums,0,nums.size() -1);
	//for(unsigned i = 0; i < nums.size(); i++)
	//{
	//    if(i < medals.size()) ranks[i] = medals[i];
	//    else ranks[i] += (i+1+'0');
	//}
	//return ranks; 
	vector<int> scores(10000, -1);
	for (unsigned i = 0; i < nums.size(); i++)
	{
		scores[nums[i]] = i;
	}
	unsigned index = 0;
	for (int i = scores.size() - 1; i >= 0; i--)
	{
		if (scores[i] != -1)
		{
			//ranks[index] = i;
			if (index < medals.size()) ranks[scores[i]] = medals[index];
			else ranks[scores[i]] += (index + 1 + '0');
			index++;
		}
	}

	return ranks;
}


vector<int> twoSum(vector<int>& numbers, int target) {
	for (int i = 0; i < numbers.size() - 1; i++)
	{
		int start = numbers[i + 1], end = numbers.size() - 1;
		while (start <= end)
		{
			int m = start + (end - start) / 2;
			if (numbers[m] == target - numbers[i]) return{ i + 1, m +1 };
			else if (numbers[m] > target - numbers[i]) end = m - 1;
			else start = m + 1;
		}
	}
	return{};

	//int index1, index2;
	//int half = target / 2;
	//index1 = 0;
	//index2 = numbers.size() - 1;
	//while (index1 < index2 - 1)
	//{
	//	int m = index1 + (index2 - index1) / 2;
	//	if (numbers[m] <= half)index1 = m;
	//	if (numbers[m] >= half) index2 = m;
	//}
	//if (index1 == index2)
	//{
	//	if (index1 == 0) index2 += 1;
	//	else if (index1 == numbers.size()) index1 -= 1;
	//	else if (numbers[index1 - 1] == numbers[index1]) index1 -= 1;
	//	else if (numbers[index1 + 1] == numbers[index1]) index2 += 1;
	//	else index1 -= 1;
	//}
	//while (numbers[index1] + numbers[index2] != target && (index1 > 0 || index2 < numbers.size() - 1))
	//{
	//	while (index1 > 0 && numbers[index1] + numbers[index2] > target) index1--;
	//	while (index2 < numbers.size() - 1 && numbers[index1] + numbers[index2] < target) index2++;
	//}
	//return vector<int>({ index1 + 1,index2 + 1 });
}
void quickSort(vector<int>& v, int low, int high)
{
	int l = low, h = high;
	if (l < 0)l = 0;
	if (h >= (int)v.size()) h = v.size() - 1;
	if (l >= h) return;
	int tmp = v[h];
	int s = l - 1;
	for (unsigned i = l; i < h; i++)
	{
		if (v[i] < tmp)
		{
			s++;
			if (s != i)
				swap(v[i], v[s]);
		}
	}
	s++;
	if (s != h)
		swap(v[h], v[s]);

	quickSort(v, l, s - 1);
	quickSort(v, s + 1, h);
}
int findContentChildren(vector<int>& g, vector<int>& s) {
	int offset = 0;
	int n = 0;
	quickSort(g, 0, g.size());
	quickSort(s, 0, s.size());
	for (unsigned i = 0; i < g.size(); i++)
	{
		int start = offset, end = s.size() - 1;
		int m = start;
		while (start <= end)
		{
			m = start + (end - start) / 2;
			//if (s[m] == g[i]) break;
			if (s[m] >= g[i]) end = m - 1;
			else
				start = m + 1;
		}
		int index = m >= start ? m : start;
		if (index < s.size() && s[index] >= g[i])
		{
			n++;
			offset = index + 1;
		}
		else return n;
	}
	return n;
}

typedef bool (*IsPre)(int);
bool IsOdd(int num)
{
	return num & 0x01;
}
void ReorderOddEven1(int* arr, unsigned len,IsPre IsOdd)
{
	if (arr == nullptr || len == 0) return;
	int l = 0, h = len - 1;
	while (l < h)
	{
		while (l < h && (*IsOdd)(arr[l])) l++;
		while (l < h && !(*IsOdd)(arr[h])) h--;
		if (l < h)	swap(arr[l],arr[h]);
	}
}

void ReorderOddEven2(int* arr, unsigned len)
{
	if (arr == nullptr || len == 0) return;
	for (int i = -1, j = 0; j < len; j++)
	{
		if (arr[j] & 0x01)//odd
		{
			i++;
			if (i != j)
				swap(arr[i], arr[j]);
		}
	}
}

struct ListNode {
	int m_nValue;
	ListNode* m_pNext;
};
ListNode* ConstructList(int* arr, unsigned len)
{
	if (arr == nullptr || len == 0) return nullptr;
	ListNode* head = nullptr;
	ListNode* p = nullptr;
	for (int i = 0; i < len; i++)
	{
		ListNode* node = (ListNode*)malloc(sizeof(ListNode));
		if (node == nullptr) return head;
		node->m_nValue = arr[i];
		node->m_pNext = nullptr;
		if (p == nullptr)
		{
			head = p = node;
		}
		else {
			p->m_pNext = node;
			p = node;
		}
	}
	return head;
}
ListNode* FindKthToTail(ListNode* head, int k)
{
	if (head == nullptr || k <= 0) return nullptr;
	ListNode *p = nullptr, *q = head;
	int i = 0;
	while (q != nullptr)
	{
		i += 1;
		if (i == k)
		{
			p = head;
		}
		if(i > k && p) p = p->m_pNext;
		q = q->m_pNext;
	}
	return p;
}


vector<vector<int>> FindPath(struct BinaryTreeNode<int>* root, int target)
{
	typedef struct BinaryTreeNode<int> BTNode;
	deque<BTNode*> d;
	stack<BTNode*> s;

	vector<vector<int>> paths;
	if (root != nullptr) s.push(root);
	int sum = 0;
	while (!s.empty())
	{
		BTNode* p = s.top();
		s.pop();
		bool isleaf = p->left == nullptr || p->right == nullptr;
		if (isleaf)
		{
			if (sum + p->value == target)
			{
				vector<int> path;
				for (deque<BTNode*>::iterator itr = d.begin(); itr != d.end(); itr++)
				{
					path.push_back((*itr)->value);
				}
				path.push_back(p->value);
				paths.push_back(path);
			}
			if (s.empty()) break;
			p = s.top();
			BTNode* q = nullptr;
			while (!d.empty())
			{
				q = d.back();
				if (q->left == p || q->right == p) break;
				d.pop_back();
				sum -= q->value;
			}
			if (q == nullptr || (q->left != p && q->right != p)) break;
		}
		else {
			sum += p->value;
			d.push_back(p);
			if (p->right) s.push(p->right);
			if (p->left) s.push(p->left);
		}
	}
	return paths;
}
#include <set>
int minMoves(vector<int>& nums)
{
	int step = 0;
	if (nums.size() < 2) return step;
	quickSort(nums, 0, nums.size() -1);
	int k = nums.size() - 1;
	while (*nums.begin() < *nums.rbegin())
	{
		for (k = nums.size() - 2; k > 0; k--)
		{
			if (nums[k] < nums[nums.size() - 1]) break;
		}
		int offset = nums[nums.size() - 1] - nums[k];
		for (int i = 0; i< nums.size() -1; i++)
		{
			nums[i] += offset;
		}
		int j = (int)nums.size() - 2;
		int tmp = nums[j + 1];
		while (j >= 0 && nums[j] > tmp)
		{
			nums[j+1] = nums[j];
			j--;
		}
		nums[j + 1] = tmp;
		step += offset;
	}
	return	step;
}

#include <unordered_map>
bool canConstruct(string ransomNote, string magazine)
{
	std::unordered_map<char, int> availLatters;
	bool res = true;
	for (string::const_iterator itr = magazine.begin(); itr != magazine.end(); itr++)
		availLatters.insert(make_pair(*itr, 1));
	for (string::const_iterator itr = ransomNote.begin(); itr != ransomNote.end(); itr++)
	{
		if (--availLatters[*itr] < 0) res = false;
	}
	return res;

}

string makeTimeFormat(int h, int m)
{
	stringstream s;
	s << h << ":";
	if (m < 10)
		s << "0";
	s << m;
	return s.str();
}

void func(const vector<int>& arr, int startIndex, int n, int sum, vector<int>& res)
{
	if (n <= 0 || startIndex >= arr.size())
	{
		res.push_back(sum);
		return;
	}
	for (int i = startIndex; i < (int)arr.size() - n +1; i++)
	{
		func(arr, i + 1, n - 1, sum + arr[i], res);
	}
}

#include <bitset>
vector<string> readBinaryWatch(int num) {
	vector<int> hoursAvail({ 1,2,4,8 });
	vector<int> minutesAvail({ 1,2,4,8,16,32 });
	vector<int> hours;
	vector<int> minutes;
	vector<string> res;
	
	//int i = num - (int)minutesAvail.size();
	//if (i < 0) i = 0;
	//if (i > hoursAvail.size()) return res;
	//for (; i <= hoursAvail.size() && i <= num; i++)
	//{
	//	func(hoursAvail, 0, i, 0, hours);
	//	func(minutesAvail, 0, num - i, 0, minutes);
	//	sort(hours.begin(),hours.end());
	//	for (int j = 0; j < hours.size(); j++)
	//		for (int k = 0; k < minutes.size(); k++)
	//			res.push_back(makeTimeFormat(hours[j], minutes[k]));
	//	hours.clear();
	//	minutes.clear();
	//}
	
	for (int i = 0; i < 12; i++)
	{
		int restN = num - bitset<4>(i).count();
		if (restN < 0) continue;
		for (int j = 0; j < 60; j++)
		{
			if (bitset<6>(j).count() == restN)
			{
				res.push_back(to_string(i) + (j < 10 ? ":0" : ":") + to_string(j));
			}
		}
	}
	return res;
}

#include "gbk.h"
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
	std::wstring nameW = gbk2uni("哈哈哈");
	
	std::cout << number << "->" << (uNum & ~(1<<31))<< std::endl;
	std::cout << number2 << "->" <<( uNum2 & ~(1 << 31) )<< std::endl;
	std::cout << number3 << "->" << uNum3 << std::endl;
	std::cout << number4 << "->" << (uNum4 & ~(1 << 31)) << std::endl;
	
	{
		/*
		关于有效对齐
		1.指定对齐值与自身对有效对齐值取小的那个
		2.类或结构体自身有效对齐值取成员中最大的成员自身有效对齐值
		3.起始地址%有效对齐值 == 0
		*/
		typedef struct bb
		{

			char c;             //[0]....[0]		有效对齐1
			int id;             //[4]....[7]		有效对齐4

			int id2;			//[8]....[11]		有效对齐4
			double weight;      //[16].....[23]　　　　　　原则１ 有效对齐8
			//float height;      //[16]..[19],总长要为８的整数倍,补齐[20]...[23]　　　　　原则３
			char c2;			//[24]....[24]		有效对齐 1
		}BB;		//[0]....[31] 整体有效对齐 8

		typedef struct aa {
			int n1;//[0]-[3]
			//int n2;//[4]-[7]
			BB b;//[8]-[39]	有效对齐8
			int n3;//[40]-[43]
			char c1;//[44]-[44]
		}AA;//[0]-[47]
		BB b;
		AA a;
		std::cout << "BB: " << sizeof(BB) << std::endl;
		std::cout << "b: " << sizeof(b) << std::endl;
		std::cout << "AA: " << sizeof(AA) << std::endl;
		std::cout << "a: " << sizeof(a) << std::endl;
		std::cout << "a.b: " << sizeof(a.b) << std::endl;
	}
	{
		using namespace link_list;
		LinkList L = nullptr;
		InitList(&L);
		ElementType e;
		int i;
		if (L)
		{
			printf("init success\n");
		}

		if (IsEmpty(L))
		{
			printf("list is empty\n");
		}

		for (i = 0; i < 10; i++)
		{
			InsertElement(L, i + 1, i);
		}

		if (GetElement(L, 1, e)) {
			printf("The first element is %d\n", e);
		}

		printf("length is %d\n", GetLength(L));

		printf("The 5 at %d\n", FindElement(L, 5, Compare));

		PreElement(L, 6, e);
		printf("The 6's previous element is %d\n", e);

		NextElement(L, 6, e);
		printf("The 6's next element is %d\n", e);

		DeleteElement(L,e);
		printf("delete first element is %d\n", e);

		printf("list:");
		int v;
		for (int i = 0; i < GetLength(L); i++)
		{
			GetElement(L, i + 1, v);
			printf("%d,", v);
		}
		DestroyList(&L);
		if (!L) {
			printf("\ndestroy success\n");
		}
	}
	{
		using namespace sequence_list;
		SeqList L = {0};
		if (InitList(L))
		{
			ElementType e;
			printf("init_success\n");
			int i;
			for (i = 0; i < 10; i++)
			{
				InsertElement(L, i + 1, i);
			}
			printf("length is %d\n", GetLength(L));
			if (GetElement(L, 1, e)) {
				printf("The first element is %d\n", e);
			}
			else
			{
				printf("element is not exist\n");
			}
			if (IsEmpty(L))
			{
				printf("list is empty\n");
			}
			else
			{
				printf("list is not empty\n");
			}
			printf("The 5 at %d\n", FindElement(L, 5, Compare));
			PreElement(L, 6, e);
			printf("The 6's previous element is %d\n", e);
			NextElement(L, 6, e);
			printf("The 6's next element is %d\n", e);
			DeleteElement(L,e);
			printf("delete first element is %d\n", e);
			printf("list:");
			int v;
			for (int i = 0; i < GetLength(L); i++)
			{
				GetElement(L, i + 1, v);
				printf("%d,", v);
			}
			if (DestroyList(L))
			{
				printf("\ndestroy_success\n");
			}
		}
	}
	{
		int a1[] = { 11,22,13,34,25,6 };
		int a2[] = { 1,2,3,4,5,6 };
		int a3[] = { 61,52,43,34,25,16 };
		int a5[] = { 1 };
		vector<int> a4(a3, a3 + 5);
		quicksort(a1, 0, 5);
		quicksort(a2, 0, 5);
		quicksort(a3, 0, 5);
		quickSort2(vector<int>({ 1 }), 0, 1);
		quickSort2(vector<int>({}), 0, 1);
		quickSort2(a4, 0, 5);
	}
	std::cout << findTheDifference("abcd","abcfdegh") << std::endl;

	constructRectangle(100000);

	vector<string> ranks = findRelativeRanks(vector<int>({ 5,2,3,4,1 }));


	vector<int> sum = twoSum(vector<int>({ 1,2,3,4,4,9,56,90 }), 8);

	vector<int> children({ 494, 324, 293, 199, 164, 32, 428, 155, 229, 452, 99, 433, 189, 47, 15, 37, 199, 81, 16, 110, 318, 253, 79, 31, 92, 337, 251, 448, 463, 178, 418, 399, 466, 284, 311, 403, 337, 89, 322, 405, 507, 367, 278, 173, 366, 399, 145, 125, 444, 505, 38, 181, 349, 194, 253, 521, 86, 353, 493, 514, 385, 273, 128, 526, 449, 420, 478, 184, 361, 444, 392, 359, 463, 395, 275, 436, 378, 363, 242, 396, 32, 140, 321, 137, 207, 130, 179, 185, 402, 160, 332, 57, 34, 80, 99, 497, 479, 317, 25, 173, 64, 314, 276, 504, 354, 430, 261, 247, 40, 266, 55, 301, 243, 184, 181, 313, 102, 487, 132, 478, 185, 223, 117, 141, 333, 353, 403, 184, 209, 188, 426, 8, 297, 314, 503, 143, 195, 513, 279, 54, 399, 17, 201, 281, 346, 122, 141, 183, 369, 330, 469, 56, 292, 65, 447, 120, 414, 279, 515, 382, 78, 290, 306, 420, 3, 113, 292, 426, 252, 100, 248, 157, 158, 320, 507, 61, 13, 143, 33, 321, 316, 48, 470, 345, 524, 429, 139, 439, 41, 433, 170, 168, 30, 422, 41, 79, 521, 222, 193, 115, 44, 121, 279, 226, 516, 188, 298, 130, 2, 438, 196, 277, 186, 401, 77, 285, 228, 167, 316, 500, 397, 286, 391, 11, 454, 453, 441, 60, 520, 107, 407, 134, 480, 327, 213, 217, 177, 494, 40, 337, 360, 74, 435, 16, 485, 33, 206, 237, 432, 181, 183, 197, 174, 241, 363, 474, 12, 21, 3, 190, 388, 182, 261, 447, 133, 514, 68, 91, 65, 232, 309, 272, 30, 50, 226, 70, 171, 306, 281, 312, 186, 491, 459, 485, 251, 115, 315, 54, 497, 488, 97, 79, 80, 342, 402, 308, 99, 132, 255, 98, 297, 64, 408, 396, 485, 325, 24, 422, 306, 395, 519, 279, 386, 316, 127, 41, 435, 463, 433, 158, 35, 123, 526, 458, 321, 4, 161, 502, 429, 423, 201, 219, 223, 222, 324, 273, 481, 146, 440, 195, 263, 384, 19, 393, 123, 414, 515, 286, 234, 526, 288, 381, 173, 177, 7, 238, 372, 111, 191, 207, 218, 479, 363, 32, 429, 507, 223, 175, 407, 455, 209, 282, 105, 71, 269, 158, 324, 84, 494, 42, 474, 79, 312, 295, 269, 3, 437, 505, 200, 389, 102, 301, 174, 71, 462, 361, 259, 409, 204, 258, 237, 154, 210, 205, 63, 378, 57, 280, 189, 287, 373, 188, 192, 444, 213, 66, 458, 293, 82, 143, 57, 396, 33, 277, 87, 498, 93, 329, 80, 171, 89, 180, 86, 485, 97, 122, 279, 256, 238, 456, 403, 145, 441, 174, 112, 498, 320, 100, 430, 440, 261, 174, 333, 157, 501, 132, 378, 320, 512, 161, 252, 13, 135, 240, 5, 25, 144, 444, 150, 189, 331, 142, 412, 363, 364, 385, 157, 16, 442, 15, 51, 314, 461, 446, 13, 53, 116, 340, 414, 14, 82, 471, 133, 482, 11, 335, 487, 308, 475, 376, 410, 181, 473, 138, 401, 329, 86, 34, 95, 66, 105, 374, 421, 447, 198, 203, 437, 517, 382, 369, 410, 76, 178, 308, 240, 144, 270, 492, 128, 384, 438, 426, 366, 361, 330, 128, 275, 383, 111, 379, 418, 282, 43, 335, 409, 212, 466, 471, 285, 221, 464, 330, 327, 166, 197, 324, 294, 459, 150, 169, 227, 262, 297, 262, 73, 138, 479, 141, 92, 19, 253, 259, 198, 230, 381, 400, 414, 235, 1, 9, 390, 196, 98, 326, 340, 324, 420, 490, 222, 9, 353, 314, 223, 235, 208, 348, 302, 481, 393, 225, 400, 165, 357, 443, 253, 16, 400, 344, 393, 269, 378, 183, 329, 304, 426, 346, 323, 441, 404, 263, 468, 500, 430, 406, 372, 289, 216, 213, 500, 260, 517, 448, 488, 490, 348, 487, 100, 41, 398, 96, 283, 393, 48, 58, 5, 257, 276, 207, 487, 344, 406, 339, 368, 267, 328, 491, 158, 410, 159, 434, 399, 264, 81, 159, 293, 77, 225, 526, 42, 160, 305, 8, 204, 62, 178, 316, 265, 216, 503, 423, 135, 72, 329, 111, 103, 172, 508, 62, 421, 194, 28, 90, 237, 281, 136, 68, 411, 373, 462, 406, 144, 426, 497, 86, 161, 307, 510, 334, 121, 501, 81, 268, 3, 369, 336, 285, 167, 429, 259, 143, 479, 201, 175, 110, 141, 460, 263, 206, 120, 92, 22, 244, 314, 268, 399, 322, 102, 266, 328, 254, 83, 256, 474, 278, 186, 469, 304, 376, 371, 175, 520, 428, 455, 111, 489, 319, 110, 286, 166, 89, 211, 314, 77, 121, 415, 317, 465, 340, 121, 284, 519, 488, 344, 233, 339, 341, 278, 246, 137, 517, 111, 150, 34, 271, 509, 379, 517, 351, 262, 161, 189, 365, 292, 462, 341, 374, 287, 29, 299, 239, 432, 407, 377, 447, 71, 326, 173, 237, 311, 451, 255, 395, 221, 470, 81, 374, 275, 38, 454, 210, 360, 189, 193, 133, 214, 380, 220, 43, 197, 282, 47, 300, 124, 298, 103, 32, 32, 205, 28, 53, 361, 42, 253, 230, 148, 389, 286, 250, 330, 184, 513, 525, 42, 32, 175, 303, 492, 286, 406, 69, 510, 456, 83, 105, 342, 367, 265, 4, 201, 249, 294, 102, 205, 273, 484, 228, 71, 395, 271, 421, 344, 341, 155, 525, 278, 82, 137, 359, 381, 269, 279, 421, 4, 43, 469, 522, 280, 394, 468, 85, 478, 188, 312, 124, 95, 36, 183, 484, 458, 271, 202, 213, 319, 332, 73, 497, 443, 161, 132, 88, 67, 280, 90, 480, 197, 296, 181, 145, 9, 440, 439, 52, 331, 231, 249, 109, 244, 462, 500, 506, 501, 503, 96, 478, 158, 224, 200, 386, 432, 195, 202, 222, 438, 383, 462, 8, 312, 251, 246, 425 });
	vector<int> cookies({ 383, 384, 347, 280, 433, 481, 462, 56, 487, 9, 162, 245, 485, 90, 380, 257, 364, 383, 451, 146, 48, 6, 366, 333, 490, 319, 438, 147, 212, 156, 296, 298, 241, 239, 36, 13, 187, 475, 154, 89, 399, 301, 175, 460, 143, 209, 237, 49, 5, 341, 115, 427, 92, 49, 304, 401, 283, 277, 251, 464, 171, 217, 369, 244, 495, 295, 422, 408, 20, 319, 113, 488, 27, 452, 302, 203, 373, 304, 160, 521, 92, 378, 317, 491, 232, 520, 182, 483, 270, 423, 298, 433, 264, 226, 283, 389, 23, 323, 364, 343, 60, 351, 282, 44, 306, 436, 273, 19, 330, 217, 89, 163, 445, 291, 439, 506, 107, 228, 436, 438, 16, 133, 197, 42, 145, 450, 100, 341, 98, 248, 94, 10, 30, 332, 518, 315, 111, 67, 85, 131, 296, 489, 32, 54, 249, 127, 211, 177, 328, 216, 511, 177, 185, 463, 386, 163, 43, 245, 435, 163, 70, 327, 503, 276, 101, 371, 58, 230, 410, 174, 187, 271, 144, 45, 335, 45, 140, 499, 2, 474, 512, 27, 260, 91, 52, 160, 49, 346, 180, 150, 59, 495, 42, 483, 275, 310, 351, 89, 135, 155, 76, 317, 495, 311, 185, 134, 396, 108, 240, 217, 417, 513, 317, 269, 316, 412, 390, 23, 259, 132, 407, 465, 283, 84, 173, 406, 425, 101, 106, 525, 379, 105, 202, 366, 410, 15, 344, 475, 519, 353, 275, 89, 415, 59, 452, 354, 118, 293, 484, 286, 81, 130, 359, 269, 223, 147, 511, 326, 418, 68, 321, 219, 331, 14, 374, 110, 4, 350, 446, 379, 105, 484, 209, 95, 505, 185, 509, 498, 248, 136, 449, 289, 54, 430, 399, 293, 311, 90, 93, 291, 113, 339, 47, 213, 181, 286, 239, 231, 495, 520, 318, 7, 437, 228, 83, 286, 24, 340, 358, 514, 339, 233, 303, 193, 239, 225, 239, 29, 434, 178, 109, 519, 434, 200, 413, 422, 42, 356, 255, 346, 429, 249, 484, 282, 346, 488, 408, 74, 2, 76, 239, 419, 230, 324, 511, 195, 396, 67, 427, 285, 273, 105, 226, 113, 318, 166, 224, 498, 382, 429, 170, 447, 92, 408, 386, 526, 302, 76, 10, 191, 488, 250, 133, 475, 42, 280, 479, 237, 492, 125, 344, 190, 390, 435, 461, 129, 494, 465, 441, 175, 252, 203, 205, 229, 87, 331, 88, 408, 43, 46, 238, 77, 37, 267, 121, 494, 157, 121, 347, 509, 511, 386, 132, 285, 112, 129, 287, 4, 18, 92, 188, 365, 241, 143, 4, 202, 78, 329, 496, 421, 194, 77, 45, 66, 466, 520, 268, 498, 40, 385, 525, 213, 375, 477, 58, 35, 357, 322, 311, 48, 526, 496, 403, 516, 162, 424, 518, 88, 486, 252, 89, 327, 365, 282, 512, 339, 487, 492, 501, 54, 521, 117, 475, 317, 254, 362, 247, 77, 201, 119, 304, 492, 305, 161, 133, 493, 45, 398, 42, 31, 476, 486, 93, 191, 183, 149, 258, 7, 125, 316, 93, 384, 440, 82, 512, 37, 151, 228, 328, 39, 21, 166, 447, 311, 351, 66, 122, 320, 48, 176, 281, 384, 145, 313, 187, 29, 327, 186, 489, 400, 391, 20, 243, 200, 333, 42, 496, 253, 292, 76, 105, 128, 182, 241, 284, 406, 247, 203, 42, 342, 375, 53, 383, 200, 461, 453, 144, 187, 313, 154, 349, 369, 48, 267, 154, 149, 423, 362, 426, 271, 507, 28, 197, 312, 107, 364, 45, 211, 260, 337, 347, 501, 78, 101, 390, 44, 247, 323, 101, 435, 442, 66, 442, 11, 204, 24, 121, 458, 347, 304, 234, 340, 500, 364, 103, 418, 24, 211, 59, 253, 95, 285, 177, 472, 425, 16, 315, 146, 497, 187, 108, 114, 47, 451, 171, 228, 518, 35, 411, 142, 63, 125, 28, 241, 157, 502, 503, 236, 82, 434, 481, 48, 494, 501, 45, 349, 179, 451, 246, 253, 526, 15, 203, 156, 482, 134, 78, 36, 284, 146, 45, 142, 113, 357, 75, 510, 131, 482, 362, 133, 148, 100, 147, 112, 343, 304, 50, 482, 348, 496, 337, 498, 468, 448, 326, 107, 219, 122, 360, 2, 476, 16, 479, 524, 34, 384, 160, 441, 353, 370, 48, 71, 255, 50, 131, 306, 419, 238, 505, 501, 470, 136, 235, 409, 215, 153, 148, 82, 120, 478, 403, 352, 341, 112, 466, 102, 75, 116, 426, 69, 129, 294, 178, 136, 498, 29, 487, 492, 343, 449, 132, 71, 218, 501, 27, 410, 449, 352, 410, 337, 152, 374, 342, 456, 418, 520, 91, 461, 317, 467, 498, 163, 227, 202, 449, 158, 218, 215, 30, 361, 333, 315, 171, 140, 2, 233, 67, 411, 104, 108, 415, 381, 478, 167, 111, 441, 413, 95, 238, 524, 35, 256, 147, 97, 85, 444, 227, 324, 93, 431, 57, 326, 378, 51, 31, 272, 91, 183, 237, 304, 198, 414, 502, 136, 109, 154, 258, 327, 165, 523, 10, 269, 97, 515, 518, 242, 171, 379, 136, 425, 302, 412, 458, 366, 244, 177, 276, 122, 108, 41, 61, 233, 413, 391, 180, 492, 469, 117, 280, 22, 488, 473, 188, 3, 108, 30, 346, 423, 230, 412, 53, 464, 491, 16, 476, 457, 120, 149, 259, 254, 458, 336, 366, 129, 111, 479, 153, 169, 150, 376, 260, 477, 218, 79, 307, 349, 440, 46, 426, 381, 67, 409, 437, 425, 231, 31, 237, 197, 477, 304, 218, 393, 70, 221, 199, 312, 514, 61, 76, 193, 226, 428, 516, 298, 479, 124, 149, 116, 222, 436, 314, 101, 224, 370, 11, 297, 361, 32, 273, 490, 115, 39, 181, 453, 31, 401, 295, 58, 327, 440, 175, 169, 133, 384, 103, 166, 283, 80, 360, 464, 51, 82, 109, 140, 43, 74, 209, 357, 275, 359, 340, 247, 454, 415, 136, 227, 350, 100, 399, 396, 510, 318, 22, 239, 461, 485, 60, 345, 7, 443, 25, 374, 85, 467, 272, 278, 387, 29, 94, 461, 232, 518, 318, 206, 460, 401, 168, 17, 406, 190, 385, 68, 55, 428, 348, 288, 55, 272, 290, 265, 118, 372, 504, 490, 142, 218, 360, 422, 350, 317, 515, 107, 167, 150, 508, 454, 152, 475, 34, 246, 483, 177, 80, 227, 423, 358, 75, 197, 481, 510, 130, 116, 402, 18, 110, 157, 22, 114, 122, 359, 237, 190, 167, 96, 432, 24, 58, 358, 470, 465, 57, 285, 163, 226, 257, 83, 57, 298, 524, 237, 45, 39, 452, 170, 301, 478, 1, 341, 476, 293, 170, 510, 390, 330, 149, 271, 499, 213, 61, 91, 95, 98, 9, 340, 78, 79, 46, 438, 456, 272, 481, 479, 306, 321, 368, 27, 10, 136, 394, 387, 216, 29, 458, 136, 278, 423, 164, 400, 520, 356, 201, 352, 19, 188, 33, 229, 289, 105, 273, 251, 516, 505, 285, 88, 291, 305, 213, 173, 470, 383, 239, 504, 190, 242, 305, 500, 208, 484, 242, 377, 325, 409, 2, 29, 343, 8, 488, 526, 54, 449, 80, 313, 511, 396, 204, 15, 377, 14, 475, 267, 94, 217, 294, 392, 34, 133, 79, 91, 260, 84, 295, 263, 449, 456, 278, 436, 18, 114, 436, 491, 509, 46, 198, 46, 377, 128, 357, 97, 188, 216, 293, 111, 498, 33, 22, 84, 41, 520, 312, 470, 442, 404, 86, 75, 213, 274, 45, 223, 73, 54, 199, 289, 81, 93, 31, 67, 97, 461, 91, 515, 285, 280, 139, 126, 370, 142, 196, 192, 392, 4, 422, 172, 304, 393, 352, 156, 105, 514, 106, 510, 337, 255, 167, 95, 424, 339, 455, 102, 81, 179, 391, 185, 410, 404, 368, 494, 398, 290, 20, 525, 218, 395, 194 });
	int res = findContentChildren(children,cookies);

	{
		int a1[] = { 11,22,13,34,25,6 };
		int a2[] = { 1,2,3,4,5,6 };
		int a3[] = { 61,52,43,34,25,16 };
		int a5[] = { 1 };
		ReorderOddEven1(a1,sizeof(a1)/sizeof(a1[0]),IsOdd);
		ReorderOddEven2(a2, sizeof(a2) / sizeof(a2[0]));
		ReorderOddEven1(a3, sizeof(a3) / sizeof(a3[0]),IsOdd);
		ReorderOddEven2(a5, sizeof(a5) / sizeof(a5[0]));
	}
	{
		int a1[] = { 11,22,13,34,25,6 };
		int a2[] = { 1,2,3,4,5,6 };
		int a3[] = { 61,52,43,34,25,16 };
		int a5[] = { 1 };
		ListNode* k1 = FindKthToTail(ConstructList(a1, sizeof(a1)/sizeof(a1[0])), 5);
		ListNode* k2 = FindKthToTail(ConstructList(a5, sizeof(a5) / sizeof(a5[0])), 5);
	}
	{
		int a1[] = { 11,22,13,34,25,6 };
		int a2[] = { 1,2,3,4,5,6 };
		int a3[] = { 61,52,43,34,25,16 };
		int a5[] = { 1 };
		int* a6 = mergesort(a1, 0, sizeof(a1) / sizeof(a1[0])-1);
		insetsort(a3, sizeof(a3) / sizeof(a3[0]));
		int* a7 = mergesort(a5, 0, 0);
		insetsort(a2, sizeof(a2) / sizeof(a2[0]));
		mergesort2(a5, 1);
		mergesort2(a1, sizeof(a1) / sizeof(a1[0]));
		mergesort2(a2, sizeof(a2) / sizeof(a2[0]));
	}

	{
		int a1[] = { 10,5,4,-1,-1,7,-1,-1,12,-1,-1};
		int a2[] = { 10,5,4,-1,7,-1,12 };
		BinaryTreeNode<int>* root = CreateBTree(a1, sizeof(a1)/sizeof(a1[0]));
		vector<vector<int>> v = FindPath(root,22);
		BinaryTreeNode<int>* root2 = CreateBTree(a2, sizeof(a2)/sizeof(a2[0]));
		vector<vector<int>> v2 = FindPath(root2, 22);
		vector<int> nums({ 1,2,3 });
		int step = minMoves(nums);
		bool res = canConstruct("ab", "b");
		step += 1;
		getchar();
	}

	{
		vector<string> s = readBinaryWatch(1);
		for (int i = 0; i < s.size(); i++)
		{
			cout << s[i] << " ";
		}
	}
	{
		vector<int> a({ 1,2,3,4,5 });
		vector<int>::iterator i = (++(a.rbegin())).base();
		--i;
		i -= 2;
		i++;
	}
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