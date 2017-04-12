#include "stdafx.h"
 
#include "osgwraper.h"
#include "osgtools.h"

 #include "DistHandler.h"
 #include "IDb3DCommandLine.h"
#include "string_tools.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbDistHandler::finish()
{
	CDbHandlerInterface::finish();
	m_pOSG->clearOverLayoutObject(pCamera);	 
}

void CDbDistHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;

	m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->AddPrompt(_T("输入第一点："));
	}
	

	status++;

}

bool CDbDistHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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
	 if (IsLButtonDown(ea))
	{
		if(status == 1)
		{	 
			_mx = GetX(ea,viewer);
			_my = GetY(ea,viewer);
			{
				p1  = m_pOSG->getWorldPos(  _mx,_my,TRUE  );
				p11= m_pOSG->getWindosPos(   p1 );

				//橡皮筋
				pCamera =createHudLine(m_pOSG, p11,p11,pGeometry);
				m_pOSG->addOverLayoutObject( pCamera);	

				p2 = p1;
				p22 = p11;
			 					  
				status=2;
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("指定第二个点："));
				}
			
			}		
		}
		else if(status == 2)
		{			
			p2    = m_pOSG->getWorldPos(GetX(ea,viewer), GetY(ea,viewer) , TRUE  );
			p22  = m_pOSG->getWindosPos(  p2 );
			p11  = m_pOSG->getWindosPos( p1 );
	 
			m_pOSG->addRubbline(pGeometry,p11,p22);	
			status=3;
		}
		return false;
	}
	else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
	{	
		if (status == 1)
		{
			p1 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);
		}
		if(status ==2)
		{
			p2    = m_pOSG->getWorldPos(GetX(ea,viewer), GetY(ea,viewer) , TRUE  );

			p22  = m_pOSG->getWindosPos(  p2);
			p11  = m_pOSG->getWindosPos( p1 );
	

			m_pOSG->addRubbline(pGeometry,p11,p22);	
		}
		return false;
	}
	else if( ea.getEventType() ==osgGA::GUIEventAdapter::RELEASE )
	{	
		if(status ==3)
		{
 			CString strFirstPTInfo;
			strFirstPTInfo.Format("第一点X坐标:%s, Y坐标%s, Z坐标%s\n",
				cdb_string_tools::num_to_string(p1.x() ),
				cdb_string_tools::num_to_string(p1.y()),
				cdb_string_tools::num_to_string(p1.z()));

			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(strFirstPTInfo);
			}
			 

			CString strSecondPTInfo;
			strSecondPTInfo.Format("第二点X坐标:%s, Y坐标%s, Z坐标%s\n", 
				cdb_string_tools::num_to_string(p2.x()),
				cdb_string_tools::num_to_string(p2.y()),
				cdb_string_tools::num_to_string(p2.z()));
			//auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(strSecondPTInfo);
			}
			
		
			CString strInfo;
			double dx = abs(p1.x() - p2.x());
			double dy = abs(p1.y() - p2.y());
			double dz = abs(p1.z() - p2.z());
			double dl = osg::Vec3(p1 - p2).length();

			strInfo.Format("DX:%s, DY:%s, DZ:%s, DL:%s\n",
				cdb_string_tools::num_to_string(dx), cdb_string_tools::num_to_string(dy), cdb_string_tools::num_to_string(dz), cdb_string_tools::num_to_string(dl));
			if (cmdLine)
			{
				cmdLine->AddPrompt(strInfo);
				cmdLine->AddPrompt(_T(""));
			}
	
			finish();
		}
		return false;
	}
	return false;
}