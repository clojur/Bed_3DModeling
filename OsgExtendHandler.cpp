#include "stdafx.h"

#include "osgwraper.h"
#include "osgtools.h"

#include "BoxSelector.h"
#include "osgExtendHandler.h"
#include "PickHandler.h"

 #include "IDb3DCommandLine.h"
#include "compare_tools.h"

 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CDbExtendHandler::finish()
{	
	CDbHandlerInterface::finish();

	/*for(int i=0; i< borderObjects.size(); i++)
	{
		m_pOSG->clearBoundingBoxObj(borderObjects[i]->GetName());
	}*/
}

void CDbExtendHandler::begin()
{
	if(m_pOSG->GetSelectObjCount() > 0 )
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("ѡ��Ҫ����Ķ���"));
		}
		
		status=2;
		m_pOSG->m_pSelector->begin();
		m_pOSG->m_pSelector->selectType = 8;
	}
	else
	{
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("ѡ�����"));
		}
	
		m_pOSG->m_pSelector->begin();
		m_pOSG->m_pSelector->selectType = 2;
		status++;
	}
}
void CDbExtendHandler::extentObjects( )
{
	std::vector< CDbObjInterface*> TrimOldObjs;
	std::vector< CDbObjInterface*> TrimNewObjs;
	std::vector< CDbObjInterface*> BorderNewObjs;

	m_pOSG->beginOperation("�ӳ�����");
	for (int i = 0; i < curOperatorObjects.size(); ++i)
	{
		CDbObjInterface* pObj = curOperatorObjects[i];		 

		//����Ƿ�յģ������ӳ�
		if (pObj->IsClose() )
			continue;

		osg::Vec3 interPT;
		BOOL flag = pObj->HitPositin(curPT1, curPT2, interPT);
		if (!flag)
			continue;

		double p = pObj->GetParameter(interPT);
		double firstValue = pObj->FirstParameter();
		double lastValue = pObj->LastParameter();

		double dExtend = 0;

		BOOL bExtendStart = FALSE;
		//�ӳ��׶�
		if (cdb_compare_tools::is_less(p, (firstValue + lastValue) / 2))
		{
			bExtendStart = TRUE;
		}
		else
		{
			bExtendStart = FALSE;
		}

		for (int j = 0; j < borderObjects.size(); ++j)
		{
			CDbObjInterface* pOhterObj = borderObjects[j];

			if (pOhterObj == pObj)
				continue;

			BOOL flag=pObj->Extend(dExtend, pOhterObj, bExtendStart);
			if (flag)
			{
				m_pOSG->updateData(pObj);
				break;
			}
		}
	}
	m_pOSG->endOperation();
}

void CDbExtendHandler::trimObjects( )
{	
	std::vector< CDbObjInterface*> TrimOldObjs;
	std::vector< CDbObjInterface*> TrimNewObjs;
	std::vector< CDbObjInterface*> BorderNewObjs;

	m_pOSG->beginOperation("�ü�����");

	for (int i=0;i<curOperatorObjects.size();++i)
	{
		CDbObjInterface* pObj = curOperatorObjects[i];
		double firstParam = pObj->FirstParameter();
		double lastParam  = pObj->LastParameter();

		//�õ���߽�Ľ���
		std::vector<double> CrossPtParam;
		GetBorderCrossPoints_Trim(pObj,CrossPtParam);

		int nCrossPtCount = CrossPtParam.size();

		//û�н��㣬���òü�
		if (nCrossPtCount==0)
			continue;

		if (!pObj->IsClose() )
		{
			CrossPtParam.push_back( pObj->FirstParameter());
			CrossPtParam.push_back(pObj->LastParameter());
		}

		std::sort(CrossPtParam.begin(), CrossPtParam.end() );

		int index = -1;

		osg::Vec3 interPT;
		BOOL flag = pObj->HitPositin(curPT1, curPT2,interPT);		

		if (!flag)
			return;

		double p = pObj->GetParameter(interPT);

		for(int j=0;j< CrossPtParam.size(); j++)
		{
			if(CrossPtParam[j] >=p )
			{
				index = j-1;
				break;
			}
		}
		std::vector< CDbObjInterface*> result;


		double t1 ;
		double t2;
		if (pObj->IsClose() && (index == -1 || index == nCrossPtCount - 1))//
		{
			t1 = CrossPtParam[nCrossPtCount-1];
			t2 = CrossPtParam[0];			
		}
		else
		{
			t1 = CrossPtParam[index];
			t2 = CrossPtParam[index+1];
		}

		flag = pObj->Trim(t1, t2, result);
		if (flag)
		{
			if (flag == 2)
			{
				m_pOSG->updateData(pObj);
				for (int j = 0; j < result.size(); j++)
					TrimNewObjs.push_back(result[j]);
			}
			else
			{
				TrimOldObjs.push_back(pObj);

				for (int j = 0; j < result.size(); j++)
					TrimNewObjs.push_back(result[j]);
			}

		}


		for(int i=0; i< borderObjects.size() ; i++ )
		{
			if(  pObj == borderObjects[i] )
			{
				if (flag!=2)
					borderObjects.erase(borderObjects.begin()+i);

				for(int j=0; j< TrimNewObjs.size(); j++)
				{
					borderObjects.push_back(TrimNewObjs[j]);
				}
				break;
			}
		}
	}

	//del
	{	
		m_pOSG->delData(TrimOldObjs);
	}
	//add
	{
 		for(int i=0; i< TrimNewObjs.size(); i++)
		{
			CDbObjInterface* pNewObj = TrimNewObjs[i];
			CString strOldTitle = pNewObj->GetTitle();

			CString newTitle;
			newTitle.Format("%s_trim%d", pNewObj->GetTitle(), i+1);
			pNewObj->SetTitle(newTitle);			
			//set id �����������
			{
				pNewObj->SetID(CDbOSGDataCenter::GetID());
				CString newName;
				newName.Format("%s%d", pNewObj->GetRTType(), pNewObj->GetID());
				pNewObj->SetName(newName);
			}
			m_pOSG->addData(pNewObj);			

			m_pOSG->PostTrimNew(pNewObj, strOldTitle);
 		}		
	}
	m_pOSG->endOperation();
}

//�������pObject��߽�m_Borders�Ľ���
void CDbExtendHandler::GetBorderCrossPoints_Extend( CDbObjInterface* pObject, std::vector<double> &leftCrossPts,std::vector<double> &rightCrossPt)
{
	double first =pObject->FirstParameter();
	double last  =pObject->LastParameter();

	//�ӳ��������еı߽���			
	for (int j=0;j<borderObjects.size();++j)
	{
		CDbObjInterface* pOhterObj = borderObjects[j];

		//�����Լ���
		if (pOhterObj->GetName() == pObject->GetName())
			continue;
		
		std::vector<osg::Vec3d> crossPts;
		pObject->GetCrossPoints(pOhterObj,crossPts,TRUE);

		//ֻȡ�ӳ��ߵĽ���
		for (int i=0;i<crossPts.size();++i)
		{
			double temp =pObject->GetParameter(crossPts[i]);
			if (cdb_compare_tools::is_less(temp, first, 1.0e-3))
			{
				leftCrossPts.push_back(temp);
			}
			else if (cdb_compare_tools::is_great(temp, last, 1.0e-3))
			{
				rightCrossPt.push_back(temp);
			}	
		}
	}	

	//����
	std::sort(leftCrossPts.begin(),leftCrossPts.end());
	std::sort(rightCrossPt.begin(),rightCrossPt.end());
}
void CDbExtendHandler::GetBorderCrossPoints_Trim( CDbObjInterface* pObject, std::vector<double> &CrossPtParam)
{
	//�������еı߽���
	for (int j=0;j<borderObjects.size();++j)
	{
		CDbObjInterface* pOhterObj = borderObjects[j];

		//�����Լ���
		if (pOhterObj->GetName() == pObject->GetName())
			continue;

		std::vector<osg::Vec3d> crossPts;
		pObject->GetCrossPoints(pOhterObj,crossPts,FALSE);

		for (int i=0;i<crossPts.size();++i)
		{
			double temp =pObject->GetParameter(crossPts[i]);
			CrossPtParam.push_back(temp);
		}
	}
	//����
	std::sort(CrossPtParam.begin(),CrossPtParam.end());
}

void CDbExtendHandler::Action( BOOL bCancle)
{
	if(bCancle)
		return;
	m_pOSG->m_bInstallValid = FALSE;

	if(bTrim)
		trimObjects();
	else
		extentObjects();

	m_pOSG->m_bInstallValid = TRUE;

}

bool CDbExtendHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;

	if( IsEscaped(ea) ) //escape ������
	{
		if (status >2)
			Action(TRUE);
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
				std::map<CString, osg::ref_ptr<osg::Node>>selectobj;
				m_pOSG->m_pSelector->getSelectSet( selectobj);
				std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectobj.begin();
				for(; it != selectobj.end() ; it++ )
				{
					CDbObjInterface* pBorderObj = m_pOSG->PreTrim(m_pOSG->FindObj(it->first, GetObjType(it->second)));
					if (pBorderObj && pBorderObj->CanBeBorder() )
						borderObjects.push_back(pBorderObj);
				}
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("ѡ��Ҫ�ü��Ķ��󣬻�shiftѡ��Ҫ����Ķ���"));
				}
					
				status=2;
	
				m_pOSG->m_pSelector->onlyClearSelectSet();

				m_pOSG->m_pSelector->begin();
				m_pOSG->m_pSelector->selectType = 8;
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
		if(m_pOSG->m_pSelector->IsFinish() )
		{
			if(m_pOSG->GetSelectObjCount() > 0 )
			{		
				std::map<CString, osg::ref_ptr<osg::Node>>selectobj;
				m_pOSG->m_pSelector->getSelectSet( selectobj);
				std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectobj.begin();
				for(; it != selectobj.end() ; it++ )
				{
					CDbObjInterface* pData = m_pOSG->FindObj( it->first ,GetObjType(it->second ) );
					if (pData)
					{
						curOperatorObjects.push_back(m_pOSG->PreTrim(pData));
					}
				}

				 curPT1 = m_pOSG->getWorldPos( m_pOSG->m_pSelector->startPT ,FALSE  );
				 curPT2 = m_pOSG->getWorldPos( m_pOSG->m_pSelector->endPT ,FALSE  );

				 Action( FALSE);

				m_pOSG->m_pSelector->onlyClearSelectSet();
				curOperatorObjects.clear();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T("ѡ��Ҫ�ü��Ķ��󣬻�shiftѡ��Ҫ����Ķ���"));
				}
				
				status=2;
				m_pOSG->m_pSelector->begin();
				m_pOSG->m_pSelector->selectType = 8;
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
	return false;
}
