#include "stdafx.h"

 
#include "osgwraper.h"
#include "osgtools.h"
#include "StretchHandler.h"
#include "BoxSelector.h"
#include "IDb3DCommandLine.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbQuickStretchHandler::finish()
{
	CDbHandlerInterface::finish();

	//清理
	m_pOSG->clearOverLayoutObject(pCamera);	

	for(int i=0; i<hotHhandlers.size(); i++)
	{
		m_pOSG->clearOverLayoutObject(hotHhandlers[i]);
	}

	std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for(; it != selectetSetMap.end(); it++)
	{	
		m_pOSG->clearTempObject( it->first);	
	}
	clearCloneObject();

}

void CDbQuickStretchHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;
	if( m_pOSG->GetSelectObjCount() == 0 )
	{
		return;
	}
	else
	{	
		status=1;
	}
}

std::vector<osg::Vec3d> CDbQuickStretchHandler::FindOldPosition(const CString& strObjName)
{
	std::map< CString, std::vector<osg::Vec3d> >::iterator it = m_oldPosition.find(strObjName);
	if (it != m_oldPosition.end())
	{
		return it->second;
	}
	std::vector<osg::Vec3d> emptyVec;
	return emptyVec;
}

void CDbQuickStretchHandler::action( bool bCancle)
{
	m_pOSG->m_bInstallValid = FALSE;
	 
	m_pOSG->beginOperation("拉升对象");

	std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for(; it != selectetSetMap.end(); it++)
	{
		CDbObjInterface* pData = m_pOSG->FindObj(it->second->GetName(), GetObjType(it->first));
		if (!pData)
			continue;
		if(!bCancle)
		{
			osg::Vec3d offset = p22-p11;
			bool flag = false;
			if (bStretch)
			{
				std::vector<int> stretchIndex = m_pOSG->GetStretchIndex(pData->GetName());
				std::vector<osg::Vec3d> oldPosition = FindOldPosition(pData->GetName());
				flag = pData->Stretch(stretchIndex, oldPosition,offset);
			}
			else
			{
				flag = pData->Move(offset);
			}
			if (flag)
				m_pOSG->updateData( pData);		
		}
	}

	m_pOSG->endOperation();

	m_pOSG->m_bInstallValid = TRUE;
}

bool CDbQuickStretchHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	double _mx = GetX(ea,viewer);
	double _my = GetY(ea,viewer);

	if (!viewer) return false;

	if(IsEscaped(ea))
	{	
		if(status > 1)
		{
			action(true);
		}
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		
		finish();
		return false;
	}

	if(status==1)
	{	
		std::vector< osg::Node* > tempObj;
		m_pOSG->m_pSelector->GetCloneStretchObj(tempObj, 0);
		//临时对象

		m_pOSG->snapExincludes.clear();
		std::vector< osg::Node* >::iterator it = tempObj.begin();
		for( ; it != tempObj.end() ; it++)
		{
			osg::Node* pNode =*it;
			CDbObjInterface* pObj =  m_pOSG->FindObj(  pNode->getName().c_str()  ,GetObjType(pNode )  );
			if(pObj)
			{
				CDbObjInterface* pCloneObj = pObj->Clone();
				if (pCloneObj)
				{
					selectetSetMap[pNode] = pCloneObj;
					m_pOSG->addTempObject(pNode);
					m_oldPosition[pObj->GetName()] = pObj->GetStretchPoint();
				}
 			}	
			else
			{
				ASSERT(FALSE);
			}
		}
		if(m_pOSG->GetSelectHotPointCount() == 0 )
		{
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}
			
			finish();
			return false;	
		}
		else
		{		

			p11=m_pOSG->getWorldPos(  _mx,_my,TRUE  );
			startPT = m_pOSG->getWindosPos(p11);
			SetConformPoint(p11);

			tempPt = p11;//记住上次的位置

			//橡皮筋
			pCamera = createHudLine(m_pOSG, startPT, startPT, pGeometry);
			m_pOSG->addOverLayoutObject(pCamera);
			status = 2;
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T("指定目标点或者位移："));
			}
		
			SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE | OSG_CMDLINE);
		}
		return false;
	}
	else if(status ==2) //指定第二个点
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{	 
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strPos = cmdLine->GetCurText();
				GetCurrentPos(p11, p22, strPos, FALSE);
				action(false);
				cmdLine->AddPrompt(_T(""));
			}
		
			finish();		
		}
		 if (IsLButtonDown(ea))
		{	
			p22= m_pOSG->getWorldPos(GetX(ea,viewer), GetY(ea,viewer) , TRUE  );
			action( false);	
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}
			finish();		
		}
		else if( ea.getEventType() ==osgGA::GUIEventAdapter::MOVE )
		{	
			p22    = m_pOSG->getWorldPos(  GetX(ea,viewer),   GetY(ea,viewer) , TRUE  );

			endPT= m_pOSG->getWindosPos(  p22 );
			startPT = m_pOSG->getWindosPos(p11);
			m_pOSG->addRubbline(pGeometry, startPT , endPT );

			//临时对象
			std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
			for( ; it != selectetSetMap.end() ; it++)
			{
				osg::Node* pNode   = it->first;
				CDbObjInterface* pObj = it->second;
				osg::Vec3d offset = p22 - p11;	
				std::vector<osg::Vec3d> oldPosition = FindOldPosition(pObj->GetName());
				if (pObj->Stretch(m_pOSG->GetStretchIndex(pObj->GetName()), oldPosition, offset))
				{
					m_pOSG->addUpdateObject(pObj,  pNode);
				}
 			}	
 			return false;
		}
		return false;
	}
	return false;
}

