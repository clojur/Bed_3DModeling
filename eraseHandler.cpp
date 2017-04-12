#include "stdafx.h"
 
#include "osgwraper.h"
#include "osgtools.h"

#include "eraseHandler.h"
#include "ViewControlData.h"
#include "BoxSelector.h"
#include "IDb3DCommandLine.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbEraseHandler::finish()
{
	CDbHandlerInterface::finish();
 }

void CDbEraseHandler::begin()
{
	if (m_pOSG->GetSelectObjCount() == 0)
	{
		m_pOSG->m_pSelector->selectType = 2;
		m_pOSG->m_pSelector->begin(_T("选择对象："));

		status = 1;
	}
	else
	{
		status = 2;
	}
}


void CDbEraseHandler::DoEraseObjects(CViewControlData* m_pOSG)
{
	std::map<CString, osg::ref_ptr<osg::Node> > selects;
	m_pOSG->m_pSelector->getSelectSet(selects);
	std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = selects.begin();

	std::vector<CDbObjInterface*> delDatas;

	for (; it != selects.end(); it++)
	{
		CString strObj = it->first;

		CDbObjInterface* pData = m_pOSG->FindObj(strObj, GetObjType(it->second));
		if (pData &&  m_pOSG->canBeDel(pData))
		{
			delDatas.push_back(pData);
		}
	}
	m_pOSG->delData(delDatas);

	m_pOSG->m_pSelector->clearSelectSet();
	//refactor  wangrui
	/*	property_grid_interface* pGrid = get_property_grid_instance();
		if (pGrid)
		{
		pGrid->clear();
	}*/
}

bool CDbEraseHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
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
	if (status == 1)
	{
		if (m_pOSG->m_pSelector->IsFinish())
		{
			DoEraseObjects(m_pOSG);	 
	 
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
	else if (status == 2)
	{
		DoEraseObjects(m_pOSG);

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
	return false;
}