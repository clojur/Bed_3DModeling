#include "StdAfx.h"

#include "resource.h"
#include "LocalResource.h"
#include "inplace_tool_window.h"

#include "inplace_window_textbox.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;


#define  IDC_EDIT_FIRST  1001


IMPLEMENT_DYNAMIC(enter_esc_edit, CEdit)
BEGIN_MESSAGE_MAP(enter_esc_edit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

enter_esc_edit::enter_esc_edit()
{
	m_text_color = RGB(0, 0, 0);
	m_back_color = RGB(200, 200, 200);
	m_border_color = RGB(0, 0, 0);
	m_back_brush.CreateSolidBrush(m_back_color);
}

enter_esc_edit::~enter_esc_edit()
{
}

HBRUSH enter_esc_edit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	CDC* dc = GetDC(); // 必须要新的dc,不得使用传入的pDC，传入的参数不能直接绘制到窗口上，要此函数返回后才由系统绘制到窗口上
	if (dc && dc->GetSafeHdc())
	{
		CRect rect;
		GetWindowRect(&rect); // 不能通过客户区得到边框，因为客户区被改动了以便让文字垂直居中，要通过窗口区域来得到
		rect.MoveToXY(0, 0);
		CBrush border_brush(m_border_color);
		dc->FrameRect(rect, &border_brush); // 绘制矩形边框
	}

	pDC->SetTextColor(m_text_color);
	pDC->SetBkColor(m_back_color);
	return m_back_brush; // 返回画刷,用来绘制整个控件背景
}



IMPLEMENT_DYNAMIC(inplace_edit, custom_inplace_window_base)
BEGIN_MESSAGE_MAP(inplace_edit, custom_inplace_window_base)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_EN_KILLFOCUS(IDC_EDIT_FIRST, on_en_kill_focus)
	ON_EN_CHANGE(IDC_EDIT_FIRST, on_en_change_text)
END_MESSAGE_MAP()

inplace_edit::inplace_edit()
{
	m_text_color = RGB(0, 0, 0);
	m_back_color = RGB(200, 200, 200);
	m_border_color = RGB(0, 0, 0);
}

inplace_edit::~inplace_edit()
{
}

BOOL inplace_edit::OnCreate(LPCREATESTRUCT lp)
{
	custom_inplace_window_base::OnCreate(lp);

	m_edit.m_text_color = m_text_color;
	m_edit.m_back_color = m_back_color;
	m_edit.m_border_color = m_border_color;

	// 在创建CEdit的时候添加多行的属性，ES_MULTILINE,因为需要把文字在垂直方向中居中，居中通过设置CEdit的客户区位置
	// 来实现，对于单行的CEdit是不能设置工作区域的，我们设置了多行，但是如果禁止了垂直滚动条和换行，那么看起来其实还是一行
	DWORD edit_style = WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE /* | ES_WANTRETURN*/;
	m_edit.Create(edit_style, CRect(), this, IDC_EDIT_FIRST);
	
	CFont ft;
	ft.CreateStockObject(DEFAULT_GUI_FONT);
	m_edit.SetFont(&ft);

	// 调整窗口时会将加载的文本全选中，因此要先将焦点移到文本框中
	set_data_to_ui();
	resize_textbox_by_text();

	return TRUE;
}

void inplace_edit::close_inplace_window(bool save_data_before_close)
{
	if (save_data_before_close)
	{
		CString old_text = m_input_text;
		get_data_from_ui();
	}

	// 调用基类方法会销毁窗口，因此要先取好数据再调用
	custom_inplace_window_base::close_inplace_window(save_data_before_close);
}

void inplace_edit::initialize_finished()
{
	m_edit.SetFocus();
	m_edit.SetSel(0, m_input_text.GetLength(), FALSE);
}

void inplace_edit::set_data_to_ui()
{
	if (!m_edit.GetSafeHwnd()) return;
	m_edit.SetWindowText(m_input_text);
}

void inplace_edit::get_data_from_ui()
{
	m_input_text = _T("");
	if (!m_edit.GetSafeHwnd()) return;
	m_edit.GetWindowText(m_input_text);
}

void inplace_edit::on_en_kill_focus()
{
	// 正常关闭也可能触发此消息，此时可能会误将标志设置为false，因此不能调用close_inplace_window(false)，而应
	// 该不干涉数据有效标志
	close_inplace_window(get_validate_flag());
}

void inplace_edit::on_en_change_text()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// 子窗口会于对话框先执行初始化,触发文本框的内容改变,并调用本函数,此时对话框还没有完成初始化,
	// 获取的窗口尺寸也是错误的,一旦进行到窗口调整的代码里就会导致错误,所以加上窗口是否可视的判断
	// 这样可以保证对话框初始化时可以正确调用调整窗口大小的代码,而且文本框初始化时内容改变也不会
	// 导致窗口调整出问题.
	// 如果将是否可视放到调整窗口的代码里边会导致对话框初始化初次显示时文本框不会根据内容调整大小

	if (GetSafeHwnd() && IsWindowVisible()) resize_textbox_by_text();
}

void inplace_edit::resize_textbox_by_text()
{
	if (!GetSafeHwnd()) return;
	if (!m_edit.GetSafeHwnd()) return;

	CWnd* parent = GetParent();
	if (!parent) return;

	CString strText;
	m_edit.GetWindowText(strText);

	CClientDC dc(this);
	if (!dc.GetSafeHdc()) return;
	dc.SelectObject(m_edit.GetFont());
	CSize cs = dc.GetTextExtent(strText + _T("  ")); // 获得字符串长度(多计算两个空格的长度，右边留稍许空白

	if (cs.cx < 40) cs.cx = 40;
	if (cs.cx > 800) cs.cx = 800;

	CRect parent_rect;
	parent->GetWindowRect(&parent_rect);

	CRect rect_desktop;
	GetDesktopWindow()->GetWindowRect(&rect_desktop);

	// 计算窗口大小并将其移到原始位置，这样的话当窗口缩小时可以还原到原来位置
	CRect rect = parent_rect;
	rect.right = rect.left + cs.cx;
	// rect.bottom = rect.top + 25; // 高度保持外部设置的不变

	// 当窗口越出屏幕时自动调整，调整后如果左边越出屏幕，则移到0处
	if (rect.right > rect_desktop.right) rect.MoveToX(rect_desktop.right - rect.Width());
	if (rect.left < 0) rect.MoveToX(0);

	// 处理窗口相互位置
	parent->MoveWindow(rect); // 先把父窗口放置好
	rect.MoveToXY(0, 0); // 移到父窗口的左上角
	MoveWindow(rect); // 先改窗口大小
	m_edit.MoveWindow(rect); // 再改文本框大小
		
	// 让文字垂直居中
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight/* + tm.tmExternalLeading*/; //获取字体的高度
	
	//计算字体到顶部的距离并将工作区域往下偏移
	CRect edit_client_rect = rect;
	m_edit.GetClientRect(&edit_client_rect);
	int offset_v = (edit_client_rect.Height() - font_height) / 2;
	edit_client_rect.top += offset_v;
	edit_client_rect.bottom -= offset_v;
	m_edit.SetRectNP(&edit_client_rect);
}

void inplace_edit::OnSizing(UINT flag, LPRECT sz)
{
	resize_textbox_by_text();
}

void inplace_edit::OnSize(UINT flag, int cx, int cy)
{
	resize_textbox_by_text();
}

