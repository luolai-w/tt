#include "stdafx.h"
#include "gbk.h"
#include "uni2gbk.inl"
#include "gbk2uni.inl"
#include <algorithm>
#include <atlbase.h>
const Uni2GbkItem * Find(const Uni2GbkItem * arr, unsigned int count, unsigned short data)
{
#if 0
	auto it = std::lower_bound(arr, arr + count, data,[](const Uni2GbkItem& item, unsigned short data) {
		return item.end < data;
	});

	if (it != arr + count && data >= it->start && data <= it->end)
	{
		return it;
	}

	return nullptr;
#else
	for (unsigned int i = 0; i < count; ++i)
	{
		if (data >= arr[i].start && data <= arr[i].end)
			return arr + i;
	}
	return nullptr;
#endif
}

std::string uni2gbk(const std::wstring& wstr)
{
	std::string str;
	for (unsigned short w : wstr)
	{
		auto * item = Find(uni2gbk_table, uni2gbk_table_count, w);
		if (item)
		{
			unsigned short code = item->data[w - item->start];
			char code2 = code & 0xff;
			char code1 = (code >> 8) & 0xff;
			if (code1 != 0) str += code1;
			str += code2;
		}
		else
		{
			str += '?';
		}
	}
	return str;
}

std::wstring gbk2uni(const std::string& str)
{
	std::wstring wstr;
	unsigned int count = (unsigned)str.size();
	for (unsigned j = 0; j < count; ++j)
	{
		unsigned char c = str[j];
		if (c <= 128 || j + 1 == count)
		{
			wstr += c;
		}
		else
		{
			unsigned char c2 = str[j + 1];
			unsigned short code = (c2 << 8) | c;
			auto * item = Find(gbk2uni_table, gbk2uni_table_count, code);
			if (item)
			{
				wstr += item->data[code - item->start];
				++j;
			}
			else
			{
				wstr += L'?';
				++j;
			}
		}
	}
	return wstr;
}