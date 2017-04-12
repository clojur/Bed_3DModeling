#include "stdafx.h"

 
#include "osgwraper.h"
#include "osgtools.h"
#include "BoxSelector.h"
#include "moveobjHandler.h"
#include "IDb3DCommandLine.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDbMoveObjhandler::CDbMoveObjhandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
{
	bQuickModel=false;
	status=0;
	pCamera=NULL;
	pGeometry=NULL;
	strName ="move";
	pParentNode = NULL;
 }

void CDbMoveObjhandler::finish()
{
	CDbHandlerInterface::finish();

	//清理
	m_pOSG->clearOverLayoutObject(pCamera);	
	std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for(; it != selectetSetMap.end(); it++)
	{	
		m_pOSG->clearTempObject( it->first);
	}
	clearCloneObject();

	pParentNode.release();

 }
void CDbMoveObjhandler::begin()
{
 
	m_pOSG->bEnableSnap =TRUE;

	if(m_pOSG->GetSelectObjCount() ==0   )
	{
		m_pOSG->m_pSelector->selectType = 2;
 		m_pOSG->m_pSelector->begin(_T("选择对象："));
		
		status=1;
	}
	else
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt("指定基点：");
		}
		
		m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
		SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE );

		status=2;
	}
}
void CDbMoveObjhandler::action( bool bCancle)
{
	std::vector<CDbObjInterface*> allObjects;

	std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for (; it != selectetSetMap.end(); it++)
	{
		if (it->second)
		{
			allObjects.push_back(it->second);
		}

		osg::Node* pNode = it->first;
		int nCount = pNode->getNumParents();
		if (nCount > 0)
		{
			osg::Group* pParent = pNode->getParent(0);
			if (pParent && pParent == pParentNode)
			{
				osg::Group* pRoot = pParent->getParent(0);

				pRoot->addChild(pNode);
				pRoot->removeChild(pParent);
			}
		}
	}

	m_pOSG->beginOperation("移动对象");

	osg::Vec3d offset = p22 - p11;
	m_pOSG->OnPreMove(allObjects, offset);

	m_pOSG->m_bInstallValid = FALSE;

	for (int i = 0; i < allObjects.size(); i++)
	{
		if (CDbObjInterface* pDataCopy = allObjects[i])
		{
			CDbObjInterface* pData = m_pOSG->FindObj(pDataCopy->GetName(), pDataCopy->GetRTType());

			if (pData == NULL)
			{
				continue;
			}
			if (!bCancle)
			{
				if (offset.length() > 0)
				{
					if (pData->Move(offset))
					{
						m_pOSG->updateData(pData);
					}
				}
			}
		}
	}

	m_pOSG->endOperation();

	m_pOSG->m_bInstallValid = TRUE;
}

void CDbMoveObjhandler::AppalyBasePT(osg::Vec3 inputPt)
{

 	startPT=m_pOSG->getWindosPos(   p11 );

	SetConformPoint(p11);

	//橡皮筋
	pCamera =createHudLine(m_pOSG, startPT,startPT,pGeometry);
	m_pOSG->addOverLayoutObject( pCamera);	

	//临时对象
	std::vector< osg::Node* > tempObj;
	m_pOSG->m_pSelector->GetCloneSelectObj(tempObj,1);
	std::vector< osg::Node* >::iterator it = tempObj.begin();
	for( ; it != tempObj.end() ; it++)
	{
		osg::Node* pNode =*it;
		CDbObjInterface* pObj =  m_pOSG->FindObj(  pNode->getName().c_str()  ,GetObjType(pNode)  );
		if(pObj)
		{
			m_pOSG->addTempObject(pNode);
			selectetSetMap[pNode] = pObj->Clone();
		}
		else
		{
			return;
		}
	}
	status=3;
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->AddPrompt(_T("指定第二个点或<使用第一点作为位移>:"));
	}

	SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE | OSG_CMDLINE);
}


bool CDbMoveObjhandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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

				status=2;
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
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strPos = cmdLine->GetCurText();
				p11 = GetPosition(strPos);
				AppalyBasePT(p11);
				m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			}
		
		}
		else if (IsLButtonDown(ea) || bQuickModel)
		{	
			p11 = m_pOSG->getWorldPos(_mx, _my, TRUE);
			AppalyBasePT(p11);
		}
		else if( ea.getEventType() ==osgGA::GUIEventAdapter::MOVE )
		{	
			p11    = m_pOSG->getWorldPos(  _mx,_my,TRUE  );	
			return false;
		}
		return false;
	}
	else if(status ==3)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))   //delta
		{
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strPos = cmdLine->GetCurText();

				GetCurrentPos(p11, p22, strPos, TRUE);
				action(false);
				cmdLine->AddPrompt(_T(""));
			}
		
			finish();		
		}
		else if ( IsLButtonDown(ea) || (osgGA::GUIEventAdapter::RELEASE == ea.getEventType() && bQuickModel))
		{
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);
			action(false);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}
			
			finish();
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
		{
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);

			endPT   = m_pOSG->getWindosPos(p22);
			startPT = m_pOSG->getWindosPos(p11);
			m_pOSG->addRubbline(pGeometry, startPT, endPT);

			//临时对象
			std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
			for (; it != selectetSetMap.end(); it++)
			{
				osg::Node* pNode = it->first;
				osg::Vec3 offset = p22 - p11;

				osg::Matrix m;
				m.makeTranslate(offset);

				if (pParentNode == NULL)
					pParentNode = new osg::MatrixTransform;

				pParentNode->setMatrix(m);
				
				int nCount = pNode->getNumParents();
				if (nCount > 0)
				{
					osg::Group* pRoot = pNode->getParent(0);
					if (pRoot && pRoot != pParentNode)
					{
						pParentNode->addChild(pNode);
						pRoot->addChild(pParentNode);
						pRoot->removeChild(pNode);
					}
				}
			}
			return false;
		}
 
		return false;
	}
 
	return false; 
}




