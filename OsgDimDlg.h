#pragma once


// CDbOSGManagerDimDlg dialog

class CDbOSGManagerDimDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDbOSGManagerDimDlg)

public:
	CDbOSGManagerDimDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDbOSGManagerDimDlg();

// Dialog Data
 	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDimValue;
	CString m_strDimName;

	int m_xPos;
	int m_yPOs;

	std::vector<CString> m_vecLable;
};
