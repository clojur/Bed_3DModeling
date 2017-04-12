#pragma once


// �����Զ���ľ͵ش��ڣ���Ҫ�Ӵ˻�������
class custom_inplace_window_base : public CWnd
{
	DECLARE_DYNAMIC(custom_inplace_window_base)
	DECLARE_MESSAGE_MAP()

public:
	custom_inplace_window_base();
	virtual ~custom_inplace_window_base();

public:
	virtual void set_data_to_ui();
	virtual void get_data_from_ui();
	virtual void initialize_finished();
	
public:
	void close_inplace_window(bool return_value = false);
	
	// ��ȡ���ڹر�ʱ��״̬��ʾ����������е�������Ч��Ϊtrue������û�ȡ�����ڴ��ڻ�ʧȥ���㵼�´��������򷵻�false;
	bool get_validate_flag() const;
	void set_validate_flag(bool flag);
};

// ����ʹ�õĹ�����,�����û����д��ڲ����������ڲ�ά�����ⲿʹ��(WYZ)
class inplace_tool_window
{
public:
	// ��ʾһ���ı�����������,���ô˺����ὫstrText�е�������ʾ�������ı�����,������ɺ�Ὣ��ֵ����strText��, ���������Ч���򷵻�true����Ч�򷵻�false
	static bool inplace_textbox(IN OUT CString& strText, IN COLORREF text_color = RGB(0, 0, 0), IN COLORREF back_color = RGB(200, 200, 200), IN COLORREF border_color = RGB(0, 0, 0));
	
	// ��ʾһ��ComboBox������ѡ����ı�, ���������Ч���򷵻�true����Ч�򷵻�false
	static bool inplace_combobox(IN OUT CString& strText, IN std::vector<CString>& all_items, IN bool can_edit = false);

	// ��ʾһ������ľ͵ش��ڣ�ָ�����ⲿ����ά��,�ҹ����Ĵ������δ�������ڲ����Զ�����������ֱ��ʹ���Ѿ������õĴ���
	// window_rectָ���͵ش��ڵĵ�λ�úʹ�С��λ��ʹ����Ļ����ϵ
	// ���������Ч���򷵻�true����Ч�򷵻�false
	static bool track_custom_inplace_window(custom_inplace_window_base* wnd, const CRect& windows_rect);
	
private:
	// ��ֹʵ�������Լ���������
	inplace_tool_window();
	inplace_tool_window(const inplace_tool_window&);
	const inplace_tool_window& operator=(const inplace_tool_window&);
	~inplace_tool_window();
};


class functor_by_message
{
public:
	functor_by_message();
	virtual ~functor_by_message();

public:
	virtual bool call();
};
