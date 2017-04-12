#pragma once

#include "osg/LineWidth"
#include "osg/StateSet"
#include "osg/StateAttribute"
#include "osg/State"
#include <vector>

class CDbOSGManager;
class CRenderingThread;
class CViewControlData;

// 注意这些id要与主窗口的菜单、ribbon的id范围区分开，否则消息响应会映射错
#define ID_OSG_MENU_COMMAND_ID_BASE		(WM_USER + 10000)
#define ID_OSG_MENU_COMMAND_ID_MIN		ID_OSG_MENU_COMMAND_ID_BASE			    // osg窗口的命令或菜单最小可用ID,11024
#define ID_OSG_MENU_COMMAND_ID_MAX		(ID_OSG_MENU_COMMAND_ID_BASE + 2000)	// osg窗口的命令或菜单最大可用ID 13024



/*
三维图视图类， 需要和CDbOSGManagerInterface实例配合使用
*/
class DB_3DMODELING_API CDbOSGBaseView : public CView 
{
	DECLARE_DYNCREATE(CDbOSGBaseView)

public:
	CDbOSGBaseView();        
	virtual ~CDbOSGBaseView();

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMenuCommand(UINT nID);
	afx_msg LRESULT OnDblClick(WPARAM wParam, LPARAM lParem);
	afx_msg BOOL CDbOSGBaseView::OnEraseBkgnd(CDC* pDC);

public:

	CViewControlData* FindControlData();
	CViewControlData* FindControlData_const() const;

	/*
	命令行处理处理 ，转发到osg对象处理 
	*/
	bool OnCommand(const CString& strCommand);	 
	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);

	virtual void OnDraw(CDC* pDC); 
 
	/*
	初始化
	*/
	virtual void OnInitialUpdate(); 

	void CancleThread();

 
	virtual bool InitOSG(CViewControlData* pOSG);

  protected:
	CDbOSGManager* mOSG;
	CRenderingThread* mThreadHandle;
	CWinThread*           m_pThreadHandle;
	int                            m_nThreadStatus; //0 normal ，1 PauseThread状态 ， -1未激活

	std::map<int ,HCURSOR> _mouseCursorMap;

public:
	BOOL m_bInit;
	BOOL m_bShowMenu;
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};