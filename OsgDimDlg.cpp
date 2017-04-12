// OsgDimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OsgDimDlg.h"
#include "afxdialogex.h"
#include "Resource.h"



// CDbOSGManagerDimDlg dialog

IMPLEMENT_DYNAMIC(CDbOSGManagerDimDlg, CDialogEx)

CDbOSGManagerDimDlg::CDbOSGManagerDimDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(IDD_DIALOG_SDIM, pParent)
	, m_strDimValue(_T(""))
{
	m_xPos = 0;
	m_yPOs = 0;

}

CDbOSGManagerDimDlg::~CDbOSGManagerDimDlg()
{
}

void CDbOSGManagerDimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strDimValue);
}


BEGIN_MESSAGE_MAP(CDbOSGManagerDimDlg, CDialogEx)
END_MESSAGE_MAP()


// CDbOSGManagerDimDlg message handlers
BOOL CDbOSGManagerDimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(m_strDimName); 
	//refactor by wr;
	//set_window_icon_same_as_application(this);

	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_EDIT1);
	if (pCombo)
	{
		pCombo->SetFocus();
		for (int i = 0; i < m_vecLable.size(); i++)
		{
			pCombo->AddString(m_vecLable[i]);
		}
	}
	if (m_xPos != 0)
	{
		SetWindowPos(NULL, m_xPos, m_yPOs, 0, 0, SWP_NOSIZE);
	}

	return FALSE;
}
