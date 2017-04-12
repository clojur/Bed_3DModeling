#include "stdafx.h"

#include "resource.h"
 
#include <osgDB/ReadFile>


#include "Pickhandler.h"
#include "BoxSelector.h"
#include "osgTools.h"
#include "OSGBaseView.h"
 
#include "2dCameramanipulator.h"
#include "dbManipulator.h"
#include "SetupScene.h"

#include "osgInterface.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CDbOSGManagerInterface::FullSceen()
{ 
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->DoFullSceen();
	} 
 }


void CDbOSGManagerInterface::DoMultiViewMax()
{

}
void CDbOSGManagerInterface::DelView(CViewControlData* pControlData)
{

}

void CDbOSGManagerInterface::AddView(CViewControlData*)
{

}

CDbOSGManagerInterface::CDbOSGManagerInterface( CView* pView ) 
{
	m_pParentView = pView;
	m_hWnd = pView->m_hWnd;

	done                   = true;
	busedMulsample = false;

	userCursorType = OSG_NORMAL_CUSSOR;

	CString strAppPath = OSGGetApplicationSelfPath();

	CString strCursor = strAppPath + "\\res\\cur_hand.cur";
	panCursor	=LoadCursorFromFile(strCursor);

	strCursor = strAppPath + "\\res\\rotate_red.cur";
	rotateCursor=LoadCursorFromFile(strCursor ) ;	
}


CDbOSGManagerInterface::~CDbOSGManagerInterface()
{

}

HCURSOR CDbOSGManagerInterface::GetCursor(int type,std::map<int,HCURSOR>& mouseCursorMap)
{
 	std::map<int,HCURSOR>::iterator it = mouseCursorMap.find( type);
	if(it == mouseCursorMap.end() )
	{
		osgViewer::GraphicsWindow::MouseCursor cursor = (osgViewer::GraphicsWindow::MouseCursor)type;
		bool bFind = true;
		switch (cursor )
		{
		case osgViewer::GraphicsWindow::MouseCursor::RightArrowCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_ARROW);
			break;
		case osgViewer::GraphicsWindow::MouseCursor::LeftArrowCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_ARROW);
			break;
		case osgViewer::GraphicsWindow::MouseCursor::InfoCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZEALL);
			break;
		case osgViewer::GraphicsWindow::MouseCursor::DestroyCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_NO );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::HelpCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_HELP );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::CycleCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_NO );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::SprayCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZEALL );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::WaitCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_WAIT);
			break;
		case osgViewer::GraphicsWindow::MouseCursor::TextCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_IBEAM );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::CrosshairCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_CROSS );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::UpDownCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZENS );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::LeftRightCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZEWE );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::TopSideCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_UPARROW );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::BottomSideCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_UPARROW );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::LeftSideCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZEWE);
			break;
		case osgViewer::GraphicsWindow::MouseCursor::RightSideCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZEWE );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::TopLeftCorner:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZENWSE );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::TopRightCorner:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZENESW );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::BottomRightCorner:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZENWSE );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::BottomLeftCorner:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_SIZENESW );
			break;
		case osgViewer::GraphicsWindow::MouseCursor::HandCursor:
			mouseCursorMap[type] = LoadCursor( NULL, IDC_HAND );
		default:
			{
				bFind = false;
				break;
			}
		}

		if(!bFind)
		{	 
			if(type == OSG_NoCursor)
				mouseCursorMap[type] =NULL;

			else if(type == OSG_NORMAL_CUSSOR)
				mouseCursorMap[type] =(AfxGetApp()->LoadCursor(IDC_CURSOR_NORMAL));
			else if (type == OSG_NORMAL_CUSSOR+1000)
				mouseCursorMap[type] = (AfxGetApp()->LoadCursor(IDC_CURSOR_NORMAL1));

			else if(type == OSG_INPUT_CUSSOR)
				mouseCursorMap[type] =(AfxGetApp()->LoadCursor(IDC_CURSOR_INPUT));
			else if (type == OSG_INPUT_CUSSOR+1000)
				mouseCursorMap[type] = (AfxGetApp()->LoadCursor(IDC_CURSOR_INPUT1));


			else if (type == OSG_SELECT_CUSSOR)
				mouseCursorMap[type] =(AfxGetApp()->LoadCursor(IDC_CURSOR_SEL));
			else if (type == OSG_SELECT_CUSSOR+1000)
				mouseCursorMap[type] = (AfxGetApp()->LoadCursor(IDC_CURSOR_SEL1));


			else if (type == OSG_SELECT_PAN)
				mouseCursorMap[type] =( panCursor ) ;	
			else if (type == OSG_SELECT_PAN+1000)
				mouseCursorMap[type] = (panCursor);


			else if(type == OSG_SELECT_ZOOM)
				mouseCursorMap[type] =(AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM));
			else if (type == OSG_SELECT_ZOOM+1000)
				mouseCursorMap[type] = (AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM1));

			else if(type == OSG_SELECT_ROTATE)
				mouseCursorMap[type] =(rotateCursor);
			else if (type == OSG_SELECT_ROTATE+1000)
				mouseCursorMap[type] = (rotateCursor);

		}
		return mouseCursorMap[type] ;
	}
	else
	{
		return it->second;
	}
}

void  CDbOSGManagerInterface::SetBehavorType(int type)
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		pControlData->SetBehavorType(type );
	} 
}


int CDbOSGManagerInterface::GetBehavorType()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->DoGetBehavorType( );
	} 
	return 0;
} 
 
 
void CUpdateCallback::UpdateBase()
{	
	if(!m_pOSG)
		return;
	m_pOSG->DoUpdateCallBack();
}

//¿ì½Ý¼ü
bool CDbOSGManagerInterface::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->OnKeyDown(nChar , nRepCnt ,nFlags );
	} 
	return false;
}

bool CDbOSGManagerInterface::OnCommand(const CString& strCommand)
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->OnCommand(strCommand);
	}  
	return false;
}


bool CDbOSGManagerInterface::pre_do_new_command(const CString& domain, const CString& command, const CString& param)
{
	CViewControlData* pControlData = FindControlData();
	if (pControlData)
	{
		return pControlData->pre_do_new_command(domain, command, param);
	}
	return false;
}

void CDbOSGManagerInterface::OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked)
{
	CViewControlData* pControlData = FindControlData();
	if (pControlData)
	{
		pControlData->OnCommandUpdate(strCommand,enable,checked);
	}
}
int CDbOSGManagerInterface::GetCommandStatus()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->m_command; 
	} 
	return 0;
}

void CDbOSGManagerInterface::SetReDraw(bool bReDraw)
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->SetReDraw(bReDraw);
	} 
}


void CDbOSGManagerInterface::SetCursorType(int type)
{
 	userCursorType = type;	
}

CView* CDbOSGManagerInterface::GetCurrentCWnd()
{
	return m_pParentView;
}
 

void CDbOSGManagerInterface::TopView()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->TopView();
	} 
}


void CDbOSGManagerInterface::FrontView()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->FrontView();
	} 

}
void CDbOSGManagerInterface::LeftView()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->LeftView();
	} 

}

void CDbOSGManagerInterface::PerspectiveView()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->PerspectiveView();
	} 
}

void CDbOSGManagerInterface::TopView2()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->TopView2();
	} 

}


void CDbOSGManagerInterface::LeftView2()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->LeftView2();
	} 
}


void CDbOSGManagerInterface::FrontView2()
{
	CViewControlData* pControlData = FindControlData();
	if(pControlData)
	{
		return pControlData->FrontView2();
	} 
}
 
 


void FindHeightLightNodeVisitor::apply( osg::Node& node )
{
	bool flag = false;
	node.getUserValue("heightLieht" ,flag);
	if(flag)
	{
		pNode = &node;

		return;
	}	
	traverse( node );
}

CRenderingThread::CRenderingThread( CDbOSGManagerInterface* ptr  )
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
	finish = false;
}


CRenderingThread::~CRenderingThread()
{
	_done = true;
	if (isRunning())
	{
		cancel();
		join();
	}
}

void CRenderingThread::Do(osgViewer::ViewerBase* viewer)
{
	CDbOSGBaseView* pBaseView = (CDbOSGBaseView*)_ptr->m_pParentView;
	if (pBaseView && pBaseView->GetSafeHwnd() && pBaseView->IsWindowVisible()  )//Tab
	{	
		_ptr->PreFrameUpdate();
		viewer->frame();
		_ptr->PostFrameUpdate();
	}
	else
	{
		//Sleep(10); zmz
	}
}
void CRenderingThread::run()
{
	if ( !_ptr )
	{
		_done = true;
		return;
	}
	osgViewer::ViewerBase* viewer = _ptr->GetCurrentViewer();
	if (!viewer)
		return;

	do
	{
		if (!_ptr->done)
		{
			//Sleep(10);zmz
			finish = true;
		}
		else
		{
			Do(viewer);
		}
	} while (!testCancel() && !viewer->done() && !_done  );

	int kk = 0;
}



UINT Render(LPVOID* ptr)
{
	CDbOSGManagerInterface* osg = (CDbOSGManagerInterface*)ptr;

	osgViewer::ViewerBase*  pBaseView = osg->GetCurrentViewer();
	CView* pOSGView = osg->GetCurrentCWnd();

	while (!pBaseView->done() && osg->done)
	{
		if (pOSGView && pOSGView->GetSafeHwnd()/*pOSGView->IsWindowVisible()*/)//zmz
		{
			DWORD t1 = GetTickCount();

			osg->PreFrameUpdate();
			pBaseView->frame();
			osg->PostFrameUpdate();
			DWORD t2 = GetTickCount();
			//TRACE("viewer->frame() erase time :%d\n", t2 - t1);//zmz
		}
		else
		{
			//Sleep(10);zmz
		}
	}
	delete osg;
	///////////
	return 1;
}