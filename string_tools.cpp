#include "stdafx.h"
#include <string>

#include "string_tools.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


cdb_string_tools::cdb_string_tools()
{
}


cdb_string_tools::~cdb_string_tools()
{
}

CString cdb_string_tools::format(const TCHAR* format, ...)
{
	CString result;

	// 以下代码来自于MFC中的Format成员函数的实现
	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	return result;
}

inline std::string cdb_string_tools::wstring_to_string(const std::wstring& str)
{
	// 没有使用系统的API WideCharToMultiByte，这样移植性较好。
	// 字符串中的中文不会乱码，因为已经切换区域代码。

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "zh-CN");

	size_t szDist = 0;
	wcstombs_s(&szDist, nullptr, 0, str.c_str(), 0);
	if (szDist <= 0) return "";
	szDist += sizeof(char); // 长度再加一个结束符(应该可以不要，懒得查MSDN了，多一个稳妥点)

	char* pstr = new char[szDist];
	memset(pstr, 0, szDist);
	size_t szDistRes = 0;
	wcstombs_s(&szDistRes, pstr, szDist, str.c_str(), szDist);
	pstr[szDist - 1] = '\0'; // 最后一个字节为结束符
	string result(pstr);
	//safe_delete_array(pstr);
	delete[] pstr;

	setlocale(LC_ALL, curLocale.c_str());

	return result;

	// 	// 没有使用系统的API WideCharToMultiByte，这样移植性较好。
	// 	// 字符串中的中文不会乱码，因为已经切换区域代码。
	// 
	// 	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	// 	setlocale(LC_ALL, "zh-CN");
	// 
	// 	int szDist = WideCharToMultiByte(CP_ACP, NULL, str.c_str(), str.length(), nullptr, 0, nullptr, FALSE);
	// 	if (szDist <= 0) return "";
	// 	szDist += sizeof(char); // 长度再加一个结束符(应该可以不要，懒得查MSDN了，多一个稳妥点)
	// 
	// 	//size_t szDist = 0;
	// 	//wcstombs_s(&szDist, nullptr, 0, str.c_str(), 0);
	// 	//if (szDist <= 0) return "";
	// 	//szDist += sizeof(char); // 长度再加一个结束符(应该可以不要，懒得查MSDN了，多一个稳妥点)
	// 
	// 	char* pstr = new char[szDist];
	// 	memset(pstr, 0, szDist);
	// 	size_t szDistRes = 0;
	// 
	// 	//wcstombs_s(&szDistRes, pstr, szDist, str.c_str(), szDist);
	// 	WideCharToMultiByte(CP_ACP, NULL, str.c_str(), str.length(), pstr, szDist, nullptr, FALSE);
	// 
	// 	pstr[szDist - 1] = '\0'; // 最后一个字节为结束符
	// 	string result(pstr);
	// 	safe_delete_array(pstr);
	// 
	// 	setlocale(LC_ALL, curLocale.c_str());
	// 
	// 	return result;
}

inline std::wstring cdb_string_tools::string_to_wstring(const std::string& str)
{
	// 没有使用系统的API MultiByteToWideChar，这样移植性较好。
	// 字符串中的中文不会乱码，因为已经切换区域代码。

	std::string s(str.c_str());

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "zh-CN");

	size_t szDist = 0;
	if (0 != mbstowcs_s(&szDist, nullptr, 0, s.c_str(), s.length())) return L""; // 返回的长度已经算上结束符了
	if (szDist <= 0) return L"";

	wchar_t* pwstr = new wchar_t[szDist];
	memset(pwstr, 0, szDist);
	mbstowcs_s(&szDist, pwstr, szDist, s.c_str(), s.length());
	pwstr[szDist - 1] = '\0';
	wstring result(pwstr);
	//safe_delete_array(pwstr);
	delete pwstr;

	setlocale(LC_ALL, curLocale.c_str());

	return result;
}

CString cdb_string_tools::wstring_to_CString(const std::wstring& str)
{
	return string_to_CString(wstring_to_string(str));
}

std::wstring cdb_string_tools::CString_to_wstring(const CString& str)
{
	return string_to_wstring(CString_to_string(str));
}

CString cdb_string_tools::string_to_CString(const std::string& str)
{
#	ifdef _UNICODE
	return CString(string_to_wstring(str).c_str());
#	else
	return CString(str.c_str());
#	endif
}

void wstring_to_utf8_string(const std::wstring& in, std::string& out)
{
	out = "";

	int out_len = ::WideCharToMultiByte(CP_UTF8, 0, in.c_str(), in.length(), nullptr, 0, nullptr, nullptr);
	if (out_len <= 0) return;

	char* pResult = new char[out_len + 1];
	if (nullptr == pResult) return;
	memset(pResult, 0, (out_len + 1) * sizeof(char));

	::WideCharToMultiByte(CP_UTF8, 0, in.c_str(), out_len, pResult, out_len * sizeof(char), nullptr, nullptr);
	pResult[out_len] = '\0';
	out.assign(pResult, out_len);

	//safe_delete_array(pResult);
	delete[] pResult;
	return;
}

void utf8_string_to_wstring(const std::string& in, std::wstring& out)
{
	int out_len = ::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), in.length() * sizeof(char), nullptr, 0);
	if (out_len <= 0) return;

	wchar_t* pResult = new wchar_t[out_len + 1];
	memset(pResult, 0, (out_len + 1) * sizeof(wchar_t));

	::MultiByteToWideChar(CP_UTF8, 0, in.c_str(), in.length() * sizeof(char), pResult, out_len);
	pResult[out_len] = L'\0';

	out.assign(pResult, out_len);
	//safe_delete_array(pResult);
	delete[] pResult;
}

std::string cdb_string_tools::utf8_to_string(const std::string& str)
{
	wstring ws;
	utf8_string_to_wstring(str, ws);
	return wstring_to_string(ws);
}

std::wstring cdb_string_tools::utf8_to_wstring(const std::string& str)
{
	wstring ws;
	utf8_string_to_wstring(str, ws);
	return ws;
}

CString cdb_string_tools::utf8_to_CString(const std::string& str)
{
	wstring ws;
	utf8_string_to_wstring(str, ws);
	return wstring_to_CString(ws);
}

std::string cdb_string_tools::string_to_utf8(const std::string& str)
{
	wstring ws = string_to_wstring(str);
	string st;
	wstring_to_utf8_string(ws, st);
	return st;
}

std::string cdb_string_tools::wstring_to_utf8(const std::wstring& str)
{
	string st;
	wstring_to_utf8_string(str, st);
	return st;
}

std::string cdb_string_tools::CString_to_utf8(const CString& str)
{
	wstring ws = CString_to_wstring(str);
	string st;
	wstring_to_utf8_string(ws, st);
	return st;
}

CString cdb_string_tools::num_to_string(long num)
{
	return num_to_string((long long)num);
}

CString cdb_string_tools::num_to_string(short num)
{
	return num_to_string((long long)num);
}

CString cdb_string_tools::num_to_string(int num)
{
	return num_to_string((long long)num);
}

CString cdb_string_tools::num_to_string(unsigned short num)
{
	return num_to_string((unsigned long long) num);
}

CString cdb_string_tools::num_to_string(unsigned int num)
{
	return num_to_string((unsigned long long) num);
}

CString cdb_string_tools::num_to_string(unsigned long num)
{
	return num_to_string((unsigned long long) num);
}

CString cdb_string_tools::num_to_string(long long num)
{
	CString strResult;
	strResult.Format(_T("%lld"), num);
	return strResult;
}

CString cdb_string_tools::num_to_string(unsigned long long num)
{
	CString strResult;
	strResult.Format(_T("%llu"), num);
	return strResult;
}

CString cdb_string_tools::num_to_string(float num, int nozero_count_after_point /*= 6*/)
{
	return num_to_string((double)num, nozero_count_after_point);
}

CString cdb_string_tools::num_to_string(double num, int nozero_count_after_point /*= 6*/)
{
	if (nozero_count_after_point < 0 || nozero_count_after_point > 20) return _T("");

	if (0 == nozero_count_after_point)
	{
		CString zero_length_result;
		zero_length_result.Format(_T("%d"), (int)num);
		return zero_length_result;
	}

	CString fmt;
	fmt.Format(_T("%%.%dlf"), nozero_count_after_point);
	CString strResult;
	strResult.Format(fmt, num);

	strResult.Trim(_T('0'));

	if (strResult.IsEmpty()) strResult = _T("0");
	if (_T(".") == strResult.Left((int)_tcslen(_T(".")))) strResult = _T("0") + strResult;
	if (_T(".") == strResult.Right((int)_tcslen(_T(".")))) strResult.Delete(strResult.GetLength() - 1);
	if (strResult.IsEmpty()) strResult = _T("0"); // 之前有删除操作，所以需要判断是否为空

	return strResult;
}

void cdb_string_tools::replace_chs_punctuation_marks_to_en(CString& str)
{
	// 中文字符在双字节模式下占两个字符，所以先转为宽字符再替换，否则要加很多字符识别和组合的逻辑
	wstring str_des = CString_to_wstring(str);

	for (auto& x : str_des)
	{
		if (L'　' == x) { x = L' '; continue; }
		if (L'，' == x) { x = L','; continue; }
		if (L'；' == x) { x = L';'; continue; }
		if (L'。' == x) { x = L'.'; continue; }
		if (L'［' == x) { x = L'['; continue; }
		if (L'］' == x) { x = L']'; continue; }
		if (L'（' == x) { x = L'('; continue; }
		if (L'）' == x) { x = L')'; continue; }
		if (L'【' == x) { x = L'['; continue; }
		if (L'】' == x) { x = L']'; continue; }
		if (L'０' == x) { x = L'0'; continue; }
		if (L'１' == x) { x = L'1'; continue; }
		if (L'２' == x) { x = L'2'; continue; }
		if (L'３' == x) { x = L'3'; continue; }
		if (L'４' == x) { x = L'4'; continue; }
		if (L'５' == x) { x = L'5'; continue; }
		if (L'６' == x) { x = L'6'; continue; }
		if (L'７' == x) { x = L'7'; continue; }
		if (L'８' == x) { x = L'8'; continue; }
		if (L'９' == x) { x = L'9'; continue; }
	}

	str = wstring_to_CString(str_des);
}

std::vector<CString> cdb_string_tools::split_string(const CString& text, const CString& split/* = _T(" ,;") */)
{
	std::vector<CString> result;
	split_string(result, text, split);
	return result;
}

void cdb_string_tools::split_string(std::vector<CString>& result, const CString& text, const CString& split)
{
	result.clear();
	if (_T("") == text) return;

	CString strTmp;
	int nLen = text.GetLength();
	for (int i = 0; i < nLen; ++i)
	{
		if (-1 == split.Find(text[i])) // 如果第i个字符不是分隔符
		{
			strTmp += text[i];
		}
		else
		{
			// 第一个字符为分隔符认为之前有一个空串，在这也插入
			result.push_back(strTmp);
			strTmp = _T("");
		}
	}
	// 把残留的是最后一个字符串加到结果中（有可能是个空串）
	// 最后一个字符为分隔符认为后面有一个空串，也会插入一个空串
	result.push_back(strTmp);
}

// void string_tools::split_string2(std::vector<CString>& result, const CString& text, const CString& split /*= _T(" ,;")*/)
// {
// 	return split_string(result, text, split);
// }

bool cdb_string_tools::StringSplit(std::string src, const std::string pattern, std::vector<std::string>& strVec)
{
	strVec.clear();

	vector<CString> result_items;
	split_string2(result_items, string_to_CString(src), string_to_CString(pattern));

	for (const CString& x : result_items) strVec.push_back(CString_to_string(x));

	return true;
}

std::string wstringTostring(const std::wstring& ws)
{
	// 没有使用系统的API WideCharToMultiByte，这样移植性较好。
	// 字符串中的中文不会乱码，因为已经切换区域代码。
	// 注意，本函数不俱备异常安全性，有时间再改了支持

	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t szDist = 0;

	wcstombs_s(&szDist, NULL, 0, ws.c_str(), 0);
	szDist += sizeof(char); // 长度再加一个结束符(应该可以不要，懒得查MSDN了，多一个稳妥点)

	char* pStr = new char[szDist];
	memset(pStr, 0, szDist);

	size_t szDistRes = 0;
	wcstombs_s(&szDistRes, pStr, szDist, ws.c_str(), szDist);

	pStr[szDist - 1] = '\0'; // 最后一个字节为结束符
	std::string strResult(pStr);
	delete[] pStr;

	setlocale(LC_ALL, curLocale.c_str());

	return strResult;
}

string cdb_string_tools::CString_to_string(const CString& str)
{
#	ifdef _UNICODE
	return wstring_to_string(wstring((const wchar_t*)str));
#	else
	return string((const char*)str);
#	endif
}


//解析数字格式 1-10/3(4,7) -> 1,10
std::vector<int> cdb_string_tools::CalTHNumber(std::string str)
{
	std::vector<int> vecNumber;

	std::string::size_type pos;
	pos = str.find("-");
	if (pos != std::string::npos)
	{
		std::string::size_type  spanPos = str.find("/");
		std::string::size_type removePos = str.find("(");

		std::string::size_type scopeEnd;
		int span = 1;
		if (spanPos != std::string::npos)
		{
			scopeEnd = spanPos - 1;

			std::string strSpan;
			if (removePos != std::string::npos)
			{
				strSpan = str.substr(spanPos, removePos - spanPos);
			}
			else
			{
				strSpan = str.substr(spanPos + 1);
			}
			span = atoi(strSpan.c_str());
		}
		else
		{
			if (removePos != std::string::npos)
			{
				scopeEnd = removePos - 1;
			}
			else
			{
				scopeEnd = str.length();
			}
		}

		std::string strBegin = str.substr(0, pos);

		std::string strEnd = str.substr(pos + 1, scopeEnd - pos);

		int iBegin = atoi(strBegin.c_str());
		int iEnd = atoi(strEnd.c_str());


		std::vector<int> removeNum;
		if (removePos != std::string::npos)
		{
			std::string strReomve = str.substr(removePos, str.length() - removePos);
			std::vector<std::string> strVecRemoves;
			cdb_string_tools::StringSplit(strReomve, ",", strVecRemoves);
			for (int i = 0; i < static_cast<int>(strVecRemoves.size()); i++)
			{
				removeNum.push_back(atoi(strVecRemoves[i].c_str()));
			}
		}

		for (int i = iBegin; i <= iEnd;)
		{
			if (std::find(removeNum.begin(), removeNum.end(), i) == removeNum.end())
				vecNumber.push_back(i);
			i = i + span;
		}

	}
	else
	{
		vecNumber.push_back(atoi(str.c_str()));
	}

	return vecNumber;
}


CString cdb_string_tools::GetApplicationSelfPath()
{
	TCHAR exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	CString strPath(exeFullPath);
	const int position = strPath.ReverseFind('\\');
	CString strResPath = strPath.Left(position + 1);
	return strResPath;
}

bool IsInteger(CString str)
{
	int nCount = str.GetLength();
	for (int i = 0; i < nCount; i++)
	{
		if (0 == isdigit(str.GetAt(i)))
		{
			return false;
		}
	}
	return true;
}
bool IsValidDoubleString(CString str)
{
	int nCount = str.GetLength();
	for (int i = 0; i < nCount; i++)
	{
		if (0 == isdigit(str.GetAt(i)) && str.GetAt(i) != '.')
		{
			return false;
		}
	}
	return true;
}
void TraceRange(CString& startStr, CString& endStr, std::vector<CString>& strvec)
{
	if (IsInteger(startStr) && IsInteger(endStr))
	{
		int start = atoi(startStr);
		int end = atoi(endStr);
		if (start == end)
		{
			strvec.push_back(startStr);
		}
		else if (start < end)
		{
			for (int i = start; i <= end; ++i)
			{
				CString iStr;
				iStr.AppendFormat("%d", i);
				strvec.push_back(iStr);
			}
		}
		else
		{
			for (int i = start; i >= end; --i)
			{
				CString iStr;
				iStr.AppendFormat("%d", i);
				strvec.push_back(iStr);
			}
		}
	}
	else
	{
		if (startStr == endStr)
		{
			strvec.push_back(startStr);
		}
		else
		{
			strvec.push_back(startStr);
			strvec.push_back(endStr);
		}
	}
}
void TraceRangeNoStep(CString& str, std::vector<CString>& strVecs)
{
	CString tempStr = str;
	int index3 = tempStr.Find('-');
	if (index3 == 0)//第一个可能为负数，不能作为范围的分割符啦
	{
		//再一次进行寻找
		int index4 = tempStr.Find('-', index3 + 1);
		if (index4 == -1)//也不存在范围啦
		{
			strVecs.push_back(tempStr);
		}
		else
		{
			//存在范围了
			if (index4 > 0 && (tempStr[index4 - 1] == 'e' || tempStr[index4 - 1] == 'E'))
			{
				strVecs.push_back(tempStr);
			}
			else
			{
				CString startStr = tempStr.Mid(0, index4);//变量开始
				CString endStr = tempStr.Mid(index4 + 1);//变量结束
				//如果都是整数的话，默认步长为一，如果从大到小的话默认情况为-1
				if (IsInteger(startStr) && IsInteger(endStr))
				{
					int start = atoi(startStr);
					int end = atoi(endStr);
					if (start == end)
					{
						strVecs.push_back(startStr);
					}
					else if (start < end)
					{
						for (int i = start; i <= end; ++i)
						{
							CString iStr;
							iStr.AppendFormat("%d", i);
							strVecs.push_back(iStr);
						}
					}
					else
					{
						for (int i = start; i >= start; --i)
						{
							CString iStr;
							iStr.AppendFormat("%d", i);
							strVecs.push_back(iStr);
						}
					}
				}
				else
				{
					if (startStr == endStr)
					{
						strVecs.push_back(startStr);
					}
					else
					{
						strVecs.push_back(startStr);
						strVecs.push_back(endStr);
					}
				}
			}

		}
	}
	else if (index3 == -1)//也不存在范围啦
	{
		strVecs.push_back(tempStr);
	}
	else//存在范围啦
	{
		//可能为科学计数法，需要特殊处理
		if (index3 >= 1)
		{
			if (tempStr[index3 - 1] == 'E' || tempStr[index3 - 1] == 'e')
			{
				strVecs.push_back(tempStr);
			}
			else
			{
				CString startStr = tempStr.Mid(0, index3);//变量开始
				CString endStr = tempStr.Mid(index3 + 1);//变量结束
				TraceRange(startStr, endStr, strVecs);
			}
		}
		else
		{
			CString startStr = tempStr.Mid(0, index3);//变量开始
			CString endStr = tempStr.Mid(index3 + 1);//变量结束
			TraceRange(startStr, endStr, strVecs);
		}

	}
}
void TraceRangeStep(CString& startStr, CString& endStr, int step, std::vector<CString>& strvec)
{
	if (IsInteger(startStr) && IsInteger(endStr))
	{
		int start = atoi(startStr);
		int end = atoi(endStr);
		if (start == end)
		{
			strvec.push_back(startStr);
		}
		else if (start < end)
		{
			for (int i = start; i <= end; i += step)
			{
				CString iStr;
				iStr.AppendFormat("%d", i);
				strvec.push_back(iStr);
			}
		}
		else
		{
			for (int i = start; i >= end; ++step)
			{
				CString iStr;
				iStr.AppendFormat("%d", i);
				strvec.push_back(iStr);
			}
		}
	}
	else
	{
		if (startStr == endStr)
		{
			strvec.push_back(startStr);
		}
		else
		{
			strvec.push_back(startStr);
			strvec.push_back(endStr);
		}
	}
}

void TraceRangeStep(CString& str, std::vector<CString>& strVecs, int step)
{
	CString tempStr = str;
	int index3 = tempStr.Find('-');
	if (index3 == 0)//第一个可能为负数，不能作为范围的分割符啦
	{
		//再一次进行寻找
		int index4 = tempStr.Find('-');
		if (index4 == -1)//也不存在范围啦
		{
			strVecs.push_back(tempStr);
		}
		else
		{
			//存在范围了
			CString startStr = tempStr.Mid(0, index4);//变量开始
			CString endStr = tempStr.Mid(index4 + 1);//变量结束
			//如果都是整数的话，默认步长为一，如果从大到小的话默认情况为-1
			if (IsInteger(startStr) && IsInteger(endStr))
			{
				int start = atoi(startStr);
				int end = atoi(endStr);
				if (start == end)
				{
					strVecs.push_back(startStr);
				}
				else if (start < end)
				{
					for (int i = start; i <= end; i += step)
					{
						CString iStr;
						iStr.AppendFormat("%d", i);
						strVecs.push_back(iStr);
					}
				}
				else
				{
					for (int i = end; i <= start; i -= step)
					{
						CString iStr;
						iStr.AppendFormat("%d", i);
						strVecs.push_back(iStr);
					}
				}
			}
			else
			{
				if (startStr == endStr)
				{
					strVecs.push_back(startStr);
				}
				else
				{
					strVecs.push_back(startStr);
					strVecs.push_back(endStr);
				}
			}
		}
	}
	else if (index3 == -1)//也不存在范围啦
	{
		strVecs.push_back(tempStr);
	}
	else//存在范围啦
	{
		CString startStr = tempStr.Mid(0, index3);//变量开始
		CString endStr = tempStr.Mid(index3 + 1);//变量结束
		TraceRangeStep(startStr, endStr, step, strVecs);
	}
}

BOOL cdb_string_tools::merge_string(CString& strRes, std::vector<CString>& vecStrings, const CString& str)
{
	if (vecStrings.size() < 1)
		return FALSE;

	if (str.IsEmpty())
		return FALSE;

	strRes = vecStrings[0];
	for (size_t i = 1; i < vecStrings.size(); ++i)
	{
		strRes = strRes + str + vecStrings[i];
	}
	return TRUE;
}

CString cdb_string_tools::merge_string(std::vector<CString>& vecStrings, const CString& splitStr /*= _T(",")*/)
{
	CString strRes;
	merge_string(strRes, vecStrings, splitStr);
	return strRes;
}

int cdb_string_tools::get_text_width_in_px(const CString& text, CDC* dc)
{
	if (!dc) return text.GetLength() * 8; // 按8个像素估一个返回，不要直接返回0

	CSize sz = dc->GetTextExtent(text);
	return (int)sz.cx;
}

void cdb_string_tools::generate_guid(CString& guid_text)
{
	GUID guid; // 生成guid作为文件名
	if (S_OK == ::CoCreateGuid(&guid))
	{
		// GUID 格式如66C78769-C996-488a-AA8F-3CB806782FE6，长度为36
		TCHAR guid_char_array[40] = { _T('\0') }; // 程序已经异常，能少分配内存尽量少分配
		_sntprintf_s(guid_char_array, _countof(guid_char_array) - 1,
			_T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"), // 0表示使用0填充
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

		guid_text = guid_char_array;
	}
}

CString cdb_string_tools::generate_guid()
{
	CString guid;
	generate_guid(guid);
	return guid;
}

void cdb_string_tools::parser_th_string(CString& str, std::vector<CString>& strvec)
{
	std::vector<CString> tempStrVec;
	CString tempStr = str;
	tempStr.Trim();
	int index = tempStr.Find('/');
	if (index == -1)//不存在步长
	{
		int index2 = tempStr.Find('*');
		if (index2 == -1)//不攒在多个的概念
		{
			int index3 = tempStr.Find('-');
			if (index3 == 0)//第一个可能为负数，不能作为范围的分割符啦
			{
				//再一次进行寻找
				int index4 = tempStr.Find('-', 1);
				if (index4 == -1)//也不存在范围啦
				{
					tempStrVec.push_back(tempStr);
				}
				else
				{
					//存在范围了
					CString startStr = tempStr.Mid(0, index4);//变量开始
					CString endStr = tempStr.Mid(index4 + 1);//变量结束
					//如果都是整数的话，默认步长为一，如果从大到小的话默认情况为-1
					if (IsInteger(startStr) && IsInteger(endStr))
					{
						int start = atoi(startStr);
						int end = atoi(endStr);
						if (start == end)
						{
							tempStrVec.push_back(startStr);
						}
						else if (start < end)
						{
							for (int i = start; i <= end; ++i)
							{
								CString iStr;
								iStr.AppendFormat("%d", i);
								tempStrVec.push_back(iStr);
							}
						}
						else
						{
							for (int i = start; i >= end; --i)
							{
								CString iStr;
								iStr.AppendFormat("%d", i);
								tempStrVec.push_back(iStr);
							}
						}
					}
					else
					{
						if (startStr == endStr)
						{
							tempStrVec.push_back(startStr);
						}
						else
						{
							tempStrVec.push_back(startStr);
							tempStrVec.push_back(endStr);
						}
					}
				}
			}
			else if (index3 == -1)//也不存在范围啦
			{
				tempStrVec.push_back(tempStr);
			}
			else//存在范围啦
			{
				if (tempStr[index3 - 1] == 'E' || tempStr[index3 - 1] == 'e')
				{
					tempStrVec.push_back(tempStr);
				}
				else
				{
					CString startStr = tempStr.Mid(0, index3);//变量开始
					CString endStr = tempStr.Mid(index3 + 1);//变量结束
					if (IsValidDoubleString(startStr) && IsValidDoubleString(endStr))
					{
						TraceRange(startStr, endStr, tempStrVec);
					}
					else
					{
						tempStrVec.push_back(tempStr);
					}

				}

			}
		}
		else
		{
			CString  coutStr = tempStr.Mid(0, index2);
			CString  rangeStr = tempStr.Mid(index2 + 1);
			int cout = atoi(coutStr.GetString());
			if (IsInteger(coutStr))
			{
				for (int i = 0; i < cout; ++i)
				{
					TraceRangeNoStep(rangeStr, tempStrVec);
				}
			}
			else
			{
				tempStrVec.push_back(tempStr);//不需要任何处理直接加到其中
			}

			//存在多个的概念
		}
	}
	else//存在步长
	{
		//对字符串进行分割
		CString str1 = tempStr.Mid(0, index);
		CString str2 = tempStr.Mid(index + 1);
		int index5 = str1.Find('-');
		int index6 = str2.Find('*');
		if (index5 == -1)//不存在范围
		{
			if (index6 == -1)//不存在多个
			{
				tempStrVec.push_back(tempStr);
			}
			else//存在多个
			{
				//int count = atoi(str2.GetString());
				CString  countStr = str2.Mid(0, index6);//不存在范围，步长也没什么作用
				CString  stepStr = str2.Mid(index6 + 1);
				int step = atoi(stepStr.GetString());
				int count = atoi(countStr.GetString());
				for (int i = 0; i < count; i++)
				{
					tempStrVec.push_back(str1);
				}
			}
		}
		else
		{
			if (index6 == -1)//存在范围，不存在多个
			{
				int step = atoi(str2.GetString());
				TraceRangeStep(str1, tempStrVec, step);
			}
			else//存在多个
			{
				CString  countStr = str2.Mid(0, index6);
				CString  stepStr = str2.Mid(index6 + 1);
				int step = atoi(stepStr.GetString());
				int count = atoi(countStr.GetString());
				for (int i = 0; i < count; i++)
				{
					TraceRangeStep(str1, tempStrVec, step);
				}
			}
		}
	}
	//strvec = tempStrVec;
	std::copy(tempStrVec.begin(), tempStrVec.end(), back_inserter(strvec));
}

void cdb_string_tools::split_string2(std::vector<CString>& result, const CString& text, const CString& split /*= _T(" ,;")*/)
{
	return split_string(result, text, split);
}