#include "stdafx.h"

 
#include "osgwraper.h"
#include "osgtools.h"
#include "BoxSelector.h"
#include "OffsetHandler.h"
#include "IDb3DCommandLine.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double CDbOffsethandler::dOffset = 0;


CDbOffsethandler::CDbOffsethandler(CViewControlData* pOSG) :CDbHandlerInterface(pOSG)
{
	status=0; 
 }

void CDbOffsethandler::finish()
{
	CDbHandlerInterface::finish();  
}
void CDbOffsethandler::begin()
{
	m_pOSG->bEnableSnap = TRUE; 
	{
		CString strInfo;
 		if (abs(dOffset) > 1.0e-3)
		{
			strInfo.Format("指定偏移距离<%g>：",dOffset );
 		}
		else
		{
			strInfo.Format("指定偏移距离：");
		}
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(strInfo);
		}
	

		m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
		SetSendPointFlag(OSG_CMDLINE);
		status = 1;
	}
 	
}
void CDbOffsethandler::action(bool bCancle)
{ 
	m_pOSG->beginOperation("偏移对象");


	std::vector<CDbObjInterface*> realCopyObjs = m_pOSG->OnPreCopy(selectetSetMap);
	for (auto& iter : realCopyObjs)
	{
		CDbObjInterface* pData = iter;
		if (!bCancle)
		{
			bUp = pData->Orietation(pickPoint);

			bool flag = pData->offset(dOffset*(double)bUp);
			if (flag)
			{
				CopyObjectAndAdd(m_pOSG, pData);
			}
		}
	}
	std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for (; it != selectetSetMap.end(); it++)
	{
		m_pOSG->clearTempObject(it->first);
	}
	selectetSetMap.clear();


	m_pOSG->endOperation();
}
 
bool CDbOffsethandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
	if (!viewer) return false;

	double _mx = GetX(ea,viewer);
	double _my = GetY(ea,viewer);

	if( IsEscaped(ea) ) //escape 被按下
	{ 
		if (status > 1)
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
	else if (status == 1)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strPos = cmdLine->GetCurText();
				if (!strPos.IsEmpty())
					dOffset = atof(strPos);

				cmdLine->AddPrompt(_T("选择对象："));
			}
		
			m_pOSG->m_pSelector->clearSelectSet();
			m_pOSG->m_pSelector->begin();
			m_pOSG->m_pSelector->selectType = 8;
			status = 2;
			return false;
		}

		return false;
	}

	if(status ==2)
	{
		if(m_pOSG->m_pSelector->IsFinish() )
		{
			if(m_pOSG->GetSelectObjCount() == 1 )
			{
				selectetSetMap.clear();
				std::vector< osg::Node* > tempObj;
				m_pOSG->m_pSelector->GetCloneSelectObj(tempObj, 1);
				std::vector< osg::Node* >::iterator it = tempObj.begin();
				for (; it != tempObj.end(); it++)
				{
					osg::Node* pNode = *it;
					CDbObjInterface* pObj = m_pOSG->FindObj(pNode->getName().c_str(), GetObjType(pNode));
					if (pObj)
					{
						CDbObjInterface* pCloneObj = pObj->Clone();
						if (pCloneObj)
						{
							selectetSetMap[pNode] = pCloneObj;
							m_pOSG->addTempObject(pNode);
 						} 
						break;
					}
					else
					{
						ASSERT(FALSE);
					}
				}

				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt("指定要偏移的那一侧的点：");
				}
		
				SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE | OSG_CMDLINE);
				m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);

				status = 3;
				return false;	
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
	
	else if( status ==3)
	{		 
	 if (IsLButtonDown(ea))
		{	
			pickPoint = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);

			action(false);	
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T("选择对象："));
			}
		
			m_pOSG->m_pSelector->clearSelectSet();
			m_pOSG->m_pSelector->begin();
			m_pOSG->m_pSelector->selectType = 8;
			status = 2;
			return false;
		}
		else if( ea.getEventType() ==osgGA::GUIEventAdapter::MOVE )
		{	
			pickPoint = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);


			//临时对象
			std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
			for (; it != selectetSetMap.end(); it++)
			{
				osg::Node* pNode = it->first;
 				CDbObjInterface* pObj = it->second;
				if (pObj)
				{
					CDbObjInterface* pTemp = pObj->Clone();
					if (pTemp)
					{
						ORIETATION bNewUp = pTemp->Orietation(pickPoint);
						if (bNewUp != bUp)
						{
							bUp = bNewUp;

							if (bNewUp != ONCURVE)
							{
								if (pTemp->offset(dOffset * (double)bUp, false))
								{
									pTemp->OnUpdate(pNode);
								}
							}
						}
						delete pTemp;					
					}
				}			 
			}
			return false;
		}
		return false;
	}
	return false;
}




