#include "stdafx.h"
  
#include "osginterface.h"
#include "osgtools.h"
#include "BoxSelector.h"
#include "osgarrayHandler.h"
#include "IDb3DCommandLine.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbArrayHandler::finish()
{
	CDbHandlerInterface::finish();

	//清理
 	std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for (; it != selectetSetMap.end(); it++)
	{
		m_pOSG->clearTempObject(it->first);
	}
	clearCloneObject();
}

void CDbArrayHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;
	if(m_pOSG->GetSelectObjCount() > 0 )
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("输入行数<1>："));
		}
	
		status=2;
	}
	else
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("选择对象："));
		}
		
		m_pOSG->m_pSelector->begin();
		m_pOSG->m_pSelector->selectType = 2;
		status++;
	}
}


void CDbArrayHandler::Action()
{
	m_pOSG->beginOperation("阵列对象");

	m_pOSG->m_bInstallValid = FALSE;

	std::vector<CDbObjInterface*> realCopyObjs = m_pOSG->OnPreCopy(selectetSetMap);
	for (auto& iter : realCopyObjs)
	{
		CDbObjInterface* pData = iter;
		int index = 0;

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				for (int k = 0; k < layers; k++)
				{
					index++;
					osg::Vec3d offset = osg::Vec3d((i + 1)*rowValue, (j + 1)*colValue, (k + 1)*layersValue);

					CDbObjInterface* pCreateData = pData->Clone();
					if (pCreateData)
					{
						pCreateData->Copy(offset);

						CopyObjectAndAdd(m_pOSG, pCreateData);

					}
				}
			}
		}
	}


	m_pOSG->endOperation();

	m_pOSG->m_bInstallValid = TRUE;
}

bool CDbArrayHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;

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

	if(status ==1)
	{
		if(m_pOSG->m_pSelector->IsFinish() )
		{
			if(m_pOSG->GetSelectObjCount() > 0 )
			{
				std::vector<osg::Node*> tempObj;
				m_pOSG->m_pSelector->GetCloneSelectObj(tempObj, 2);
				for (int i = 0; i < tempObj.size(); i++)
				{
					osg::Node* pNode = tempObj[i];
					CDbObjInterface* pObj = m_pOSG->FindObj( pNode->getName().c_str(), GetObjType(pNode));
					if (pObj)
					{
 
						CDbObjInterface* pCloneObj = pObj->Clone();
						if (pCloneObj)
						{
							selectetSetMap[pNode] = pCloneObj;
							m_pOSG->addTempObject(pNode);
						}
					}
				}
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入行数<1>："));
				}
				
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
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strRow = cmdLine->GetCurText();
				if (strRow.IsEmpty())
					row = 1;
				else
					row = atoi(strRow);

				cmdLine->AddPrompt(_T("输入列数<1>："));
			}
		
			status=3;
		}	
	
	}
	else if(status ==3)
	{                  
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				CString strCol = cmdLine->GetCurText();
				if (strCol.IsEmpty())
					col = 1;
				else
					col = atoi(strCol);
				cmdLine->AddPrompt(_T("输入层数<1>："));
			}
		

 				
			status=4;
		}
	
	}
	else if(status ==4)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);

			auto cmdLine = GetDb3DCommandLine();
			CString strLayer;
			if (cmdLine)
			{
				strLayer = cmdLine->GetCurText();
			}
		

			if (strLayer.IsEmpty())
				layers = 1;
			else
				layers = atoi(strLayer);

			if(row >1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入行间距："));
				}
				
				status=5;
			}
			else if( col>1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入列间距："));
				}
				
				status=6;
			}
			else if(layers >1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入层间距："));
				}
				
				status=7;
			}
			else
			{
				finish();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
				 
				return false;
			}
		}
	
	}

	else if(status ==5)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				rowValue = atof(cmdLine->GetCurText());
			}
		
			if( col>1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入列间距："));
				}
				 
				status=6;
			}
			else if(layers >1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入层间距："));
				}
					
				status=7;
			}
			else
			{
				Action();
				finish();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
			
				return false;
			}
		}	
	}
	else if(status ==6)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			auto cmdLine = GetDb3DCommandLine();
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			if (cmdLine)
			{
				colValue = atof(cmdLine->GetCurText());
			}
		
			if(layers >1)
			{
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入层间距："));
				}
				
				status=7;
			}
			else
			{
				Action();

				finish();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
				
				return false;
			}
		}	
	}
	else if(status ==7)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))
		{
			auto cmdLine = GetDb3DCommandLine();
			
			m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
			if (cmdLine)
			{
				layersValue = atof(cmdLine->GetCurText());
			}
		
			Action();	
			{
				finish();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
				
				return false;
			}
		}		
	}
	return false;
}
