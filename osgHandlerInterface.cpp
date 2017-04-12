#include "stdafx.h"

#include "osgHandlerinterface.h"
#include "osgTools.h"
#include "boxselector.h"
#include "CommonFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


osgHandlerInterface::osgHandlerInterface(CViewControlData* pOSG)
{
	m_pOSG = pOSG;
	SendPointFlag = OSG_CMDLINE;
	strName = "未知命令";
	lastConfirmPointBeValid = FALSE;
	//m_OldbOrtho = pOSG->m_bOrtho;
}
void osgHandlerInterface::finish()
{
	//m_pOSG->m_bOrtho = m_OldbOrtho;

	status=0;
	lastConfirmPointBeValid = FALSE;
	m_pOSG->bEnableSnap = FALSE;
	m_pOSG->bClearSnap    =TRUE;
	m_pOSG->m_command = 0;

	m_pOSG->m_pSelector->clearSelectSet();

	m_pOSG->SetCursorType(OSG_NORMAL_CUSSOR);

	m_pOSG->m_bInstallValid = TRUE;

	m_pOSG->m_pSelector->selectType = 1;
	std::vector<CString> flitter;
	m_pOSG->m_pSelector->setSelectFliter(flitter);//zmz

	m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);


}
void osgHandlerInterface::SetConformPoint(osg::Vec3d confirmPT)
{
	lastConfirmPointBeValid = TRUE;
	lastConfirmPoint = confirmPT; 
}

void osgHandlerInterface::clearCloneObject()
{
	//清除临时对象；
	std::map<osg::Node* , C3DInterface*>::iterator it2 = selectetSetMap.begin();
	for( ; it2 != selectetSetMap.end() ; it2++)
	{
		m_pOSG->delCloneObjes.push_back( it2->second);
	}
	selectetSetMap.clear();
}



double osgHandlerInterface::GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
{
	double x =   ea.getX();
	if(viewer)
	{
		x-=viewer->getCamera()->getViewport()->x();
	}
	return x;
}

double osgHandlerInterface::GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
{
	double y =  ea.getY();
	if(viewer)
	{
		y-=viewer->getCamera()->getViewport()->y();
	}
	return y;
}

void GetCurrentPos(osg::Vec3d& p11, osg::Vec3d& p22, CString& strPos, BOOL bRelative)
{
	strPos.Replace("，", ",");
	strPos.Replace("；", ";");

	int pos = strPos.Find("@");

	if (pos == -1) // 
	{
		int pos2 = strPos.Find(",");
		if (pos2 == -1) //表示相对当前的方向
			bRelative = TRUE;
		else
			bRelative = FALSE;

		if (bRelative)  
		{
			double dist = atof(strPos);
			//dir
			osg::Vec3d dir = p22 - p11;
			if (dir.length2() < 1.0e-6)
				dir.set(1, 0, 0);
			dir.normalize();
			p22 = p11 + dir*dist;
		}	 
		else //绝对坐标输入
		{
			p22 = GetPosition(strPos);
		}
	}
	else
	{
		CString revPos = strPos.Mid(pos + 1);

		std::vector<CString> result;
		//refacor wr
		//string_tools::split_string(result,revPos);
 
		double dx = 0;
		double dy = 0;
		double dz = 0;

		if ( result.size() >0 )
			dx = atof(result[0]);
		if (result.size() >1)
		{
 			dy = atof(result[1]);
		}
		if (result.size() > 2)
		{
 			dz = atof(result[2]);
		} 

		p22 = p11 + osg::Vec3d(dx, dy, dz);

	}
}


void CopyObjectAndAdd(CViewControlData* m_pOSG, C3DInterface* pCreateData)
{
	CString strPrix;
	strPrix.Format("%s_", pCreateData->GetTitle());


	pCreateData->SetID(COSGDataCenter::GetID());

	CString newName;
	newName.Format("%s%d", pCreateData->GetRTType(), pCreateData->GetID());
	pCreateData->SetName(newName);

	if (1)
	{
		std::vector<CString> allName = m_pOSG->GetObjTitlesByType(pCreateData->GetRTType());
		pCreateData->SetTitle(GetNonExistName(allName, "", strPrix));
	}
	else
	{
		pCreateData->SetTitle(m_pOSG->GetDefaultTitle(pCreateData->GetRTType()));
	}

	m_pOSG->addData(pCreateData);
}
