#pragma once

#include "inplace_tool_window.h"


// 就地组合窗口
class inplace_combo : public custom_inplace_window_base
{
	DECLARE_DYNAMIC(inplace_combo)
	DECLARE_MESSAGE_MAP()

public:
	inplace_combo();
	virtual ~inplace_combo();

public:
	std::vector<CString>	m_all_list_items;
	CString					m_current_selected_item;

protected:
	virtual BOOL OnCreate(LPCREATESTRUCT lp);
	virtual void close_inplace_window(bool save_data_before_close);
	virtual void set_data_to_ui();
	virtual void get_data_from_ui();

protected:
	afx_msg void on_cbn_select_change();
	afx_msg void on_cbn_kill_focus();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT flag, int cx, int cy);
	afx_msg void OnSizing(UINT flag, LPRECT sz);

private:
	void window_layout();

private:
	CComboBox m_combo;
};
