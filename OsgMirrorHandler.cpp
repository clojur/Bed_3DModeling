#include "stdafx.h"
 
#include "osgwraper.h"
#include "osgtools.h"

#include "BoxSelector.h"
#include "osgMirrorHandler.h"
#include "IDb3DCommandLine.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDbMirrorHandler::CDbMirrorHandler(CViewControlData* pOSG) :CDbHandlerInterface(pOSG)
{
	status = 0;
	pCamera = NULL;
	pGeometry = NULL;
	pLastMatrix = NULL;
	strName = "mirror";
	pParentNode = NULL;
}

void CDbMirrorHandler::finish()
{
	CDbHandlerInterface::finish();

 	m_pOSG->clearOverLayoutObject(pCamera);	
	std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for(; it != selectetSetMap.end(); it++)
	{
		m_pOSG->clearTempObject( it->first);	
	}
	selectetSetMap.clear(); 

	pParentNode.release();
}

void CDbMirrorHandler::begin()
{
	m_pOSG->bEnableSnap =TRUE;
	if(m_pOSG->GetSelectObjCount() > 0 )
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("输入基点："));
		}

		 m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
		 SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE);
		 status = 2;
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
		status=1;
	}
}


void CDbMirrorHandler::action( )
{
	m_pOSG->m_bInstallValid = FALSE;

	m_pOSG->beginOperation("镜像对象");

 	std::vector<CDbObjInterface*> realCopyObjs = m_pOSG->OnPreCopy(selectetSetMap);
	for (int i = 0; i < realCopyObjs.size(); i++)
	{
		if (status > 3)
		{
			CDbObjInterface* pData = NULL;
			if (bKeepSource)
				pData = realCopyObjs[i];
			if (!bKeepSource)
				pData = m_pOSG->FindObj(realCopyObjs[i]->GetName(), realCopyObjs[i]->GetRTType());

			osg::Matrix pMatrix;
			GetMirrorMatrix(pMatrix);
			pData->Mirror(&pMatrix);

			if (bKeepSource)
			{
				CopyObjectAndAdd(m_pOSG, pData);
			}
			else
			{
				m_pOSG->updateData(pData);
			}
		}
	}

	std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for (; it != selectetSetMap.end(); it++)
	{
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

	m_pOSG->endOperation();

	m_pOSG->m_bInstallValid = TRUE;

}
void CDbMirrorHandler::GetMirrorMatrix(osg::Matrix& pMatrix )
{
	osg::Vec3  normal = getMirrorPlane() ;
	osg::Plane plane;
	plane.set( normal , p22);	
	
	double dis = plane.distance( osg::Vec3(0,0,0) );
	{
		double m00 =1-2*normal.x()*normal.x();
		double m01 =  -2*normal.x()*normal.y();
		double m02 =  -2*normal.x()*normal.z();
		double m03 =0;
		//
		double m10 =   -2*normal.x()*normal.y();
		double m11 = 1-2*normal.y()*normal.y();
		double m12 =  -2*normal.y()*normal.z();
		double m13 =    0;
		//
		double m20=  -2*normal.x()*normal.z();
		double m21 = -2*normal.y()*normal.z();
		double m22=1-2*normal.z()*normal.z();
		double m23=0;
		//
		double m30 = -2*normal.x()*dis;
		double m31 = -2*normal.y()*dis;
		double m32 = -2*normal.z()*dis;
		double m33 =1;

		pMatrix.set(m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23,m30,m31,m32,m33);
	}
		
}

void CDbMirrorHandler::mirrorObject()
{
	std::map<osg::Node* , CDbObjInterface*>::iterator it = selectetSetMap.begin();
	for( ; it != selectetSetMap.end() ; it++ )
	{
		osg::Node*      pNode = it->first;
		CDbObjInterface* pData   =it->second;

		osg::Matrix pMatrix;
		GetMirrorMatrix(pMatrix );

		if(pLastMatrix)
		{
			pData->Mirror(pLastMatrix);
		}
		else
		{
			pLastMatrix = new osg::Matrix;
		}
		pData->Mirror( &pMatrix);

		pLastMatrix->set(pMatrix);

		m_pOSG->addUpdateObject( pData, pNode);
	}
}

osg::Vec3  CDbMirrorHandler::getMirrorPlane()
{
	osg::Vec3 normal(1,0,0);
	osg::Plane plane;
	osg::Vec3 otherPT;


	int viewType = m_pOSG->viewType;
	

 	if(viewType ==1 || viewType ==5)//xy 平面
	{
		otherPT.set(p11.x() , p11.y() , p11.z()-999 );		 
	}
	else if (viewType == 2 || viewType == 6) //xz 平面
	{
		otherPT.set(p11.x(),  p11.y()-999, p11.z() );		 
	}
	else if (viewType == 3 || viewType == 7) //yz 平面
	{
		otherPT.set( p11.x()-999, p11.y(), p11.z()  );	 
	}
	else if (viewType >= 4)
	{
		otherPT.set(999, 999, 0.5*(p11.z() + p22.z()));
	}
	plane.set(p11, p22, otherPT );
	normal = plane.getNormal();
 	return normal;
}

bool CDbMirrorHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	double _mx = GetX(ea,viewer);
	double _my = GetY(ea,viewer);
	if( IsEscaped(ea) ) //escape 被按下
	{   	
		if(status >1  )
		{
			action();			
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
			m_pOSG->m_pSelector->finish();

			if(m_pOSG->GetSelectObjCount() > 0 )
			{
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("输入基点："));
				}
				
				 m_pOSG->SetCursorType(OSG_INPUT_CUSSOR);
				 SetSendPointFlag(OSG_LBTNDOWN | OSG_MOUSEMOVE);
				 status = 2;
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
		return false;
	}
	else if(status ==2)
	{
	 if (IsLButtonDown(ea))
		{
				p11=m_pOSG->getWorldPos(  _mx,_my,TRUE );

				startPT = m_pOSG->getWindosPos(p11);
				SetConformPoint(p11);



				//橡皮筋
				pCamera  =createHudLine(m_pOSG, startPT,startPT,pGeometry);
				m_pOSG->addOverLayoutObject( pCamera);	

				//临时对象
				std::vector< osg::Node* > tempObj;
				m_pOSG->m_pSelector->GetCloneSelectObj(tempObj,0);
				std::vector< osg::Node* >::iterator it = tempObj.begin();
				for( ; it != tempObj.end() ; it++)
				{
					osg::Node* pNode =*it;
					CDbObjInterface* pObj =  m_pOSG->FindObj(  pNode->getName().c_str() ,GetObjType(pNode)  );
					if(pObj)
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
					cmdLine->AddPrompt(_T("指定第二个点："));
				}
				
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
	 if (IsLButtonDown(ea))
		{	
	/*		if(pCamera)
			{
				m_pOSG->clearOverLayoutObject(pCamera);	
				pCamera = NULL;
			}*/

			p22= m_pOSG->getWorldPos(GetX(ea,viewer), GetY(ea,viewer) , TRUE );
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt("是否删除原对象？[是(Y)否(N)]<N>：");
			}
			

			status=4;
			return false;
		}
		else if( ea.getEventType() ==osgGA::GUIEventAdapter::MOVE )
		{		
			p22    = m_pOSG->getWorldPos(GetX(ea,viewer), GetY(ea,viewer) , TRUE  );

			endPT= m_pOSG->getWindosPos(  p22 );		 
			startPT = m_pOSG->getWindosPos(p11);
			m_pOSG->addRubbline(pGeometry, startPT, endPT);

			//临时对象
			std::map<osg::Node*, CDbObjInterface*>::iterator it = selectetSetMap.begin();
			for (; it != selectetSetMap.end(); it++)
			{
				osg::Node* pNode = it->first;
				osg::Matrix m;
				GetMirrorMatrix(m);
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
	}
	else if(status ==4)
	{
		if (IsReturn(ea) || IsRButtonDown(ea))   //D
		{	
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
				CString strPos = cmdLine->GetCurText();
				if (strPos.CompareNoCase("Y") == 0)
				{
					bKeepSource = FALSE;
				}
				else
					bKeepSource = TRUE;
			}
		
			action();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}
		
			finish();		
		}		
	}
	return false;
}