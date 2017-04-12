// FullBridgeView3D.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"

#include "OSGWraper.h"
#include "osgtools.h"
#include "BoxSelector.h"
#include "OSGBaseView.h"
#include "IDb3DCommandLine.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CDbOSGBaseView, CView)

CDbOSGBaseView::CDbOSGBaseView()
{
	m_bInit = FALSE;
	m_bShowMenu = FALSE;

	mOSG = NULL;
	mThreadHandle = NULL;
	m_pThreadHandle = NULL;
}

CDbOSGBaseView::~CDbOSGBaseView()
{

}

BEGIN_MESSAGE_MAP(CDbOSGBaseView, CView)
	  //{{AFX_MSG_MAP(CDbOSGBaseView)
	  ON_WM_CREATE()
	  ON_WM_DESTROY()
	  ON_WM_KEYDOWN()
	  ON_WM_CHAR()
	  ON_WM_SETCURSOR()
	  ON_COMMAND_RANGE(ID_OSG_MENU_COMMAND_ID_MIN, ID_OSG_MENU_COMMAND_ID_MAX, OnMenuCommand)
  
	  ON_MESSAGE(DB_DBLCLICKOBJ, OnDblClick)
	 //}}AFX_MSG_MAP
	 ON_WM_TIMER()
END_MESSAGE_MAP() 

LRESULT CDbOSGBaseView::OnDblClick(WPARAM wParam, LPARAM lParem)
{
	CDbObjInterface* pObj = (CDbObjInterface*)wParam;
	if (pObj)
	{	 
		//refactor by wr
		BOOL flag= pObj->OnDblClick();
 
	}
	return 1;
}

void CDbOSGBaseView::OnMenuCommand(UINT nID)
{
 	CViewControlData* pOSG = FindControlData();
	if (pOSG)
	{
		pOSG->OnMenuCommand(nID);
	}
 }

int CDbOSGBaseView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// ±ÜÃâÉÁË¸
	lpCreateStruct->style |= ( WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	return 0;
}

void CDbOSGBaseView::OnDraw(CDC* /*pDC*/)
{
}
 

void CDbOSGBaseView::OnDestroy()
{
	if (mThreadHandle)
	{
		delete mThreadHandle;
		if (mOSG)
		{
			delete mOSG;
			mOSG = NULL;
		}
	}
	CView::OnDestroy();
}

void CDbOSGBaseView::CancleThread()
{ 
	mOSG->done = false; 
	CViewControlData* pControlData = FindControlData();
	if (pControlData)
	{
		pControlData->bCancle = TRUE;
	}
	
	if (mThreadHandle)
	{
		mThreadHandle->_done = true;
		mThreadHandle->cancel();
	}
}

void CDbOSGBaseView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->OnText(nChar, nRepCnt, nFlags);
	}
	
 
}

void CDbOSGBaseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
 	if(mOSG)
	{
		bool flag = mOSG->OnKeyDown(nChar , nRepCnt , nFlags);
	}
}


BOOL CDbOSGBaseView::OnEraseBkgnd(CDC* pDC)
{		
	return CView::OnEraseBkgnd(pDC);
}
  
bool CDbOSGBaseView::pre_do_new_command(const CString& domain, const CString& command, const CString& param)
{
	bool bRes = false;
	if (mOSG)
	{
		bRes = mOSG->pre_do_new_command(domain,command,param);
	}
	//if (bRes)
	//{
	//	SetFocus();
	//}
	return bRes;
}

bool CDbOSGBaseView::OnCommand(const CString& strCommand)
{
	bool bRes = false;
	if (mOSG)
	{
		bRes = mOSG->OnCommand(strCommand);
	}
	//if (bRes)
	//{
	//	SetFocus();
	//}
	return bRes;
}


void CDbOSGBaseView::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
	if (mOSG)
	{
		mOSG->OnCommandUpdate(strCommand, enable, checked);
	}
}


CViewControlData* CDbOSGBaseView::FindControlData()
{
	if (mOSG)
	{
		return mOSG->FindControlData();
	}
	return NULL;
}

CViewControlData* CDbOSGBaseView::FindControlData_const() const
{
	if (mOSG)
	{
		return mOSG->FindControlData();
	}
	return NULL;
}


bool CDbOSGBaseView::InitOSG(CViewControlData* pOSG)
{
	if (pOSG)
	{
		pOSG->mCView = this;

		mOSG = new CDbOSGManager(this); 
		mOSG->InitOSG(pOSG);
		mThreadHandle = new CRenderingThread(mOSG);
		mThreadHandle->start();
		return true;
	}
	return false;
}

BOOL CDbOSGBaseView::PreTranslateMessage(MSG* pMsg)
{
	if (!mOSG)
	{
		return __super::PreTranslateMessage(pMsg);
	}

	if (pMsg->message == WM_CHAR)
	{
		SendMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE)
		{
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->OnText(pMsg->wParam, 1, 1);
			}
			return TRUE;
		}
	}
	else if (pMsg->message == WM_MBUTTONDBLCLK)
	{
		mOSG->FullSceen();
		return true;
	}
	if (pMsg->message == WM_RBUTTONDOWN )
	{
		if ( mOSG && mOSG->GetCommandStatus() == 0)
		{
		CViewControlData* pControlData = mOSG->FindControlData();
		if (pControlData)
		{
			MOUSEMODE defaultMouseMode = MM_SHOW_MENU;
			MOUSEMODE editMouseMode    = MM_SHOW_MENU;

			if (pControlData->m_pSelector->GetSelectNodeCount() == 0)//////defaultMode
			{
				if (defaultMouseMode == MM_REPEATLASTCOMMAND)
				{
					if (pControlData->RepeatLastCommand() )
					{
						return TRUE; 
					}
				}			
				else
				{
					CString strInput;
					auto cmdLine = GetDb3DCommandLine();
					if (cmdLine)
					{
						strInput = cmdLine->GetCurText();
					}
				
					if (!strInput.IsEmpty())
					{
						if( cmdLine)
						{
							cmdLine->OnText(VK_SPACE, 1, 1);
						}
					
						return TRUE;
					}
					else
					{
						m_bShowMenu = TRUE;
						pControlData->OnRButtonDown();
						m_bShowMenu = FALSE;
						return TRUE;
					}
				}
			}
			else                                            //editMode
			{
				if (editMouseMode == MM_SHOW_MENU)
				{
					m_bShowMenu = TRUE;
					pControlData->OnRButtonDown();
					m_bShowMenu = FALSE;
					return TRUE;
				}
			}
		}

		}
	}
	return __super::PreTranslateMessage(pMsg);
}

#define IsMidMousepressed() ( (GetAsyncKeyState(VK_MBUTTON) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsSHIFTpressed()    ( (GetKeyState(VK_SHIFT)        & (1 << (sizeof(SHORT)*8-1))) != 0 )
BOOL CDbOSGBaseView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (!mOSG)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		return TRUE;
	}
	else
	{
		if (m_bShowMenu)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return TRUE;
		}
		else
		{
			if (IsMidMousepressed())
			{
				if (IsSHIFTpressed())
				{
					SetCursor(mOSG->GetCursor(OSG_SELECT_ROTATE, _mouseCursorMap));
					return  TRUE;
				}
				else
				{
					SetCursor(mOSG->GetCursor(OSG_SELECT_PAN, _mouseCursorMap));
					return  TRUE;
				}
			}
			else
			{
				int curType = mOSG->userCursorType;
				CViewControlData* pControlData = FindControlData();
				if (pControlData && (abs(pControlData->backColor[0] - 0.5) < 0.05 && abs(pControlData->backColor[1] - 0.5) < 0.05 && abs(pControlData->backColor[2] - 0.5) < 0.05))
				{
					curType += 1000;
				}

				CViewControlData* pOSG = mOSG->FindControlData();
				if (pOSG && pOSG->bRotate)
				{
					SetCursor(mOSG->GetCursor(OSG_SELECT_ROTATE, _mouseCursorMap));
				}
				else
				{
					SetCursor(mOSG->GetCursor(curType, _mouseCursorMap));
				}

 			}
 			
		}	

	}

	return  TRUE;
}


void CDbOSGBaseView::OnInitialUpdate()
{
	if(!m_bInit)
	{
		CView::OnInitialUpdate();
	} 
}
void CDbOSGBaseView::OnTimer(UINT_PTR nIDEvent)
{	
	__super::OnTimer(nIDEvent);
} 

 
void CDbOSGBaseView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
  