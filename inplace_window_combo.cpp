#include "StdAfx.h"

#include "resource.h"
#include "LocalResource.h"
#include "inplace_tool_window.h"

 #include "string_tools.h"

#include "inplace_window_combo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


#define  IDC_COMBO_FIRST  1001


IMPLEMENT_DYNAMIC(inplace_combo, custom_inplace_window_base)
BEGIN_MESSAGE_MAP(inplace_combo, custom_inplace_window_base)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_CBN_SELCHANGE(IDC_COMBO_FIRST, on_cbn_select_change)
	ON_CBN_KILLFOCUS(IDC_COMBO_FIRST, on_cbn_kill_focus)
END_MESSAGE_MAP()

inplace_combo::inplace_combo()
{
}

inplace_combo::~inplace_combo()
{
}

BOOL inplace_combo::OnCreate(LPCREATESTRUCT lp)
{
	custom_inplace_window_base::OnCreate(lp);

	DWORD edit_style = WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CHILD | ES_AUTOHSCROLL | CBS_DROPDOWNLIST;
	m_combo.Create(edit_style, CRect(), this, IDC_COMBO_FIRST);

	CFont ft;
	ft.CreateStockObject(DEFAULT_GUI_FONT);
	m_combo.SetFont(&ft);
	m_combo.SetFocus();

	set_data_to_ui();

	return TRUE;
}

void inplace_combo::close_inplace_window(bool save_data_before_close)
{
	if (save_data_before_close)
	{
		CString old_text = m_current_selected_item;
		get_data_from_ui();

		// 调用基类方法会销毁窗口，因此要先取好数据再调用
		custom_inplace_window_base::close_inplace_window(save_data_before_close);
	}
	else
	{
		custom_inplace_window_base::close_inplace_window(false);
	}
}

void inplace_combo::set_data_to_ui()
{
	if (!GetSafeHwnd() || !m_combo.GetSafeHwnd()) return;

	CWnd* parent = GetParent();
	if (!parent) return;

	//m_combo.LockWindowUpdate();

	CRect parent_rect;
	parent->GetWindowRect(&parent_rect);

	int max_width = parent_rect.Width();
	CDC* combo_dc = m_combo.GetDC();
	combo_dc->SelectObject(m_combo.GetFont());

	m_combo.ResetContent();
	for (const CString& item : m_all_list_items)
	{
		m_combo.AddString(item);

		// 多留25个像素，避免滚动条把字给遮住
		int cur_text_width = cdb_string_tools::get_text_width_in_px(item, combo_dc) + 25;
		if (cur_text_width > max_width) max_width = cur_text_width;
	}

	m_combo.SelectString(-1, m_current_selected_item);

	parent_rect.right = parent_rect.left + max_width;
	parent->MoveWindow(parent_rect);

	parent_rect.MoveToXY(0, 0);
	m_combo.MoveWindow(parent_rect);
	m_combo.SetDroppedWidth(max_width);

	//m_combo.UnlockWindowUpdate();
	//m_combo.UpdateWindow();
}

void inplace_combo::get_data_from_ui()
{
	if (!GetSafeHwnd() || !m_combo.GetSafeHwnd()) return;

	m_combo.GetWindowText(m_current_selected_item);
}

void inplace_combo::on_cbn_select_change()
{
	close_inplace_window(true);
}

void inplace_combo::on_cbn_kill_focus()
{
	// 正常关闭也可能触发此消息，此时可能会误将标志设置为false，因此不能调用close_inplace_window(false)，而应
	// 该不干涉数据有效标志
	close_inplace_window(get_validate_flag());
}

void inplace_combo::OnSize(UINT flag, int cx, int cy)
{
	window_layout();
}

void inplace_combo::OnSizing(UINT flag, LPRECT sz)
{
	window_layout();
}

void inplace_combo::window_layout()
{
	if (!GetSafeHwnd() || !m_combo.GetSafeHwnd()) return;

	CWnd* parent = GetParent();
	if (!parent) return;

	CRect parent_rect;
	parent->GetWindowRect(&parent_rect);

	parent_rect.MoveToXY(0, 0);
	m_combo.MoveWindow(parent_rect);
}

