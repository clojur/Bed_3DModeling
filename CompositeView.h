#pragma once

 
#include "osg/LineWidth"
#include "osg/StateSet"
#include "osg/StateAttribute"
#include "osg/State"
#include <vector>
#include "osgViewer/View"
 

class CDbCompositeViewOSG;
 
/*
�����ͼ�� ��Ҫ��CDbOSGManagerInterfaceʵ�����ʹ��
*/
class DB_3DMODELING_API CDbCompositeView : public CView
{
	DECLARE_SERIAL(CDbCompositeView)

public:
	CDbCompositeView();        
	virtual ~CDbCompositeView();

	virtual void OnDraw(CDC* pDC);  

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
 	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
 	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual void OnSize(UINT nType, int cx, int cy);

 	afx_msg void OnMenuCommand(UINT nID);

	/*
	�����д����� ��ת����osg������ 
	*/
	bool OnCommand(const CString& strCommand);	 
	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);
	afx_msg LRESULT OnDblClick(WPARAM wParam, LPARAM lParem);

public:

	/*
	��ʼ��
	*/
	virtual void OnInitialUpdate();

	void CancleThread();
	 

	//�̺߳���
	void PauseThread();
	void ResumeThread();
	int  GetCurrentThreadStatus(){ return m_nThreadStatus; };
	void SetCurrentThreadStatus(int nStatus){ m_nThreadStatus = nStatus; }
 protected:
	CDbCompositeViewOSG* mOSG;
 
 	CWinThread*           m_pThread;
	int                   m_nThreadStatus; //0 normal ��1 PauseThread״̬ �� -1δ����

	std::map<int,HCURSOR> _mouseCursorMap;
protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bInit; 
	BOOL m_bShowMenu;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);

	void SetActiveView(osgViewer::View* pView);

 };