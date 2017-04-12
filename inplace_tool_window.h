#pragma once


// 对于自定义的就地窗口，需要从此基类派生
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
	
	// 获取窗口关闭时的状态标示，如果窗口中的数据有效则为true，如果用户取消窗口窗口或失去焦点导致窗口销毁则返回false;
	bool get_validate_flag() const;
	void set_validate_flag(bool flag);
};

// 对外使用的工具类,避免用户进行窗口操作，便于内部维护和外部使用(WYZ)
class inplace_tool_window
{
public:
	// 显示一个文本框并输入数据,调用此函数会将strText中的内容显示到浮动文本框中,输入完成后会将新值存入strText中, 如果输入有效，则返回true，无效则返回false
	static bool inplace_textbox(IN OUT CString& strText, IN COLORREF text_color = RGB(0, 0, 0), IN COLORREF back_color = RGB(200, 200, 200), IN COLORREF border_color = RGB(0, 0, 0));
	
	// 显示一个ComboBox并传回选择的文本, 如果输入有效，则返回true，无效则返回false
	static bool inplace_combobox(IN OUT CString& strText, IN std::vector<CString>& all_items, IN bool can_edit = false);

	// 显示一个定义的就地窗口，指针由外部负责维护,且关联的窗口如果未创建则内部会自动创建，否则直接使用已经创建好的窗口
	// window_rect指定就地窗口的的位置和大小，位置使用屏幕坐标系
	// 如果输入有效，则返回true，无效则返回false
	static bool track_custom_inplace_window(custom_inplace_window_base* wnd, const CRect& windows_rect);
	
private:
	// 禁止实例化、以及拷贝构造
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
