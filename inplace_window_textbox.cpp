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
	CDC* dc = GetDC(); // ����Ҫ�µ�dc,����ʹ�ô����pDC������Ĳ�������ֱ�ӻ��Ƶ������ϣ�Ҫ�˺������غ����ϵͳ���Ƶ�������
	if (dc && dc->GetSafeHdc())
	{
		CRect rect;
		GetWindowRect(&rect); // ����ͨ���ͻ����õ��߿���Ϊ�ͻ������Ķ����Ա������ִ�ֱ���У�Ҫͨ�������������õ�
		rect.MoveToXY(0, 0);
		CBrush border_brush(m_border_color);
		dc->FrameRect(rect, &border_brush); // ���ƾ��α߿�
	}

	pDC->SetTextColor(m_text_color);
	pDC->SetBkColor(m_back_color);
	return m_back_brush; // ���ػ�ˢ,�������������ؼ�����
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

	// �ڴ���CEdit��ʱ����Ӷ��е����ԣ�ES_MULTILINE,��Ϊ��Ҫ�������ڴ�ֱ�����о��У�����ͨ������CEdit�Ŀͻ���λ��
	// ��ʵ�֣����ڵ��е�CEdit�ǲ������ù�������ģ����������˶��У����������ֹ�˴�ֱ�������ͻ��У���ô��������ʵ����һ��
	DWORD edit_style = WS_VISIBLE | WS_BORDER | WS_TABSTOP | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE /* | ES_WANTRETURN*/;
	m_edit.Create(edit_style, CRect(), this, IDC_EDIT_FIRST);
	
	CFont ft;
	ft.CreateStockObject(DEFAULT_GUI_FONT);
	m_edit.SetFont(&ft);

	// ��������ʱ�Ὣ���ص��ı�ȫѡ�У����Ҫ�Ƚ������Ƶ��ı�����
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

	// ���û��෽�������ٴ��ڣ����Ҫ��ȡ�������ٵ���
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
	// �����ر�Ҳ���ܴ�������Ϣ����ʱ���ܻ��󽫱�־����Ϊfalse����˲��ܵ���close_inplace_window(false)����Ӧ
	// �ò�����������Ч��־
	close_inplace_window(get_validate_flag());
}

void inplace_edit::on_en_change_text()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// �Ӵ��ڻ��ڶԻ�����ִ�г�ʼ��,�����ı�������ݸı�,�����ñ�����,��ʱ�Ի���û����ɳ�ʼ��,
	// ��ȡ�Ĵ��ڳߴ�Ҳ�Ǵ����,һ�����е����ڵ����Ĵ�����ͻᵼ�´���,���Լ��ϴ����Ƿ���ӵ��ж�
	// �������Ա�֤�Ի����ʼ��ʱ������ȷ���õ������ڴ�С�Ĵ���,�����ı����ʼ��ʱ���ݸı�Ҳ����
	// ���´��ڵ���������.
	// ������Ƿ���ӷŵ��������ڵĴ�����߻ᵼ�¶Ի����ʼ��������ʾʱ�ı��򲻻�������ݵ�����С

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
	CSize cs = dc.GetTextExtent(strText + _T("  ")); // ����ַ�������(����������ո�ĳ��ȣ��ұ�������հ�

	if (cs.cx < 40) cs.cx = 40;
	if (cs.cx > 800) cs.cx = 800;

	CRect parent_rect;
	parent->GetWindowRect(&parent_rect);

	CRect rect_desktop;
	GetDesktopWindow()->GetWindowRect(&rect_desktop);

	// ���㴰�ڴ�С�������Ƶ�ԭʼλ�ã������Ļ���������Сʱ���Ի�ԭ��ԭ��λ��
	CRect rect = parent_rect;
	rect.right = rect.left + cs.cx;
	// rect.bottom = rect.top + 25; // �߶ȱ����ⲿ���õĲ���

	// ������Խ����Ļʱ�Զ�������������������Խ����Ļ�����Ƶ�0��
	if (rect.right > rect_desktop.right) rect.MoveToX(rect_desktop.right - rect.Width());
	if (rect.left < 0) rect.MoveToX(0);

	// �������໥λ��
	parent->MoveWindow(rect); // �ȰѸ����ڷ��ú�
	rect.MoveToXY(0, 0); // �Ƶ������ڵ����Ͻ�
	MoveWindow(rect); // �ȸĴ��ڴ�С
	m_edit.MoveWindow(rect); // �ٸ��ı����С
		
	// �����ִ�ֱ����
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int font_height = tm.tmHeight/* + tm.tmExternalLeading*/; //��ȡ����ĸ߶�
	
	//�������嵽�����ľ��벢��������������ƫ��
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

