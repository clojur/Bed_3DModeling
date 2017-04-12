#include "stdafx.h"

 
#include "osgwraper.h"
#include "osgtools.h"

#include "BoxSelector.h"
#include "osgCopyHandler.h"
#include "IDb3DCommandLine.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDbCopyHandler::CDbCopyHandler(CViewControlData* pOSG) :CDbHandlerInterface(pOSG)
{
	status = 0;
	pCamera = NULL;
	pGeometry = NULL;
	strName = "copy";
	pParentNode = NULL;
}

 
void CDbCopyHandler::finish()
{
	CDbHandlerInterface::finish();
	m_pOSG->m_bInstallValid = TRUE;
	m_pOSG->endOperation();


}

void CDbCopyHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;
	if(m_pOSG->GetSelectObjCount() > 0 )
	{
		m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
		SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE);
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("指定基点：");
		}
		
		status=2;
	}
	else
	{
		
		 auto cmdLine = GetDb3DCommandLine();
		 if (cmdLine)
		 {
			 cmdLine->AddPrompt("指定基点：");
		 }
		m_pOSG->m_pSelector->begin();
		m_pOSG->m_pSelector->selectType = 2;
		status=1;
	}
	m_pOSG->m_bInstallValid = FALSE;

	m_pOSG->beginOperation("拷贝对象");


}

void CDbCopyHandler::action( bool bCancle)
{

	std::vector<CDbObjInterface*> realCopyObjs = m_pOSG->OnPreCopy(selectetSetMap);
	for (int i = 0; i < realCopyObjs.size(); i++)
	{
		CDbObjInterface* pData = realCopyObjs[i];
		if (!bCancle)
		{
			osg::Vec3d offset = p22 - p11;
			bool flag = pData->Copy(offset);
			if (flag)
			{
				CopyObjectAndAdd(m_pOSG,  pData);		 
			}
		}
	}
	


	selectetSetMap.clear();

	m_pOSG->clearTempObject(pParentNode );
	 
	pParentNode = NULL;

	m_pOSG->clearOverLayoutObject(pCamera);
	pCamera = NULL;
}
void CDbCopyHandler::mCopyImplement( )
{ 
	if (1)
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("指定第二个点或 <输入位移>:"));
		}

		SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE | OSG_CMDLINE);
		status++;
	} 
}

bool CDbCopyHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	double _mx = GetX(ea,viewer);
	double _my = GetY(ea,viewer);

	if( IsEscaped(ea) ) //escape 被按下
	{   	
		if(status >1  )
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

	if(status ==1)
	{
		if(m_pOSG->m_pSelector->IsFinish() )
		{
			if(m_pOSG->GetSelectObjCount() > 0 )
			{
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt("指定基点：");
				}
			
				 m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
				 SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE);
				 status++;
			}
			else
			{
				finish();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
	
				return false;
			}
		}
		else
		{
			return m_pOSG->m_pSelector->handle(ea, aa);
		}
	}
	else if(status ==2)
	{
		//移动时不显示夹点
		std::map<CString, osg::ref_ptr<osg::Node> > tempObj;
		m_pOSG->m_pSelector->getSelectSet(tempObj);
		std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = tempObj.begin();
		for(; it != tempObj.end() ; it++)
		{
			m_pOSG->ClearHandlers( it->second);
		}

		if (IsReturn(ea) || IsRButtonDown(ea))   //D
		{	
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);

			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strPos = cmdLine->GetCurText();

				p11 = GetPosition(strPos);
				startPT = m_pOSG->getWindosPos(p11);
			}
		

			SetConformPoint(p11);

			mCopyImplement();
			return false;
		}
		 if (IsLButtonDown(ea))
		{	
			p11=m_pOSG->getWorldPos(  _mx,_my,TRUE  );

 			startPT=m_pOSG->getWindosPos(   p11 );

			SetConformPoint(p11);

			mCopyImplement();		 
			return false;
		 }
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
		{
			p11 = m_pOSG->getWorldPos(_mx, _my, TRUE);
			return false;
		}
		return false;
	}
	else if(status >2)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))   //D
		{	
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			CString strPos;
			if (cmdLine)
			{
				strPos = cmdLine->GetCurText();
			}
		
			if (strPos.IsEmpty())
			{
				action(true);
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
				
				finish();
				return false;
			}

			GetCurrentPos(p11, p22, strPos,TRUE);
			action(false);
		
			mCopyImplement();
			return false;
		}
		 if (IsLButtonDown(ea))
		{
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);
			action(false);
			mCopyImplement();
			return false;
		 }
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE)
		{
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);
			endPT = m_pOSG->getWindosPos(p22);
			startPT = m_pOSG->getWindosPos(p11);
			if (!pParentNode)
			{
				pParentNode = new osg::MatrixTransform;
				m_pOSG->addTempObject(pParentNode);
				std::vector<osg::Node*> tempNodes;
				m_pOSG->m_pSelector->GetCloneSelectObj(tempNodes, 2);
				for (int i = 0; i < tempNodes.size(); i++)
				{
					osg::Node* pNode = tempNodes[i];
					CDbObjInterface* pObj = m_pOSG->FindObj(pNode->getName().c_str(), GetObjType(pNode));
					if (pObj)
					{
						CDbObjInterface* pCloneObj = pObj->Clone();
						if (pCloneObj)
						{
							selectetSetMap[pNode] = pCloneObj;
							pParentNode->addChild(pNode);
						}
					}

				}
			}

			if (!pCamera)
			{
				pCamera = createHudLine(m_pOSG, startPT, endPT, pGeometry);
				m_pOSG->addOverLayoutObject(pCamera);
			}
			else
			{
				m_pOSG->addRubbline(pGeometry, startPT, endPT);
			}

			osg::Vec3 offset = p22 - p11;
			osg::Matrix m;
			m.makeTranslate(offset);
			pParentNode->setMatrix(m);

			return false;
		}
		return false;
	}	 
	
	return false;
}