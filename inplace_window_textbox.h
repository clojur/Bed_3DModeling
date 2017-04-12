#pragma once
#include "inplace_tool_window.h"

// 配合就地文本编辑窗口并能处理Enter和ESC的消息
class enter_esc_edit : public CEdit
{
	DECLARE_DYNAMIC(enter_esc_edit)
	DECLARE_MESSAGE_MAP()

public:
	enter_esc_edit();
	virtual ~enter_esc_edit();

public:
	COLORREF	m_text_color;			// 字体颜色
	COLORREF	m_back_color;			// 文本框背景颜色
	COLORREF	m_border_color;			// 文本框边框颜色

protected:
	CBrush		m_back_brush;			// 文本框背景画刷

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

// 就地文本编辑窗口
class inplace_edit : public custom_inplace_window_base
{
	DECLARE_DYNAMIC(inplace_edit)
	DECLARE_MESSAGE_MAP()

public:
	inplace_edit();
	virtual ~inplace_edit();

public:
	CString		m_input_text;		// 文本框中的内容，窗口创建时会将此值自动填充到界面上，完成输入后会写回到此变量，如果用户没有确认输入，请不要改这个值，保持创建时传入的值用于识别改动是否有效
	COLORREF	m_text_color;		// 字体颜色
	COLORREF	m_back_color;		// 文本框背景颜色
	COLORREF	m_border_color;		// 文本框边框颜色

protected:
	virtual BOOL OnCreate(LPCREATESTRUCT lp);
	virtual void close_inplace_window(bool save_data_before_close);
	virtual void initialize_finished();
	virtual void set_data_to_ui();
	virtual void get_data_from_ui();

protected:
	afx_msg void on_en_kill_focus();
	afx_msg void on_en_change_text();
	afx_msg void OnSize(UINT flag, int cx, int cy);
	afx_msg void OnSizing(UINT flag, LPRECT sz);

private:
	// 获取文本框中的字符长度,并自动计算对话框尺寸,当长度超过800个像素时不再扩大,或者小于30个像素时不再缩小
	void resize_textbox_by_text();

private:
	enter_esc_edit m_edit;

private:
	friend class enter_esc_edit;
};
