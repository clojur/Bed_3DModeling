#include "stdafx.h"

#include "osgwraper.h"
#include "osgtools.h"

#include "osg/linewidth"

 
#include "BoxSelector.h"
#include "IDb3DCommandline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDbBoxSelector::CDbBoxSelector(CViewControlData* pOSG)
{
	m_pOSG=pOSG;
	status=0;
	selectType=1;
	//pUserWnd = NULL;
}
//设置选择过滤器
void CDbBoxSelector::setSelectFilter(std::vector<CString> selectFilter)
{
	m_selectFilter = selectFilter;
}

void CDbBoxSelector::clearSelectFilter()
{
	m_selectFilter.clear();
}

void CDbBoxSelector::finish()
{
	status =0;
	m_pOSG->SetCursorType(OSG_NORMAL_CUSSOR);
	selectType=1;
	//refacor wr
	//if (pUserWnd)
	//{
	//	std::vector<CString> selectResult;
	//	std::vector<CDbObjInterface*> allObject = GetSpeSelectObjects(m_selectFliter);
	//	for (int i = 0; i < allObject.size(); i++)
	//		selectResult.push_back(allObject[i]->GetName());

	//	m_selectFliter.clear();

	//	pUserWnd->end_select(selectResult);
	//}
	//pUserWnd = NULL;
}
void CDbBoxSelector::begin(const CString& strPropt)
{
	//pUserWnd = pWnd;

	m_pOSG->SetCursorType(OSG_SELECT_CUSSOR);
	m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);

 	status=1;
	auto cmdLine = GetDb3DCommandLine();
	if(selectType <16)
	{
		if (cmdLine)
		{
			if (strPropt.IsEmpty())
				cmdLine->AddPrompt(_T("选择对象："));
			else
				cmdLine->AddPrompt(strPropt);
		}
	

	}
	else if(selectType ==16)
	{
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T("选择夹点："));
		}
		 
	}
}
//refacor by wr
//void CDbBoxSelector::begin(const CString& strPropt, selector_interface* pWnd)
//{
//	//pUserWnd = pWnd;
//
//	m_pOSG->SetCursorType(OSG_SELECT_CUSSOR);
//	m_pOSG->GetCurrentCWnd()->SendMessage(WM_SETCURSOR, 0, 0);
//
// 	status=1;
//	auto cmdLine = GetDb3DCommandLine();
//	if(selectType <16)
//	{
//		if (cmdLine)
//		{
//			if (strPropt.IsEmpty())
//				cmdLine->AddPrompt(_T("选择对象："));
//			else
//				cmdLine->AddPrompt(strPropt);
//		}
//	
//
//	}
//	else if(selectType ==16)
//	{
//		if (cmdLine)
//		{
//			cmdLine->AddPrompt(_T("选择夹点："));
//		}
//		 
//	}
//}
void CDbBoxSelector::begin(int type, const CString& strProp)
{
	selectType = type;
	begin(strProp);
}

bool CDbBoxSelector::IsFinish()
{
	if(status ==0)
	{
		return true;
	}
	else
		return false;
}
//

double CDbBoxSelector::GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
{
	double x =  ea.getX();
	if(viewer)
	{
		x-=viewer->getCamera()->getViewport()->x();
	}
	return x;
}

double CDbBoxSelector::GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer)
{
	double y =   ea.getY();
	if(viewer)
	{
		y-=viewer->getCamera()->getViewport()->y();
	}
	return y;
}
bool CDbBoxSelector::ApplySelect(osg::Node* pNode, BOOL bBox)
{
	if (pNode)
	{
		CString objType = GetObjType(pNode);
		if (objType == OT_UNKNOWN)
			return false;
		if (bBox && objType == OT_DIM)
			return false;

		if (!pNode->getBound().valid())
			return false;

		if (!m_selectFilter.empty())
		{
			if (std::find(m_selectFilter.begin(), m_selectFilter.end(), objType) != m_selectFilter.end())
			{
				return true;
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CDbBoxSelector::ApplySelect(CDbObjInterface* pData)
{
	if (pData)
	{
		CString objType = pData->GetRTType();
		if (!m_selectFilter.empty())
		{
			if (std::find(m_selectFilter.begin(), m_selectFilter.end(), objType) != m_selectFilter.end())
			{
				return true;
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CDbBoxSelector::ApplySelect(osg::Node* pNode)
{
	return ApplySelect(pNode, FALSE);
}

bool CDbBoxSelector::DoSinglePick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;

	std::vector<osg::Node*> selectObjs;

	osg::Vec3 insectPoint;
	osg::Node* pNode = m_pOSG->pickSingle(ea.getXnormalized(), ea.getYnormalized(), viewer, TRUE, TRUE,insectPoint);

	if(pNode )
	{	
		selectObjs.push_back( pNode);
		if(selectType <16)
		{
			BOOL unSelectMode = FALSE;
			int modKeyMask = ea.getModKeyMask();

			if ((modKeyMask& osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT))
			{
				unSelectMode = TRUE;
			}

			if (unSelectMode) //退选
			{
 				MakeObjectUnSelected(selectObjs, m_pOSG);
			}
			else
			{
				m_pOSG->coordPos = insectPoint;
				MakeObjectSelected(selectObjs, m_pOSG);
			}
		}
	
		m_pOSG->SetRubboxVisible(FALSE);

		return true;
	}
	return false;
}

bool CDbBoxSelector::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	
	double _mx = GetX(ea, viewer);
	double _my = GetY(ea, viewer);

	if(IsEscaped(ea))
	{	
	
		m_pOSG->SetRubboxVisible(FALSE);
		auto cmdLine = GetDb3DCommandLine();
		if (cmdLine)
		{
			cmdLine->AddPrompt(_T(""));
		}
		
		finish();
		return false;
	}		
	if(status == 1)
	{	
		if (osgGA::GUIEventAdapter::PUSH == ea.getEventType() && ea.getButton() != osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
		{
			if (IsRButtonDown(ea) )
			{
				finish();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine)
				{
					cmdLine->AddPrompt(_T(""));
				}
				return false;
			}
			else
			{
				p11 = m_pOSG->getWorldPos(_mx, _my, FALSE);


				startPT = m_pOSG->getWindosPos(p11);
				endPT = startPT;

				x1 = ea.getXnormalized();
				y1 = ea.getYnormalized();

				startPT.x() -= 10;
				startPT.y() -= 10;
				endPT.x() += 10;
				endPT.y() += 10;

				m_pOSG->SetCursorType(OSG_NoCursor);
				if (DoSinglePick(ea, aa))
				{
					if (selectType == 1)
					{
						finish();
					
						auto  cmdLine = GetDb3DCommandLine();
						if (cmdLine != NULL)
						{
							cmdLine->AddPrompt(_T(""));
						}
					}
					else if (selectType == 2)
					{
						begin();
					
						auto  cmdLine = GetDb3DCommandLine();
						if (cmdLine != NULL)
						{
							cmdLine->AddPrompt(_T("选择对象："));
						}
					}
					else if (selectType == 8)
					{
						finish();
			
						auto  cmdLine = GetDb3DCommandLine();
						if (cmdLine != NULL)
						{
							cmdLine->AddPrompt(_T(""));
						}
					}
					else if (selectType == 16)
					{
						begin();
						auto  cmdLine = GetDb3DCommandLine();
						if (cmdLine != NULL)
						{
							cmdLine->AddPrompt(_T("选择夹点："));
						}
					
					}
					return false;
				}
				else
				{
					if (selectType == 8)
					{
 						m_pOSG->SetCursorType(OSG_SELECT_CUSSOR);
						auto  cmdLine = GetDb3DCommandLine();
						if (cmdLine != NULL)
						{
							cmdLine->AddPrompt(_T("选择对象："));
						}
					
						return false;
					}
					else
					{

						m_pOSG->InitRubbox( startPT, endPT);


						status = 2;

						if (selectType == 1)
						{
							auto  cmdLine = GetDb3DCommandLine();
							if (cmdLine!=NULL)
							{
								cmdLine->AddPrompt(_T("选择对角点："), FALSE);
							}
							
						}
					}
					
					return false;
				}
			}
		}
		else if ( IsReturn(ea))
		{
			finish();
			auto cmdLine = GetDb3DCommandLine();
			if (cmdLine)
			{
				cmdLine->AddPrompt(_T(""));
			}
			return false;
		}
	
		return false;
	}
	else if(status == 2)
	{	
		if (osgGA::GUIEventAdapter::PUSH == ea.getEventType() && ea.getButton() != osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
		{
			p22       = m_pOSG->getWorldPos(_mx, _my, TRUE);
			endPT   = m_pOSG->getWindosPos(p22);
			startPT  = m_pOSG->getWindosPos(p11);
			m_pOSG->UpdateRubbox(startPT, endPT);
			if (selectType == 16)
			{
				UpdateStretchHandler(startPT, endPT);
			}
			/////////////////////////////////////////////////////////////////
			std::vector<osg::Node*> selectObjs;
			if (startPT.x() == GetX(ea, viewer) && startPT.y() == GetY(ea, viewer))
			{
				osg::Vec3 insectPoint;

				osg::Node* pNode = m_pOSG->pickSingle(ea.getXnormalized(), ea.getYnormalized(), viewer, TRUE, TRUE,insectPoint);
				if (pNode)
				{
					selectObjs.push_back(pNode);
				}
			}
			else
			{
				bool bReserveSelect = false;

				double mx0 = x1;
				double my0 = y1;

				double mx1 = ea.getXnormalized();
				double my1 = ea.getYnormalized();

				if (mx0 > mx1)
				{			
					bReserveSelect = true;
				}

				{
					osg::Viewport* viewport = viewer->getCamera()->getViewport();

					double mx00 = viewport->x() + (int)((double)viewport->width()*(mx0*0.5 + 0.5));
					double my00 = viewport->y() + (int)((double)viewport->height()*(my0*0.5 + 0.5));
					double mx11 = viewport->x() + (int)((double)viewport->width()*(mx1*0.5 + 0.5));
					double my11 = viewport->y() + (int)((double)viewport->height()*(my1*0.5 + 0.5));
					selectObjs = m_pOSG->BoxPick(mx00, my00, mx11, my11, viewer, bReserveSelect);
				}	
			}
			if (selectObjs.size() > 0)
			{
				if (selectType < 16)
				{
					BOOL unSelectMode = FALSE;
					int modKeyMask = ea.getModKeyMask();
					if ((modKeyMask& osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT))
					{
						unSelectMode = TRUE;
					}
					if (unSelectMode)
					{
						MakeObjectUnSelected(selectObjs, m_pOSG);
					}
					else
					{
						MakeObjectSelected(selectObjs, m_pOSG);
					}
				}
			}

			m_pOSG->SetRubboxVisible(FALSE);

			if (selectType == 1 || selectType ==8)
			{
				finish();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine!=NULL)
				{
					cmdLine->AddPrompt(_T(""));
				}
	
			}
			else if (selectType == 2)
			{
				begin();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine != NULL)
				{
					cmdLine->AddPrompt(_T("选择对象："));
				}

			}
			else if (selectType == 16)
			{
				begin();
				auto cmdLine = GetDb3DCommandLine();
				if (cmdLine != NULL)
				{
					cmdLine->AddPrompt(_T("选择夹点："));
				}

			}
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::MOVE || ea.getEventType() == osgGA::GUIEventAdapter::DRAG)
		{				 
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), FALSE);
			endPT   = m_pOSG->getWindosPos(p22);
			startPT = m_pOSG->getWindosPos(p11);

			m_pOSG->UpdateRubbox( startPT, endPT);

			return false;
		}
		else if (ea.getEventType() == osgGA::GUIEventAdapter::SCROLL)
		{
			p22 = m_pOSG->getWorldPos(GetX(ea, viewer), GetY(ea, viewer), TRUE);
			endPT   = m_pOSG->getWindosPos(p22);
			startPT = m_pOSG->getWindosPos(p11);

			m_pOSG->UpdateRubbox(startPT, endPT);
			return false;
		}
		return false;
	}	
	return false;
}


////////////////////////////////////////////////

void CDbBoxSelectorHandler::finish()
{
	status=0;
	m_pOSG->m_command = 0;
}

void CDbBoxSelectorHandler::begin()
{
	status=1;
	m_pOSG->m_pSelector->selectType=2;
	m_pOSG->m_pSelector->begin();
	auto cmdLine = GetDb3DCommandLine();
	if (cmdLine)
	{
		cmdLine->AddPrompt(_T("选择对象："));
	}
	 
}

bool CDbBoxSelectorHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);

	if (!viewer) return false;
	if(m_pOSG->m_pSelector->IsFinish() )
	{
		finish();
	}
	else
	{
		 m_pOSG->m_pSelector->handle( ea, aa);
	}
	return false;
}

bool CDbBoxSelector::IsSelected(const CString& strObj)
{
	if( selectetSet.find(strObj) != selectetSet.end()  )
	{
		return true;
	}
	return false;
}

bool CDbBoxSelector::addToSet(CString tempSelectObjName, osg::Node* tempSelectObj)
{
	if(tempSelectObjName == "hudline" || tempSelectObjName.IsEmpty() || tempSelectObjName == "HeightLightEffectNode" )
	{
		//ASSERT(FALSE);
		return false;
	}
	if(!IsSelected(tempSelectObjName) )
	{
		selectetSet[tempSelectObjName]=tempSelectObj;
		selectetSetBak[tempSelectObjName] = tempSelectObj;
	}
	return true; //已经被选中了；
}
void CDbBoxSelector::onlyClearSelectSet()
{
	selectetSet.clear();
}

void CDbBoxSelector::clearSelectSet()
{
	std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = selectetSetBak.begin();
	for (; it != selectetSetBak.end(); it++)
	{
		m_pOSG->clearBoundingBoxObj(it->first);
	}
	selectetSet.clear();
	selectetSetBak.clear();
}


void CDbBoxSelector::getSelectSet(std::map<CString, osg::ref_ptr<osg::Node>>& selectMap)
{
	selectMap = selectetSet;
}


int CDbBoxSelector::GetSelectNodeCount()
{
	return selectetSet.size();
}


std::vector<int> CDbBoxSelector::GetStretchIndex(const CString& strObj)
{
	std::map<CString, std::vector<int> >::iterator it = selectObjStretchIndex.find(strObj);
	if(it != selectObjStretchIndex.end() )
		return it->second;
		
	return std::vector<int>();
}

int CDbBoxSelector::GetSelectHotPointCount()
{
	int nCount = 0;

	std::map<CString, std::vector<int> >::iterator it = selectObjStretchIndex.begin();
	for(; it != selectObjStretchIndex.end() ; it++)
	{		
		nCount += it->second.size();
	}

	return nCount;
}

BOOL  CDbBoxSelector::bInStretchHandler(osg::Vec2 p1, osg::Vec2 p2)
{
	std::vector<osg::Node*> handlers;
	selectObjStretchIndex.clear();

	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectetSet.begin();
	for (; it != selectetSet.end(); it++)
	{
		if (it->second)
		{
			CDbObjInterface* pData = m_pOSG->FindObj(it->first, GetObjType(it->second));
			if (pData)
			{
				std::vector<int>  stretchIndexs;
				std::vector<osg::Vec3d>  stretchPTs = pData->GetStretchPoint();
				for (int i = 0; i < stretchPTs.size(); i++)
				{
					osg::Vec2 pt = m_pOSG->getWindosPos(stretchPTs[i].x(), stretchPTs[i].y(), stretchPTs[i].z());
					if (PointIsInRect(pt, p1, p2))
					{
						return TRUE;					 
					}
				}
		 
			}
		}
	}
	return FALSE;
}

std::vector<osg::Node*> CDbBoxSelector::UpdateStretchHandler(osg::Vec2 p1, osg::Vec2 p2)
{
	stretchNodes.clear();

	std::vector<osg::Node*> handlers;
	selectObjStretchIndex.clear();

	std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = selectetSet.begin();
	for(; it != selectetSet.end() ; it++)
	{
		if(it->second )
		{
			CDbObjInterface* pData =  m_pOSG->FindObj( it->first,GetObjType(it->second) );
			if(pData)
			{
				bool bStretch = false;

				std::vector<int>  stretchIndexs;
				std::vector<osg::Vec3d>  stretchPTs =  pData->GetStretchPoint();
				for(int i=0; i<stretchPTs.size(); i++ )
				{
					osg::Vec2 pt = m_pOSG->getWindosPos(stretchPTs[i].x() , stretchPTs[i].y(), stretchPTs[i].z() );
					if( PointIsInRect( pt , p1 , p2))
					{
						bStretch = true;

						stretchIndexs.push_back( i);

						osg::Geode* pHandler= new osg::Geode;
						CreateHotHandler( stretchPTs[i] , stretchHandlerColor,pHandler , handlersize);
						handlers.push_back(pHandler);		

						m_pOSG->addOverLayoutObject( pHandler);

					}
				}
				if(stretchIndexs.size() > 0 )
				{
					selectObjStretchIndex[it->first] = stretchIndexs;
				}

				if (bStretch)
				{
					stretchNodes.push_back(it->second);
				}
			}
		}
	}
	return handlers;
}

CString CDbBoxSelector::GetSelectNodeName()
{
	if(selectetSet.size() ==1)
	{
		return selectetSet.begin()->first;
	}
	return "";
}

void CDbBoxSelector::GetCloneSelectObj(std::vector<osg::Node*>& tempObj,int type)
{
	std::map<CString, osg::ref_ptr<osg::Node> >::iterator it = selectetSet.begin();
	for(; it != selectetSet.end() ; it++)
	{
		if( it->second )
		{
			osg::Node*pClone = osg::clone(it->second.get(), osg::CopyOp(osg::CopyOp::DEEP_COPY_ALL));
			pClone->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
			tempObj.push_back(pClone);
		}
	}
}


void CDbBoxSelector::GetCloneStretchObj(std::vector<osg::Node*>& tempObj, int type)
{ 
	for (int i = 0; i < stretchNodes.size(); i++)
	{		
		osg::Node*pClone = osg::clone(stretchNodes[i] , osg::CopyOp(osg::CopyOp::DEEP_COPY_ALL));
		pClone->getOrCreateStateSet()->removeAttribute(osg::StateAttribute::LINESTIPPLE);
		tempObj.push_back(pClone);
	}
}
std::vector<osg::Node*> CDbBoxSelector::GetAllSelectNode()
{
	std::vector<osg::Node*> allNode;
	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectetSet.begin();
	for (; it != selectetSet.end(); it++)
	{
		allNode.push_back(it->second);
 	}
	return allNode;
}

osg::ref_ptr<osg::Node> CDbBoxSelector::GetLastSelectNode(const CString& strObj)
{
	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectetSetBak.find(strObj);
	if (it != selectetSetBak.end())
	{
		return it->second;
	}
	return NULL;
}
// 从选择集中查找指定类型对象
std::vector<CDbObjInterface*> CDbBoxSelector::GetSpeSelectObjects(const CString& objType)
{
	std::vector<CDbObjInterface*> datas;

	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectetSet.begin();
	for( ; it != selectetSet.end() ; it++)
	{
		CDbObjInterface* pData=m_pOSG->FindObj( it->first  ,objType);
		if(pData != NULL)
			datas.push_back( pData);
	}
	return datas;
}
std::vector<CDbObjInterface*> CDbBoxSelector::GetSpeSelectObjects(std::vector<CString> selectFliter)
{
	std::vector<CDbObjInterface*> datas;

	std::map<CString, osg::ref_ptr<osg::Node>>::iterator it = selectetSet.begin();
	for (; it != selectetSet.end(); it++)
	{
		for (int i = 0; i < selectFliter.size(); i++)
		{
			CDbObjInterface* pData = m_pOSG->FindObj(it->first, selectFliter[i] );
			if (pData != NULL)
			{
				if (std::find(datas.begin(), datas.end(), pData) ==datas.end() )
					datas.push_back(pData);
			}
		}
	}
	return datas;
}
