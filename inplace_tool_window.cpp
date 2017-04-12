#include "StdAfx.h"

#include <atomic>
#include <afxdialogex.h>

#include "resource.h"
#include "LocalResource.h"

 
#include "inplace_window_textbox.h"
#include "inplace_window_combo.h"

#include "inplace_tool_window.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


class custom_inplace_window_base;

// �ڲ�˽�õ��࣬���ڳ��ؾ͵ر༭����
class inplace_popup_basic_blank_wnd : public CDialogEx
{
	DECLARE_DYNAMIC(inplace_popup_basic_blank_wnd)
	DECLARE_MESSAGE_MAP()

public:
	inplace_popup_basic_blank_wnd();
	~inplace_popup_basic_blank_wnd();

public:
 
public:
	custom_inplace_window_base* m_outer_wnd;
	bool m_option_is_validate;
	CRect m_start_rect;

public:
	virtual void OnOK();
	virtual void OnCancel();

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	void adjust_window_layout();

protected:
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSizing(UINT type, LPRECT rt);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
};

IMPLEMENT_DYNAMIC(inplace_popup_basic_blank_wnd, CDialogEx)
BEGIN_MESSAGE_MAP(inplace_popup_basic_blank_wnd, CDialogEx)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

inplace_popup_basic_blank_wnd::inplace_popup_basic_blank_wnd() : CDialogEx(IDD_DIALOG_INPLACE_BLANK_BASE_WND)
{
	m_outer_wnd = nullptr;
	m_option_is_validate = false;
}

inplace_popup_basic_blank_wnd::~inplace_popup_basic_blank_wnd()
{
}

BOOL inplace_popup_basic_blank_wnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (!GetSafeHwnd()) return FALSE;

	MoveWindow(m_start_rect);

	if (!m_outer_wnd) return TRUE;

	bool out_wnd_validate = true;
	if (!m_outer_wnd->GetSafeHwnd())
	{
		// ����ʧ�ܲ�Ҫ���أ���ʾ�Ի�����Ŀհ����ӣ��Ա�ʹ�����ܴӽ����Ͽ�������
		if (FALSE == m_outer_wnd->Create(nullptr, nullptr,
			WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/ | WS_CLIPCHILDREN, CRect(), this, 1501))
			out_wnd_validate = false;
	}

	if (out_wnd_validate)
	{
		m_outer_wnd->SetParent(this);
		m_outer_wnd->SetOwner(this);

		// �ٴ��޸ķ���Լ����ڵ�OnCreate��Ϣ��Ӧ���޸��˴���ʱָ���ķ��
		m_outer_wnd->ModifyStyle(WS_BORDER | WS_CAPTION | WS_POPUP | WS_CLIPSIBLINGS, WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/ | WS_CLIPCHILDREN);
		m_outer_wnd->ModifyStyleEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, NULL);
	}

	m_outer_wnd->set_data_to_ui();
	adjust_window_layout();
	m_outer_wnd->initialize_finished();

	return FALSE; // ����FALSEʱ�Ի��򲻻�����ӿؼ��Ľ���
}

BOOL inplace_popup_basic_blank_wnd::PreTranslateMessage(MSG* pMsg)
{
	// ΢��δ�ĵ�������Ϣ����Ҫ�����ı����봦��ĸ߼����ܣ�������ʵ��������ģ̬�����ⲿ���ж�
	//if (0x118 == pMsg->message) OnCancel();

	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
			OnOK();
		else if (VK_ESCAPE == pMsg->wParam)
			OnCancel();
		else
			; // nothing;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void inplace_popup_basic_blank_wnd::OnOK()
{
	if (m_outer_wnd) m_outer_wnd->get_data_from_ui();

	 m_option_is_validate = true;
	 CDialogEx::OnOK();
}

void inplace_popup_basic_blank_wnd::OnCancel()
{
	 m_option_is_validate = false;
	 CDialogEx::OnCancel();
}

void inplace_popup_basic_blank_wnd::adjust_window_layout()
{
	CRect rt;
	GetWindowRect(&rt);
	rt.MoveToXY(0, 0);
	if (m_outer_wnd && m_outer_wnd->GetSafeHwnd()) m_outer_wnd->MoveWindow(&rt);
}

void inplace_popup_basic_blank_wnd::OnSize(UINT type, int cx, int cy)
{
	adjust_window_layout();
}

void inplace_popup_basic_blank_wnd::OnSizing(UINT type, LPRECT rt)
{
	adjust_window_layout();
}

BOOL inplace_popup_basic_blank_wnd::OnEraseBkgnd(CDC* dc)
{
	CRect rc;
	GetWindowRect(&rc);
	rc.MoveToXY(0, 0);

	if (dc)
	{
		CBrush back_brush;
		back_brush.CreateSolidBrush(RGB(240, 240, 240));
		dc->FillRect(&rc, &back_brush);
		dc->SetDCPenColor(RGB(70, 70, 70));
		dc->MoveTo(0, 0);
		dc->LineTo(rc.Width() - 1, 0);
		dc->LineTo(rc.Width() - 1, rc.Height() - 1);
		dc->LineTo(0, rc.Height() - 1);
		dc->LineTo(0, 0);
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

// custom_inplace_window_base��ʵ�ֱ�������ڲ�˽�õ���inplace_popup_basic_blank_wnd����

IMPLEMENT_DYNAMIC(custom_inplace_window_base, CWnd)
BEGIN_MESSAGE_MAP(custom_inplace_window_base, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


custom_inplace_window_base::custom_inplace_window_base()
{
}

custom_inplace_window_base::~custom_inplace_window_base()
{

}

void custom_inplace_window_base::set_data_to_ui()
{

}

void custom_inplace_window_base::get_data_from_ui()
{

}

void custom_inplace_window_base::initialize_finished()
{

}

void custom_inplace_window_base::close_inplace_window(bool return_value /*= false*/)
{
	CWnd* inplace_wnd = GetParent();
	if (!inplace_wnd || !inplace_wnd->GetSafeHwnd()) return;

	inplace_popup_basic_blank_wnd* real_inplace_wnd = dynamic_cast<inplace_popup_basic_blank_wnd*>(inplace_wnd);
	if (!real_inplace_wnd) return;

	if (return_value)
		real_inplace_wnd->OnOK();
	else
		real_inplace_wnd->OnCancel();
}

bool custom_inplace_window_base::get_validate_flag() const
{
	CWnd* inplace_wnd = GetParent();
	if (!inplace_wnd || !inplace_wnd->GetSafeHwnd()) return false;

	inplace_popup_basic_blank_wnd* real_inplace_wnd = dynamic_cast<inplace_popup_basic_blank_wnd*>(inplace_wnd);
	if (real_inplace_wnd) return real_inplace_wnd->m_option_is_validate;

	return false;
}

void custom_inplace_window_base::set_validate_flag(bool flag)
{
	CWnd* inplace_wnd = GetParent();
	if (!inplace_wnd || !inplace_wnd->GetSafeHwnd()) return;

	inplace_popup_basic_blank_wnd* real_inplace_wnd = dynamic_cast<inplace_popup_basic_blank_wnd*>(inplace_wnd);
	if (real_inplace_wnd) real_inplace_wnd->m_option_is_validate = flag;
}


//////////////////////////////////////////////////////////////////////////


bool inplace_tool_window::inplace_textbox(IN OUT CString& strText, COLORREF text_color /*= RGB(0, 0, 0)*/,
	COLORREF back_color /*= RGB(200, 200, 200)*/, COLORREF border_color /*= RGB(0, 0, 0)*/)
{
	// ��������Ļ�ȡ���λ�ã�������С�û�˫���º��ƶ���λ����
	CPoint pt;
	GetCursorPos(&pt);

	inplace_edit float_edit;
	float_edit.m_text_color = text_color;
	float_edit.m_back_color = back_color;
	float_edit.m_border_color = border_color;
	float_edit.m_input_text = strText;
	if (!track_custom_inplace_window(&float_edit, CRect(pt.x - 30, pt.y + 11, pt.x + 30, pt.y + 32)))
	{
		strText = _T("");
		return false;
	}

	strText = float_edit.m_input_text;
	return true;
}

bool inplace_tool_window::inplace_combobox(IN OUT CString& strText, IN std::vector<CString>& all_items, IN bool can_edit /*= false*/)
{
	// ��������Ļ�ȡ���λ�ã�������С�û�˫���º��ƶ���λ����
	CPoint pt;
	GetCursorPos(&pt);

	inplace_combo float_combo;
	float_combo.m_all_list_items = all_items;
	float_combo.m_current_selected_item = strText;
	//float_combo.m_text_color = text_color;
	//float_combo.m_back_color = back_color;
	//float_combo.m_border_color = border_color;
	//float_combo.m_input_text = strText;
	if (!track_custom_inplace_window(&float_combo, CRect(pt.x - 30, pt.y + 11, pt.x + 30, pt.y + 32)))
	{
		strText = _T("");
		return false;
	}

	strText = float_combo.m_current_selected_item;
	return true;
}

class inplace_windows_implement_functor : public functor_by_message
{
public:
	inplace_windows_implement_functor() {}
	virtual ~inplace_windows_implement_functor() {}

public:
	custom_inplace_window_base* m_wnd = nullptr;
	CRect m_windows_rect;

public:
	virtual bool call()
	{
		CRect checked_rect = m_windows_rect;
		if (checked_rect.Width() < 10) checked_rect.right = checked_rect.left + 10;
		if (checked_rect.Height() < 5) checked_rect.bottom = checked_rect.top + 5;
		checked_rect.MoveToY(checked_rect.top - checked_rect.Height());

		CLocalResource tmp_res;
		inplace_popup_basic_blank_wnd dlg;
		dlg.m_outer_wnd = m_wnd;
		dlg.m_start_rect = checked_rect;
		dlg.m_option_is_validate = false;
		dlg.DoModal();

		return dlg.m_option_is_validate;
	}
};

bool inplace_tool_window::track_custom_inplace_window(custom_inplace_window_base* wnd, const CRect& windows_rect)
{

	// ���ڼ��׵ķ�ֹ�����Ҳ���������ʵ���is_doing = false��ʱ�����˵㣬�˴�������ô�ϸ�������߳̿��ƣ�����ν���Ժ�����Ҫ��˵��
	// osg��Ⱦ�̻߳���ô˺���
	static std::atomic<bool> is_doing = false;

	if (is_doing) return false;
	is_doing = true;

	inplace_windows_implement_functor fun;
	fun.m_wnd = wnd;
	fun.m_windows_rect = windows_rect;

	//application_interface* app = get_application_instance();
	//if (!app) return false;
//	bool result = app->do_function_by_message(&fun); // ��Ҫֱ�ӵ���fun->call��������osg��Ⱦ�߳��л�����
	bool result = fun.call() ;  

	is_doing = false;
	return result;
}

inplace_tool_window::inplace_tool_window()
{

}

inplace_tool_window::inplace_tool_window(const inplace_tool_window&)
{

}

inplace_tool_window::~inplace_tool_window()
{

}

const inplace_tool_window& inplace_tool_window::operator=(const inplace_tool_window&)
{
	return *this;
}
 
functor_by_message::functor_by_message()
{

}
functor_by_message::~functor_by_message()
{

}
bool functor_by_message::call()
{
	return false;
}
