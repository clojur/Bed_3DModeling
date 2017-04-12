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

	// ���´���������MFC�е�Format��Ա������ʵ��
	va_list argList;
	va_start(argList, format);
	result.FormatV(format, argList);
	va_end(argList);

	return result;
}

inline std::string cdb_string_tools::wstring_to_string(const std::wstring& str)
{
	// û��ʹ��ϵͳ��API WideCharToMultiByte��������ֲ�ԽϺá�
	// �ַ����е����Ĳ������룬��Ϊ�Ѿ��л�������롣

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "zh-CN");

	size_t szDist = 0;
	wcstombs_s(&szDist, nullptr, 0, str.c_str(), 0);
	if (szDist <= 0) return "";
	szDist += sizeof(char); // �����ټ�һ��������(Ӧ�ÿ��Բ�Ҫ�����ò�MSDN�ˣ���һ�����׵�)

	char* pstr = new char[szDist];
	memset(pstr, 0, szDist);
	size_t szDistRes = 0;
	wcstombs_s(&szDistRes, pstr, szDist, str.c_str(), szDist);
	pstr[szDist - 1] = '\0'; // ���һ���ֽ�Ϊ������
	string result(pstr);
	//safe_delete_array(pstr);
	delete[] pstr;

	setlocale(LC_ALL, curLocale.c_str());

	return result;

	// 	// û��ʹ��ϵͳ��API WideCharToMultiByte��������ֲ�ԽϺá�
	// 	// �ַ����е����Ĳ������룬��Ϊ�Ѿ��л�������롣
	// 
	// 	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	// 	setlocale(LC_ALL, "zh-CN");
	// 
	// 	int szDist = WideCharToMultiByte(CP_ACP, NULL, str.c_str(), str.length(), nullptr, 0, nullptr, FALSE);
	// 	if (szDist <= 0) return "";
	// 	szDist += sizeof(char); // �����ټ�һ��������(Ӧ�ÿ��Բ�Ҫ�����ò�MSDN�ˣ���һ�����׵�)
	// 
	// 	//size_t szDist = 0;
	// 	//wcstombs_s(&szDist, nullptr, 0, str.c_str(), 0);
	// 	//if (szDist <= 0) return "";
	// 	//szDist += sizeof(char); // �����ټ�һ��������(Ӧ�ÿ��Բ�Ҫ�����ò�MSDN�ˣ���һ�����׵�)
	// 
	// 	char* pstr = new char[szDist];
	// 	memset(pstr, 0, szDist);
	// 	size_t szDistRes = 0;
	// 
	// 	//wcstombs_s(&szDistRes, pstr, szDist, str.c_str(), szDist);
	// 	WideCharToMultiByte(CP_ACP, NULL, str.c_str(), str.length(), pstr, szDist, nullptr, FALSE);
	// 
	// 	pstr[szDist - 1] = '\0'; // ���һ���ֽ�Ϊ������
	// 	string result(pstr);
	// 	safe_delete_array(pstr);
	// 
	// 	setlocale(LC_ALL, curLocale.c_str());
	// 
	// 	return result;
}

inline std::wstring cdb_string_tools::string_to_wstring(const std::string& str)
{
	// û��ʹ��ϵͳ��API MultiByteToWideChar��������ֲ�ԽϺá�
	// �ַ����е����Ĳ������룬��Ϊ�Ѿ��л�������롣

	std::string s(str.c_str());

	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "zh-CN");

	size_t szDist = 0;
	if (0 != mbstowcs_s(&szDist, nullptr, 0, s.c_str(), s.length())) return L""; // ���صĳ����Ѿ����Ͻ�������
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
	if (strResult.IsEmpty()) strResult = _T("0"); // ֮ǰ��ɾ��������������Ҫ�ж��Ƿ�Ϊ��

	return strResult;
}

void cdb_string_tools::replace_chs_punctuation_marks_to_en(CString& str)
{
	// �����ַ���˫�ֽ�ģʽ��ռ�����ַ���������תΪ���ַ����滻������Ҫ�Ӻܶ��ַ�ʶ�����ϵ��߼�
	wstring str_des = CString_to_wstring(str);

	for (auto& x : str_des)
	{
		if (L'��' == x) { x = L' '; continue; }
		if (L'��' == x) { x = L','; continue; }
		if (L'��' == x) { x = L';'; continue; }
		if (L'��' == x) { x = L'.'; continue; }
		if (L'��' == x) { x = L'['; continue; }
		if (L'��' == x) { x = L']'; continue; }
		if (L'��' == x) { x = L'('; continue; }
		if (L'��' == x) { x = L')'; continue; }
		if (L'��' == x) { x = L'['; continue; }
		if (L'��' == x) { x = L']'; continue; }
		if (L'��' == x) { x = L'0'; continue; }
		if (L'��' == x) { x = L'1'; continue; }
		if (L'��' == x) { x = L'2'; continue; }
		if (L'��' == x) { x = L'3'; continue; }
		if (L'��' == x) { x = L'4'; continue; }
		if (L'��' == x) { x = L'5'; continue; }
		if (L'��' == x) { x = L'6'; continue; }
		if (L'��' == x) { x = L'7'; continue; }
		if (L'��' == x) { x = L'8'; continue; }
		if (L'��' == x) { x = L'9'; continue; }
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
		if (-1 == split.Find(text[i])) // �����i���ַ����Ƿָ���
		{
			strTmp += text[i];
		}
		else
		{
			// ��һ���ַ�Ϊ�ָ�����Ϊ֮ǰ��һ���մ�������Ҳ����
			result.push_back(strTmp);
			strTmp = _T("");
		}
	}
	// �Ѳ����������һ���ַ����ӵ�����У��п����Ǹ��մ���
	// ���һ���ַ�Ϊ�ָ�����Ϊ������һ���մ���Ҳ�����һ���մ�
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
	// û��ʹ��ϵͳ��API WideCharToMultiByte��������ֲ�ԽϺá�
	// �ַ����е����Ĳ������룬��Ϊ�Ѿ��л�������롣
	// ע�⣬���������㱸�쳣��ȫ�ԣ���ʱ���ٸ���֧��

	std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t szDist = 0;

	wcstombs_s(&szDist, NULL, 0, ws.c_str(), 0);
	szDist += sizeof(char); // �����ټ�һ��������(Ӧ�ÿ��Բ�Ҫ�����ò�MSDN�ˣ���һ�����׵�)

	char* pStr = new char[szDist];
	memset(pStr, 0, szDist);

	size_t szDistRes = 0;
	wcstombs_s(&szDistRes, pStr, szDist, ws.c_str(), szDist);

	pStr[szDist - 1] = '\0'; // ���һ���ֽ�Ϊ������
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


//�������ָ�ʽ 1-10/3(4,7) -> 1,10
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
	if (index3 == 0)//��һ������Ϊ������������Ϊ��Χ�ķָ����
	{
		//��һ�ν���Ѱ��
		int index4 = tempStr.Find('-', index3 + 1);
		if (index4 == -1)//Ҳ�����ڷ�Χ��
		{
			strVecs.push_back(tempStr);
		}
		else
		{
			//���ڷ�Χ��
			if (index4 > 0 && (tempStr[index4 - 1] == 'e' || tempStr[index4 - 1] == 'E'))
			{
				strVecs.push_back(tempStr);
			}
			else
			{
				CString startStr = tempStr.Mid(0, index4);//������ʼ
				CString endStr = tempStr.Mid(index4 + 1);//��������
				//������������Ļ���Ĭ�ϲ���Ϊһ������Ӵ�С�Ļ�Ĭ�����Ϊ-1
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
	else if (index3 == -1)//Ҳ�����ڷ�Χ��
	{
		strVecs.push_back(tempStr);
	}
	else//���ڷ�Χ��
	{
		//����Ϊ��ѧ����������Ҫ���⴦��
		if (index3 >= 1)
		{
			if (tempStr[index3 - 1] == 'E' || tempStr[index3 - 1] == 'e')
			{
				strVecs.push_back(tempStr);
			}
			else
			{
				CString startStr = tempStr.Mid(0, index3);//������ʼ
				CString endStr = tempStr.Mid(index3 + 1);//��������
				TraceRange(startStr, endStr, strVecs);
			}
		}
		else
		{
			CString startStr = tempStr.Mid(0, index3);//������ʼ
			CString endStr = tempStr.Mid(index3 + 1);//��������
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
	if (index3 == 0)//��һ������Ϊ������������Ϊ��Χ�ķָ����
	{
		//��һ�ν���Ѱ��
		int index4 = tempStr.Find('-');
		if (index4 == -1)//Ҳ�����ڷ�Χ��
		{
			strVecs.push_back(tempStr);
		}
		else
		{
			//���ڷ�Χ��
			CString startStr = tempStr.Mid(0, index4);//������ʼ
			CString endStr = tempStr.Mid(index4 + 1);//��������
			//������������Ļ���Ĭ�ϲ���Ϊһ������Ӵ�С�Ļ�Ĭ�����Ϊ-1
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
	else if (index3 == -1)//Ҳ�����ڷ�Χ��
	{
		strVecs.push_back(tempStr);
	}
	else//���ڷ�Χ��
	{
		CString startStr = tempStr.Mid(0, index3);//������ʼ
		CString endStr = tempStr.Mid(index3 + 1);//��������
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
	if (!dc) return text.GetLength() * 8; // ��8�����ع�һ�����أ���Ҫֱ�ӷ���0

	CSize sz = dc->GetTextExtent(text);
	return (int)sz.cx;
}

void cdb_string_tools::generate_guid(CString& guid_text)
{
	GUID guid; // ����guid��Ϊ�ļ���
	if (S_OK == ::CoCreateGuid(&guid))
	{
		// GUID ��ʽ��66C78769-C996-488a-AA8F-3CB806782FE6������Ϊ36
		TCHAR guid_char_array[40] = { _T('\0') }; // �����Ѿ��쳣�����ٷ����ڴ澡���ٷ���
		_sntprintf_s(guid_char_array, _countof(guid_char_array) - 1,
			_T("%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"), // 0��ʾʹ��0���
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
	if (index == -1)//�����ڲ���
	{
		int index2 = tempStr.Find('*');
		if (index2 == -1)//�����ڶ���ĸ���
		{
			int index3 = tempStr.Find('-');
			if (index3 == 0)//��һ������Ϊ������������Ϊ��Χ�ķָ����
			{
				//��һ�ν���Ѱ��
				int index4 = tempStr.Find('-', 1);
				if (index4 == -1)//Ҳ�����ڷ�Χ��
				{
					tempStrVec.push_back(tempStr);
				}
				else
				{
					//���ڷ�Χ��
					CString startStr = tempStr.Mid(0, index4);//������ʼ
					CString endStr = tempStr.Mid(index4 + 1);//��������
					//������������Ļ���Ĭ�ϲ���Ϊһ������Ӵ�С�Ļ�Ĭ�����Ϊ-1
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
			else if (index3 == -1)//Ҳ�����ڷ�Χ��
			{
				tempStrVec.push_back(tempStr);
			}
			else//���ڷ�Χ��
			{
				if (tempStr[index3 - 1] == 'E' || tempStr[index3 - 1] == 'e')
				{
					tempStrVec.push_back(tempStr);
				}
				else
				{
					CString startStr = tempStr.Mid(0, index3);//������ʼ
					CString endStr = tempStr.Mid(index3 + 1);//��������
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
				tempStrVec.push_back(tempStr);//����Ҫ�κδ���ֱ�Ӽӵ�����
			}

			//���ڶ���ĸ���
		}
	}
	else//���ڲ���
	{
		//���ַ������зָ�
		CString str1 = tempStr.Mid(0, index);
		CString str2 = tempStr.Mid(index + 1);
		int index5 = str1.Find('-');
		int index6 = str2.Find('*');
		if (index5 == -1)//�����ڷ�Χ
		{
			if (index6 == -1)//�����ڶ��
			{
				tempStrVec.push_back(tempStr);
			}
			else//���ڶ��
			{
				//int count = atoi(str2.GetString());
				CString  countStr = str2.Mid(0, index6);//�����ڷ�Χ������Ҳûʲô����
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
			if (index6 == -1)//���ڷ�Χ�������ڶ��
			{
				int step = atoi(str2.GetString());
				TraceRangeStep(str1, tempStrVec, step);
			}
			else//���ڶ��
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