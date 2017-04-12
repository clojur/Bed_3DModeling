#pragma once
class CTHExcel;
class CTHDataBase;

// CAutoFormDlg dialog

typedef BOOL (*CellClickedFdn)(UINT , NMHDR * , LRESULT* );


//class DB_3DMODELING_API CAutoFormDlg : public CXTPResizeDialog
//{
//	DECLARE_DYNAMIC(CAutoFormDlg)
//
//public:
//	CAutoFormDlg(CWnd* pParent = NULL);   // standard constructor
//	virtual ~CAutoFormDlg();
//
//protected:
//	HICON m_hIcon;
//
//public:
//	CStatic	m_staticBorder;
//	CRect m_rTabCtrl;
//	CTHExcel *m_pSubTable;
//	CTHDataBase *m_pDatabase;
//	CString m_strTitle;
//	CString m_strInstName;
//
//	CellClickedFdn* m_pUserDefineFunc;
//public:
//	void AddDatabase(CTHDataBase*);
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//
//private:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnSize(UINT nType, int cx, int cy);
//	DECLARE_MESSAGE_MAP()
//	afx_msg void OnBnClickedOk();
//	afx_msg void OnBnClickedCancel();
//	afx_msg BOOL OnButtonClocked(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam);
//
//	afx_msg BOOL OnCellValueChanged(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam);
//	afx_msg BOOL OnRowDelPre(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam);
//	afx_msg BOOL OnRowInsertPre(UINT nID, NMHDR * pNotifyStruct, LRESULT* lParam);
//public:
//	virtual BOOL PreOnClose();
// };