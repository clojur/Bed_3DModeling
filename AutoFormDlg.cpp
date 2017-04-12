//// AppendixDlg.cpp : implementation file
////
//
#include "stdafx.h"
//#include "resource.h"
//#include "AutoFormDlg.h"
//#include "DbUnity/db_table_templates.h"
//#include "DbUnity/db_dialog.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//#define IDS_TABLEGRID_START 100
//#define IDS_TABLEGRID_END   200
//
//
//// CAutoFormDlg dialog
//
//IMPLEMENT_DYNAMIC(CAutoFormDlg, CXTPResizeDialog)
//
//CAutoFormDlg::CAutoFormDlg(CWnd* pParent /*=NULL*/)
//: CXTPResizeDialog(IDD_APPENDIXDLG, pParent)
//{
//	m_strTitle = _T("子表");
//	m_strInstName = _T("");
//	m_pSubTable = NULL;
//	m_pDatabase = NULL;
//	m_pUserDefineFunc = NULL;
//	m_hIcon = AfxGetApp()->LoadIcon(100);
//
//}
//
//CAutoFormDlg::~CAutoFormDlg()
//{
//	delete m_pSubTable;
//	m_pSubTable = NULL;
//	delete m_pDatabase;
//	m_pDatabase = NULL;
//}
//
//void CAutoFormDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CXTPResizeDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_SINGLEGRID, m_staticBorder);
//
//}
//
//void CAutoFormDlg::AddDatabase(CTHDataBase* pDatabase)
//{
//	m_pDatabase = pDatabase;
//}
//
//
//BEGIN_MESSAGE_MAP(CAutoFormDlg, CXTPResizeDialog)
////	ON_WM_SIZE()
//	ON_BN_CLICKED(IDOK, &CAutoFormDlg::OnBnClickedOk)
//	ON_BN_CLICKED(IDCANCEL, &CAutoFormDlg::OnBnClickedCancel)
//	ON_NOTIFY_EX_RANGE(GN_BUTTONCLICKED, IDS_TABLEGRID_START,IDS_TABLEGRID_END,OnButtonClocked)
//	ON_NOTIFY_EX_RANGE(GN_CELLCHANGE_POST, IDS_TABLEGRID_START, IDS_TABLEGRID_END, OnCellValueChanged)
//	ON_NOTIFY_EX_RANGE(GN_INSERTROWS_PRE, IDS_TABLEGRID_START, IDS_TABLEGRID_END, OnRowInsertPre)
//	ON_NOTIFY_EX_RANGE(GN_REMOVEROWS_PRE, IDS_TABLEGRID_START, IDS_TABLEGRID_END, OnRowDelPre)
//	ON_WM_PAINT()
// 	ON_WM_DESTROY()
//
//END_MESSAGE_MAP()
// 
//
//// CAutoFormDlg message handlers
//
//BOOL CAutoFormDlg::OnInitDialog() 
//{
//	CXTPResizeDialog::OnInitDialog();
//
//	set_window_icon_same_as_application(this);
//
//
//	SetResize(IDC_SINGLEGRID, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);
// 	SetResize(IDOK,     XTP_ANCHOR_BOTTOMRIGHT, XTP_ANCHOR_BOTTOMRIGHT);
//	SetResize(IDCANCEL, XTP_ANCHOR_BOTTOMRIGHT, XTP_ANCHOR_BOTTOMRIGHT);
//
//
//	{
//		CXTPWindowRect rcBorder(&m_staticBorder);
//		ScreenToClient(&rcBorder);
//
//		// get the size of the thumb box in a horizontal scroll bar.
//		int cy = ::GetSystemMetrics(SM_CXHTHUMB); // SM_CYHSCROLL
//
//		CRect rTabCtrl = rcBorder;
//		rTabCtrl.DeflateRect(2, 2);
//
//		CRect rcCtrl = rcBorder;
//		rcCtrl.DeflateRect(2, 1, 2, cy + 1);
//
//		// Create  the flat tab control.
//		m_rTabCtrl = rTabCtrl;
//	}
//	SetWindowText(m_strTitle);
//
//	
//	SetRedraw(FALSE);
//
//	BOOL bResize = FALSE;
//
//	int iSel = 0;
//	CXTPEmptyRect rTab;
//	RemoveResize(IDC_SINGLEGRID);
//
//
//	if (m_pSubTable == NULL)
//	{
//		m_pSubTable = new CTHExcel();
//		ASSERT(m_pSubTable);
//		m_pSubTable->CreateDirect(this, IDC_SINGLEGRID, m_rTabCtrl, FALSE);
//	}
//	m_pSubTable->ModifyStyleEx(NULL, WS_EX_STATICEDGE);
//	
//	if (m_pDatabase == NULL)
//	{
//		m_pDatabase = db_table_templates::create_data_base(m_strInstName);
//
// 		if (m_pDatabase == NULL)
//		{
//			_ASSERTE(("创建DataBase失败", FALSE));
//			return FALSE;
//		}
//	}
//
//	m_pSubTable->SetUsingDataBase(m_pDatabase);
//
//	m_pDatabase->SetRowHeight(0, 0);
//
//	int nCurrentWidth = 0;
//	int totalH = 0;
//	{
//		for (int j = 0; j < m_pDatabase->GetColCount(); ++j)
//		{
//			nCurrentWidth += m_pDatabase->GetColWidth(j);
//		}
//		nCurrentWidth += 70;// 20为滚动条
//
//		for (int j = 0; j < m_pDatabase->GetRowCount(); j++)
//		{
//			totalH += m_pDatabase->GetRowHeight(j);
//		}
//		totalH += 20;
// 	}
//
//	m_pSubTable->SetWindowPos(&wndTop, 0, 0, 0, 0,
//		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
//	SetResize(IDC_SINGLEGRID, XTP_ANCHOR_TOPLEFT, XTP_ANCHOR_BOTTOMRIGHT);
//
//	if (bResize)
//	{
//		CXTPResize::Size();
//	}
//	if (totalH > 500)
//		nCurrentWidth += 30;
//
//	if (nCurrentWidth > 1020)
//		nCurrentWidth = 1020;
//
//	MoveWindow(0, 0, nCurrentWidth, 500);
//	CenterWindow(NULL);
//
//	SetRedraw(TRUE);
//	RedrawWindow(0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
//
//	return TRUE;
//}
////
////void CAutoFormDlg::OnSize(UINT nType, int cx, int cy)
////{
////	CXTPResizeDialog::OnSize(nType, cx, cy);
////
////	if (m_pSubTable != NULL && m_pSubTable->GetSafeHwnd() != NULL)
////	{
////		m_pSubTable->MoveWindow (0, 0, cx, cy-30);
////	}
////
////	if (GetDlgItem(IDOK)->GetSafeHwnd() != NULL && GetDlgItem(IDCANCEL)->GetSafeHwnd() != NULL)
////	{
////		CRect rect;
////		GetDlgItem(IDOK)->GetWindowRect(rect);
////		ScreenToClient(&rect);
////		GetDlgItem(IDOK)->MoveWindow(cx/3-rect.Width()/2, cy-rect.Height(), rect.Width(), rect.Height());
////		GetDlgItem(IDCANCEL)->MoveWindow(cx*2/3-rect.Width()/2, cy-rect.Height(), rect.Width(), rect.Height());
////	}
////}
//
//void CAutoFormDlg::OnBnClickedOk()
//{
//	if (PreOnClose() )
//		OnOK();
//	return;
//}
//
//void CAutoFormDlg::OnBnClickedCancel()
//{
//	// TODO: Add your control notification handler code here
//	OnCancel();
//}
//
//BOOL CAutoFormDlg::PreOnClose()
//{
//	return TRUE;
//}
//
//BOOL CAutoFormDlg::OnButtonClocked( UINT nID, NMHDR * pNotifyStruct, LRESULT* result )
//{
//	if (!m_pDatabase)
//	{
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	if (m_pUserDefineFunc)
//	{
//		return (*m_pUserDefineFunc)(nID, pNotifyStruct, result);
//	}
//	return TRUE;
//}
//
//
//BOOL CAutoFormDlg::OnCellValueChanged(UINT nID, NMHDR * pNotifyStruct, LRESULT* result)
//{
//	if (!m_pDatabase)
//	{
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	NM_THGRID *pMsg = (NM_THGRID *)pNotifyStruct;
//	if (pMsg == NULL)
//	{
//		return FALSE;
//	}
//	int row = pMsg->rowNo;
//	int col = pMsg->colNo;
//	if ( m_strTitle == "用户坐标系")
//	{
//		if (col == 5)
//		{
//			CString strFlag = m_pDatabase->GetCell(row, col)->GetText();
//			if (strFlag == "1")
//			{
//				int nCount = m_pDatabase->GetRowCount();
//				for (int i = 2; i < nCount; i++)
//				{
//					if (i != row)
//					{
//						m_pDatabase->Set_Prop(i - 2, "bcur", FALSE);
//
//					}
//				}
//			}
//		}
//	} 
//	
//	return FALSE;
// }
//
//BOOL CAutoFormDlg::OnRowDelPre(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam)
//{
//	if (!m_pDatabase)
//	{
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	NM_THGRID *pMsg = (NM_THGRID *)pNotifyStruct;
//	if (pMsg == NULL)
//	{
//		return FALSE;
//	}
//	int row = pMsg->rowNo;
//	int col = pMsg->colNo;
//
//	if (m_strTitle == "用户坐标系")
//	{
//		if (row < 6)
//			*lParam = TRUE;
//		return TRUE;
//	}
//	else
//		return FALSE;
//}
//BOOL CAutoFormDlg::OnRowInsertPre(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam)
//{
//	if (!m_pDatabase)
//	{
//		ASSERT(FALSE);
//		return FALSE;
//	}
//	NM_THGRID *pMsg = (NM_THGRID *)pNotifyStruct;
//	if (pMsg == NULL)
//	{
//		return FALSE;
//	}
//	int row = pMsg->rowNo;
//	int col = pMsg->colNo;
//
//	if (m_strTitle == "用户坐标系")
//	{
//		if (row <6)
//			*lParam = TRUE;
//		return TRUE;
//	}
//	else
//		return FALSE;
//}
