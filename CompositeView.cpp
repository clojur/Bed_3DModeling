// FullBridgeView3D.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"

#include "osgtools.h"

#include "multiviewportosg.h"
#include "OSGWraper.h"
#include "BoxSelector.h"
#include "compositeview.h"
#include "IDb3DCommandLine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// 注意这些id要与主窗口的菜单、ribbon的id范围区分开，否则消息响应会映射错
#define ID_OSG_MENU_COMMAND_ID_BASE		(WM_USER + 10000)
#define ID_OSG_MENU_COMMAND_ID_MIN		ID_OSG_MENU_COMMAND_ID_BASE				// osg窗口的命令或菜单最小可用ID,11024
#define ID_OSG_MENU_COMMAND_ID_MAX		(ID_OSG_MENU_COMMAND_ID_BASE + 2000)	// osg窗口的命令或菜单最大可用ID 13024


IMPLEMENT_SERIAL(CDbCompositeView, CView,1)

CDbCompositeView::CDbCompositeView()
{
 	mOSG = NULL;
 	m_bInit = FALSE;
	//mThreadHandle = NULL;
	m_pThread = NULL;
	m_nThreadStatus = 0;
	m_bShowMenu = FALSE;
  }

CDbCompositeView::~CDbCompositeView()
{

}

/////////
BOOL CDbCompositeView::OnEraseBkgnd(CDC* pDC)
{	 
	if (!mOSG || mOSG->mViewer->getNumViews() == 0)
	{
		CRect rect;
		GetClientRect(rect);

		CBrush brush;
		brush.CreateSolidBrush(RGB(0, 0, 0));
		FillRect(pDC->m_hDC, &rect, (HBRUSH)brush);
		brush.DeleteObject();
	} 
	return TRUE;
}

BEGIN_MESSAGE_MAP(CDbCompositeView, CView)
	  //{{AFX_MSG_MAP(CDbOSGBaseView)
	  ON_WM_CREATE()
	  ON_WM_DESTROY() 
	  ON_WM_SETCURSOR()
	  ON_WM_KEYDOWN()
	  ON_WM_CHAR()
	  ON_WM_ERASEBKGND()
	  ON_COMMAND_RANGE(10, 10000,OnMenuCommand)

	 
	  ON_MESSAGE(DB_DBLCLICKOBJ, OnDblClick)

	  ON_COMMAND_RANGE(ID_OSG_MENU_COMMAND_ID_MIN, ID_OSG_MENU_COMMAND_ID_MAX, OnMenuCommand)
END_MESSAGE_MAP()



LRESULT CDbCompositeView::OnDblClick(WPARAM wParam, LPARAM lParem)
{
	CDbObjInterface* pObj = (CDbObjInterface*)wParam;
	if (pObj)
	{
		BOOL flag =  pObj->OnDblClick();
		if (flag)
		{
			//property_grid_interface* pGrid = get_property_grid_instance();
		/*	if (pGrid)
			{
				pGrid->populate(pObj);
			}*/
		}
	}
	return 1;
}

void CDbCompositeView::OnMenuCommand(UINT nID)
{
	CViewControlData* pControlData = mOSG->FindControlData();
	if (pControlData)
	{
		pControlData->OnMenuCommand(nID);
	}
}
bool CDbCompositeView::pre_do_new_command(const CString& domain, const CString& command, const CString& param)
{
	bool bRes = false;
	if (mOSG)
	{
		bRes = mOSG->pre_do_new_command(domain,command,param);
	}
	return bRes;
}

bool CDbCompositeView::OnCommand(const CString& strCommand)
{
	bool bRes = false;
	if (mOSG)
	{
		bRes = mOSG->OnCommand(strCommand);
	}
	return bRes;
}

void CDbCompositeView::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
 	if (mOSG)
	{
		mOSG->OnCommandUpdate(strCommand,enable,checked);
	}
}


#define IsMidMousepressed() ( (GetAsyncKeyState(VK_MBUTTON) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsSHIFTpressed()    ( (GetKeyState(VK_SHIFT)        & (1 << (sizeof(SHORT)*8-1))) != 0 )

BOOL CDbCompositeView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{ 
	if(!mOSG)
	{
		SetCursor(NULL);
		return TRUE;
	}
	else
	{
		if (m_bShowMenu)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return TRUE;
		}

		if ( IsMidMousepressed())
		{
			if(IsSHIFTpressed())
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
			CViewControlData* pControlData =  mOSG->FindControlData();
			if (pControlData && (abs(pControlData->backColor[0] - 0.5) < 0.05 && abs(pControlData->backColor[1] - 0.5) < 0.05 && abs(pControlData->backColor[2] - 0.5) < 0.05))
			{
				curType += 1000;
			}
			if (pControlData && pControlData->bRotate)
			{
				SetCursor(mOSG->GetCursor(OSG_SELECT_ROTATE, _mouseCursorMap));
			}		
			else
			{
				SetCursor(mOSG->GetCursor(curType, _mouseCursorMap));
			}
 		}
	} 
	return  TRUE;
}

int CDbCompositeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;  
	return 0;
}

void CDbCompositeView::OnDraw(CDC* pDC)
{

}
  

void CDbCompositeView::OnDestroy()
{ 	
 

	CView::OnDestroy();
}
void CDbCompositeView::CancleThread()
{
	if (m_pThread)
	{
		m_pThread->ResumeThread();
	}
	mOSG->done = false;
	if (m_pThread)
	{
		DWORD flag = WaitForSingleObject(m_pThread->m_hThread, 3000);
		if (flag != 0)
		{
			DWORD error = GetLastError();
			TerminateThread(m_pThread->m_hThread, flag);
 			m_pThread = NULL;
 		}
	}

 }


void CDbCompositeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(mOSG)
	{
		bool flag = mOSG->OnKeyDown(nChar , nRepCnt , nFlags);
	}
}

void CDbCompositeView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->OnText(nChar, nRepCnt, nFlags);
	}
	
}

void CDbCompositeView::SetActiveView(osgViewer::View* pView)
{
	if (mOSG)
	{
		if (pView != mOSG->GetCurrentView())
		{
	 
			mOSG->SetViewCurrent(pView);
		}
	}
}

 
BOOL CDbCompositeView::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_RBUTTONDOWN  && mOSG && (mOSG->GetCommandStatus() ==0)  )
	{ 
		POINT point;
 		::GetCursorPos(&point);

		osgViewer::View* pCurView =  mOSG->GetCurosrView( point);	 
		SetActiveView(pCurView);
		CViewControlData* pControlData = mOSG->FindControlData();
		if(pControlData)
		{		
			MOUSEMODE defaultMouseMode = MM_SHOW_MENU;
			MOUSEMODE editMouseMode = MM_SHOW_MENU;

			if (pControlData->m_pSelector->GetSelectNodeCount() == 0)//////defaultMode
			{
				if (defaultMouseMode == MM_REPEATLASTCOMMAND)
				{
					if (pControlData->RepeatLastCommand())
					{
						return TRUE;
					}
				}
				else
				{
					auto cmdLine = GetDb3DCommandLine();
					CString strInput;
					if (cmdLine)
					{
						strInput = cmdLine->GetCurText();
					}
			
					if (!strInput.IsEmpty())
					{
						if (cmdLine)
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
	else if(pMsg->message == WM_CHAR)
	{
		SendMessage(pMsg->message , pMsg->wParam , pMsg->lParam);
		return TRUE;
	}
	else if(pMsg->message == WM_KEYDOWN)
	{	
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE)
		{
			GetDb3DCommandLine()->OnText(pMsg->wParam, 1, 1);
			return TRUE;
		 
		}	
	}
	else if(pMsg->message == WM_MBUTTONDBLCLK)
	{
		POINT point;
		::GetCursorPos(&point);

		osgViewer::View* pCurView =  mOSG->GetCurosrView(point);
		SetActiveView(pCurView);
 
		mOSG->FullSceen();
		return TRUE;	
	}	
	else if(pMsg->message == WM_LBUTTONDOWN)
	{
		POINT point;
		::GetCursorPos(&point);
		if (mOSG)
		{
			osgViewer::View* pCurView = mOSG->GetCurosrView(point);
			SetActiveView(pCurView);
		}
	}	
	else if(pMsg->message==WM_ERASEBKGND )
	{
		if(mOSG)
		{
			SendMessage(WM_ERASEBKGND,pMsg->wParam, pMsg->lParam);	 
			return TRUE;
		}
	}

	return __super::PreTranslateMessage(pMsg);
}


void CDbCompositeView::OnInitialUpdate()
{
	if(!m_bInit)
	{
		CView::OnInitialUpdate();
	}
	
}
  
void CDbCompositeView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

//
void CDbCompositeView::PauseThread()
{
	//if (m_pThread && m_nThreadStatus == 0)
	//{
	//	m_pThread->SuspendThread();
	//	m_nThreadStatus = 1;
	//}
}
void CDbCompositeView::ResumeThread()
{
	//if (m_pThread && m_nThreadStatus == 1)
	//{
	//	m_pThread->ResumeThread();
	//	m_nThreadStatus = 0;
	//}
}
