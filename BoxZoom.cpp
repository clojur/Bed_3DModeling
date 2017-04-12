#include "stdafx.h"

#include "osgwraper.h"
#include "osgtools.h"
#include "osg/linewidth"



 
#include "BoxZoom.h"
#include "IDb3DCommandLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbBoxZoomHandler::finish()
{
	status =0;
	m_pOSG->m_command = 0;
	m_pOSG->SetCursorType(OSG_NORMAL_CUSSOR);

	if (pNode)
	{
		m_pOSG->clearOverLayoutObject(pNode);
	}
}

void CDbBoxZoomHandler::begin()
{
	m_pOSG->SetCursorType(OSG_SELECT_CUSSOR);
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->AddPrompt(_T("输入第一个角点："));
	}
	 
	status++;
}

void CDbBoxZoomHandler::boxZoom()
{
	return m_pOSG->BoxZoom(p1,p2);
}
bool CDbBoxZoomHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	if(IsEscaped(ea))
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		

		finish();
		return false;
	}
	 if (IsLButtonDown(ea))
	{
		if(status == 1)
		{	
			m_pOSG->SetCursorType(OSG_NoCursor);

			_mx = GetX(ea,viewer);
			_my = GetY(ea,viewer);
			{ 			
				CPoint pt = m_pOSG->GetCursorPos();
				p11.set(pt.x , pt.y);
				p1.set(ea.getXnormalized(), ea.getYnormalized());
				p2 = p1;

				p22 = p11;
				{
					pNode =createHudSelectBOX(m_pOSG, p11,p22,pGeometry);
					m_pOSG->addOverLayoutObject( pNode);
				}						  
				status=2;
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("选择第二个角点："));
				}
			}		
		}
		else if(status == 2)
		{			
 			CPoint pt = m_pOSG->GetCursorPos();
			p22.set(pt.x , pt.y);

			m_pOSG->addRubbbox(pGeometry , p11,p22);
			status=3;
		}
		return false;
	}
	else if( ea.getEventType() ==osgGA::GUIEventAdapter::MOVE )
	{	
		if(status ==2)
		{
			p22.set(GetX(ea,viewer), GetY(ea,viewer) );
			p2.set(ea.getXnormalized(), ea.getYnormalized());

			CPoint pt = m_pOSG->GetCursorPos();
			p22.set(pt.x , pt.y);

			m_pOSG->addRubbbox(pGeometry , p11,p22);

		}
		return false;
	}
	else if( ea.getEventType() ==osgGA::GUIEventAdapter::RELEASE )
	{	
		if(status ==3)
		{
			boxZoom();

			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}

			finish();
		}
		return false;
	}
	return false;
}

