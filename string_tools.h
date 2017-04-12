#pragma once

class  DB_3DMODELING_API cdb_string_tools
{
private:
	cdb_string_tools();
	~cdb_string_tools();

public:
	// ��ʽ��������һ��CString������Ϊ��CString��Format����һ��
	static CString format(const TCHAR* format, ...);

	// �ַ���ת��
	// ע�⣺
	//   ���Եĺ������е� string��Ϊmulti-byte �ַ�����utf8��Ϊ utf-8 ��ʽ��string��������������һ�������ڲ����벻һ��
	//   wstring ��Ϊunicode�ַ���������Ϊ utf-16 ��ʽ
	//   CString ����Ŀ���õ��ַ�������أ��൱�� multi-byte �� string �� utf-16 �� wstring
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

	// ������תΪ�ַ����������С�����Ტ��β�������0ȥ��,����6С��
	static CString num_to_string(short num);
	static CString num_to_string(int num);
	static CString num_to_string(long num);
	static CString num_to_string(long long num);
	static CString num_to_string(unsigned short num);
	static CString num_to_string(unsigned int num);
	static CString num_to_string(unsigned long num);
	static CString num_to_string(unsigned long long num);
	static CString num_to_string(float num, int nozero_count_after_point = 5); // �����С������ĩβ��0ȥ����nozero_count_after_point��ʾС���������������󳤶ȣ����20λ���������봦��
	static CString num_to_string(double num, int nozero_count_after_point = 5); // �����С������ĩβ��0ȥ����nozero_count_after_point��ʾС���������������󳤶ȣ����20λ���������봦��

	// ���ַ����е����Ķ��š���š��ֺš�����С���š����������š�ȫ�ǿո�ȫ������Ӣ�İ���ַ�
	static void replace_chs_punctuation_marks_to_en(CString& str);

	// ���ַ�����ָ�����ַ����е������ַ�����ֲ����ؽ����Ĭ�Ϸָ���Ϊ�ո񶺺ŷֺţ��ָ�����������ʱ��������ʹ�����ô���
	static std::vector<CString> split_string(const CString& text, const CString& split = _T(" ,;"));
	static void split_string(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));

	//֧��ͬ�����ֱ��ʽ//�������ָ�ʽ 1-10/3(4,7) -> 1,10
	static std::vector<int> CalTHNumber(std::string str);

	// ����IO�淶n - m / k*i��ʽ���˺���������n - m / k*i�ĸ�ʽ�������ַ���������ʹ��ParserTHString
	// @param[in] str ���������ַ���
	// @param[out] strvec �����õ��ַ�������
	static void parser_th_string(CString& str, std::vector<CString>& strvec);

	// �ϲ��ַ���
	// @param[in] vecStrings ���ϲ����ַ�����splitStr �ָ���
	// @param[out] strRes �ϲ��õ��ַ���
	static BOOL merge_string(CString& strRes, std::vector<CString>& vecStrings, const CString& splitStr = _T(","));
	static CString merge_string(std::vector<CString>& vecStrings, const CString& splitStr = _T(","));

	// ����dc�����ַ����Ļ��ƿ�ȣ�������Ϊ��λ��
	static int get_text_width_in_px(const CString& text, CDC* dc);

	// ����guid�ַ�������ʽΪ��66C78769-C996-488a-AA8F-3CB806782FE6������Ϊ36
	static void generate_guid(CString& guid_text);
	static CString generate_guid();

public:
	// �ѷ����Ľӿ� /////////////////////////////////////////////////////////////

	//�ַ����ָ�����split_string()�ӿ�
	static void split_string2(std::vector<CString>& result, const CString& text, const CString& split = _T(" ,;"));
	static bool StringSplit(std::string src, std::string pattern, std::vector<std::string>& strVec);

	// ���ص�ǰӦ�ó�������·������β����\\��,�����file_tools.h�еĺ���
	static CString GetApplicationSelfPath();
};

