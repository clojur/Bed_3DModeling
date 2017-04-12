#pragma once
#include "inplace_tool_window.h"

// ��Ͼ͵��ı��༭���ڲ��ܴ���Enter��ESC����Ϣ
class enter_esc_edit : public CEdit
{
	DECLARE_DYNAMIC(enter_esc_edit)
	DECLARE_MESSAGE_MAP()

public:
	enter_esc_edit();
	virtual ~enter_esc_edit();

public:
	COLORREF	m_text_color;			// ������ɫ
	COLORREF	m_back_color;			// �ı��򱳾���ɫ
	COLORREF	m_border_color;			// �ı���߿���ɫ

protected:
	CBrush		m_back_brush;			// �ı��򱳾���ˢ

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};

// �͵��ı��༭����
class inplace_edit : public custom_inplace_window_base
{
	DECLARE_DYNAMIC(inplace_edit)
	DECLARE_MESSAGE_MAP()

public:
	inplace_edit();
	virtual ~inplace_edit();

public:
	CString		m_input_text;		// �ı����е����ݣ����ڴ���ʱ�Ὣ��ֵ�Զ���䵽�����ϣ����������д�ص��˱���������û�û��ȷ�����룬�벻Ҫ�����ֵ�����ִ���ʱ�����ֵ����ʶ��Ķ��Ƿ���Ч
	COLORREF	m_text_color;		// ������ɫ
	COLORREF	m_back_color;		// �ı��򱳾���ɫ
	COLORREF	m_border_color;		// �ı���߿���ɫ

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
	// ��ȡ�ı����е��ַ�����,���Զ�����Ի���ߴ�,�����ȳ���800������ʱ��������,����С��30������ʱ������С
	void resize_textbox_by_text();

private:
	enter_esc_edit m_edit;

private:
	friend class enter_esc_edit;
};
