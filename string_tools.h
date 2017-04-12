#pragma once

class  DB_3DMODELING_API cdb_string_tools
{
private:
	cdb_string_tools();
	~cdb_string_tools();

public:
	// 格式化并生成一个CString对象，行为与CString的Format保持一致
	static CString format(const TCHAR* format, ...);

	// 字符串转换
	// 注意：
	//   所以的函数名中的 string均为multi-byte 字符串，utf8均为 utf-8 格式的string，二者数据类型一样，但内部编码不一样
	//   wstring 均为unicode字符串，编码为 utf-16 形式
	//   CString 与项目设置的字符编码相关，相当于 multi-byte 的 string 或 utf-16 的 wstring
	static std::string wstring_to_string(const std::wstring& str);
	static std::string CString_to_string(const CString& str);
	static std::string utf8_to_string(const std::string& str);
	static std::wstring string_to_wstring(const std::string& str);
	static std::wstring CString_to_wstring(const CString& str);
	static std::wstring utf8_to_wstring(const std::string& str);
	static CString wstring_to_CString(const std::wstring& str);
	static CString string_to_CString(const std::string& str);
	static CString utf8_to_CString(const std::string& str);
	static std::string string_to_utf8(const std::string& str);
	static std::string wstring_to_utf8(const std::wstring& str);
	static std::string CString_to_utf8(const CString& str);

	// 将数字转为字符串，如果是小数，会并把尾部多余的0去掉,保留6小数
	static CString num_to_string(short num);
	static CString num_to_string(int num);
	static CString num_to_string(long num);
	static CString num_to_string(long long num);
	static CString num_to_string(unsigned short num);
	static CString num_to_string(unsigned int num);
	static CString num_to_string(unsigned long num);
	static CString num_to_string(unsigned long long num);
	static CString num_to_string(float num, int nozero_count_after_point = 5); // 结果的小数部分末尾的0去除，nozero_count_after_point表示小数点后非零的数据最大长度（最多20位，四舍五入处理）
	static CString num_to_string(double num, int nozero_count_after_point = 5); // 结果的小数部分末尾的0去除，nozero_count_after_point表示小数点后非零的数据最大长度（最多20位，四舍五入处理）

	// 将字符串中的中文逗号、句号、分号、左右小括号、左右中括号、全角空格全部换成英文半角字符
	static void replace_chs_punctuation_marks_to_en(CString& str);

	// 把字符串按指定的字符串中的任意字符来拆分并返回结果，默认分隔符为空格逗号分号，分隔符可能是临时变量，不使用引用传递
	static std::vector<CString> split_string(const CString& text, const CString& split = _T(" ,;"));
	static void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));

	//支持同豪数字表达式//解析数字格式 1-10/3(4,7) -> 1,10
	static std::vector<int> CalTHNumber(std::string str);

	// 解析IO规范n - m / k*i格式，此函数仅解释n - m / k*i的格式完整的字符串解析请使用ParserTHString
	// @param[in] str 待解析的字符串
	// @param[out] strvec 解析好的字符串数组
	static void parser_th_string(CString& str, std::vector<CString>& strvec);

	// 合并字符串
	// @param[in] vecStrings 待合并的字符串，splitStr 分隔符
	// @param[out] strRes 合并好的字符串
	static BOOL merge_string(CString& strRes, std::vector<CString>& vecStrings, const CString& splitStr = _T(","));
	static CString merge_string(std::vector<CString>& vecStrings, const CString& splitStr = _T(","));

	// 根据dc计算字符串的绘制宽度（以像素为单位）
	static int get_text_width_in_px(const CString& text, CDC* dc);

	// 生成guid字符串，格式为：66C78769-C996-488a-AA8F-3CB806782FE6，长度为36
	static void generate_guid(CString& guid_text);
	static CString generate_guid();

public:
	// 已废弃的接口 /////////////////////////////////////////////////////////////

	//字符串分割，请改用split_string()接口
	static void split_string2(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
	static bool StringSplit(std::string src, std::string pattern, std::vector<std::string>& strVec);

	// 返回当前应用程序所在路径（结尾包含\\）,请改用file_tools.h中的函数
	static CString GetApplicationSelfPath();
};

