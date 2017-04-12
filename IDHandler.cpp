#include "stdafx.h"
 
#include "osgwraper.h"
#include "osgtools.h"

 #include "IDHandler.h"
 #include "IDb3DCommandLine.h"
#include "string_tools.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbIDHandler::finish()
{
	CDbHandlerInterface::finish();
 }

void CDbIDHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;

	m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->AddPrompt(_T("指定点："));
	}
		

	status++;

}

bool CDbIDHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	double _mx = GetX(ea,viewer);
	double _my = GetY(ea,viewer);
	if( IsEscaped(ea) ) //escape 被按下
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		
		finish();
		return false;
	}	 
	if( ea.getEventType() ==osgGA::GUIEventAdapter::RELEASE )
	{
		p1 = m_pOSG->getWorldPos(_mx, _my, TRUE);

		CString strFirstPTInfo;
		strFirstPTInfo.Format("X=%s    Y=%s    Z=%s\n",
			cdb_string_tools::num_to_string(p1.x()),
			cdb_string_tools::num_to_string(p1.y()),
			cdb_string_tools::num_to_string(p1.z()));
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(strFirstPTInfo);
			cmdLine->AddPrompt(_T(""));
		}
	
		finish();
		return false;
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{	 
		p1 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE); 
		 
		return false;
	}
	return false;
}