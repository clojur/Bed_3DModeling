// MFC_OSG.cpp : implementation of the CDbOSGManager class
//
#include "stdafx.h"

#include "osg/linestipple"
#include "osg/linewidth"
#include "osgUtil/SmoothingVisitor"
#include "osgInterface.h"
#include "OSGBaseView.h"
#include "osgtools.h"
#include "osgObject.h"
#include "string_tools.h"
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD CDbOSGDataCenter::maxID=1;


CDbObjInterface::~CDbObjInterface()
{
	std::map<CString, CDb_PrimitiveInfo*>::iterator it = m_meshInfo.begin();
	for (; it != m_meshInfo.end(); it++)
	{
		delete it->second;
	}
	m_meshInfo.clear();
}
CDbObjInterface::CDbObjInterface()
{
	m_pParentObj = NULL;//zmz
	m_pOSG = NULL;
	m_bSelected = FALSE;
	m_bDrawHandler = TRUE;
	m_bClose = FALSE;
	m_type = OT_UNKNOWN;
	m_visible = true;
}

CDbObjInterface::CDbObjInterface(CViewControlData* pOSG)
{
	m_pParentObj = NULL;//zmz
	m_pOSG = pOSG;
	m_bSelected = FALSE;
	m_bDrawHandler = TRUE;
	m_bClose = FALSE;
	m_type = OT_UNKNOWN;
	m_visible = true;
 }


void GetSubDrawable(osg::Node* pNode, std::vector<osg::Drawable*>& drawables)
{
	osg::Group* pGroup = pNode->asGroup();
	if (pGroup)
	{
		int nChild = pGroup->getNumChildren();
		for (int i = 0; i < nChild; i++)
		{
			osg::Node* pChild = pGroup->getChild(i);
			GetSubDrawable(pChild, drawables);
		}
	}
	else
	{
		osg::Drawable* pDrawable = pNode->asDrawable();
		if (pDrawable)
		{
			drawables.push_back(pDrawable);		
		}
		else
		{
			return;
		}
	}
}
osg::Node* CDbObjInterface::AddSelectEffect(osg::Node* pSelf, BOOL bSelect)
{
	osg::Geode* pNewGeode = new osg::Geode;
	std::vector<osg::Drawable*> allSubItem;
	GetSubDrawable(pSelf, allSubItem);
	for (int i = 0; i < allSubItem.size(); i++)
	{
		osg::Drawable* pDrawable = allSubItem[i];
		osgText::Text* pText = dynamic_cast<osgText::Text*>(pDrawable);
		if (!pText)
		{
			pNewGeode->addDrawable(osg::clone(pDrawable, osg::CopyOp(osg::CopyOp::DEEP_COPY_ALL)));
		}
	}
	return pNewGeode;
}

void CDbObjInterface::ClearSelectEffect(osg::Node* pSelf, BOOL bSelect)
{
	return;
}

bool CDbObjInterface::Array(osg::Vec3d& offset)              //array操作， 默认为执行copy,构件的copy操作可能不同
{
	return Move(offset);
}

bool CDbObjInterface::Copy(osg::Vec3d& offset)             //copy操作， 默认为执行Move
{
	return Move(offset);
}
bool CDbObjInterface::offset(double dOffset, bool bTemp)
{
	return false;
}


std::vector<osg::Vec3d> CDbObjInterface::GetStretchPoint()
{
	std::vector<osg::Vec3d> temp;
	return temp;
}

int CDbObjInterface::GetIndex(osg::Vec3& resPT)
{
	int index = 0;
	double dMin = 1.0e10;

	 std::vector<osg::Vec3d> pts =  GetStretchPoint();
	 for(int i=0; i< pts.size(); i++)
	 {
		osg::Vec3d pt =pts[i];

		double dist = ( pt.x()-  resPT.x() )*( pt.x()-  resPT.x() )+ ( pt.y()-  resPT.y())*( pt.y()-  resPT.y() );
		if(dist < dMin)
		{
			dMin = dist;
			index = i;
		}
	 }
	 return index+1;
}


std::vector<osg::Node*> CDbObjInterface::GetChildNodes()
{
	std::vector<osg::Node*> childNodes;

	std::vector<CDbObjInterface*> childObjects = GetSubObjList(false);
	for (int i = 0; i < childObjects.size(); i++)
	{
		osg::Node * pParent = m_pOSG->GetDataParent(childObjects[i]);
 		if (pParent)
		{
			std::vector<osg::Node*> nodeVec;
			GetChildByName2(nodeVec, childObjects[i]->GetName(), pParent->asGroup(), GetRTType());
		 
			for (int j = 0; j < nodeVec.size(); j++)
			{
				if ( std::find( childNodes.begin(),childNodes.end(), nodeVec[j] ) == childNodes.end() )
					childNodes.push_back(nodeVec[j]);
			}

 		}
	}
 
	return childNodes;
}

std::vector<osg::Node*> CDbObjInterface::GetDimNodes()
{
	std::vector<osg::Node*> dimVec;
	osg::Node * pParent = GetChildByName( GetName(), m_pOSG->dimObjs);
	if (pParent)
	{
		GetChildDimByName(dimVec, GetName() + "@", pParent->asGroup() );
	}
	return dimVec;
}


void CDbObjInterface::DeleteObjectNode()
{
	if (m_pOSG)
	{
		//1.删除图形包围框
		m_pOSG->clearBoundingBoxObj(GetName());


		//3.删除图形对象
		std::vector<osg::Node*> nodeVec;
		GetChildByName2(nodeVec, GetName(), m_pOSG->GraphicsGroup, GetRTType() );
		for (int j = 0; j < nodeVec.size(); j++)
		{
			m_pOSG->delObject(nodeVec[j]);
		}

		std::vector<osg::Node*> childObject = GetChildNodes();
		for (int j = 0; j < childObject.size(); j++)
		{
			m_pOSG->delObject(childObject[j]);
		}

		std::vector<osg::Node*> dimVec = GetDimNodes();
 		for (int j = 0; j < dimVec.size(); j++)
		{
			m_pOSG->delObject(dimVec[j]);
		}
	}
}


 
void CDbObjInterface::setUserValue(const CString &strKey, const CString &strValue)
{
	m_mapUserValue[strKey] = strValue;
}

void CDbObjInterface::getUserValue(const CString &strKey, CString &strValue) const
{
	std::map<CString, CString>::const_iterator iter_find = m_mapUserValue.find(strKey);
	if (iter_find != m_mapUserValue.end())
	{
		strValue = iter_find->second;
	}
}

std::vector<CDbObjInterface*> CDbObjInterface::CreateDimObjects()
{
	std::vector<CDbObjInterface*> emptyDims;
	return emptyDims;
}

BOOL CDbObjInterface::OnUpdateByDim(const CString& strDimFlag, const CString& strDimName, const CString& strDimValue)
{
	return FALSE;
}

bool CDbObjInterface::OnUpdateByChild(CDbObjInterface* pChild)
{
	return false;
}

std::vector<CDbObjInterface*> CDbObjInterface::CreateSubObjList()
{
	std::vector<CDbObjInterface*> objs;
	return objs;
}

std::vector<CDbObjInterface*> CDbObjInterface::GetSubObjList(BOOL bIncludeSefl)
{
	std::vector<CDbObjInterface*>  datas;
	if (bIncludeSefl)
		datas.push_back(this);
	return datas;

} 
 

void CDbObjInterface::UpdateDBData(db_changeMode changeMode)
{
	if (m_pParentObj)
	{
		m_pParentObj->OnUpdateByChild(this);
		m_pParentObj->UpdateDBData(changeMode);
	}
	else
	{
		/*
		...........dataexchange;
		*/
	}
 
}
void CDbObjInterface::SetDimVisible(const CString& strDimType, BOOL bVisible)
{
	std::vector<osg::Node*> dimVec = GetDimNodes();
	for (int i = 0; i < dimVec.size(); i++)
	{
		if (dimVec[i])
		{
			std::string dimName("");
			dimVec[i]->getUserValue("dimName", dimName);
			if (dimName.c_str() == strDimType || strDimType.IsEmpty())
			{
				if (bVisible)
					dimVec[i]->setNodeMask(0xffffffff);
				else
					dimVec[i]->setNodeMask(0x0);
			}		
		}
 	}
}

void CDbObjInterface::AddDimNodes()
{
	std::vector<CDbObjInterface*> subDims = CreateDimObjects();
	for (int i = 0; i < subDims.size(); i++)
	{
		CDbObjInterface* pDim = subDims[i];
		m_pOSG->m_pUserData->AddObj(pDim); 
		pDim->UpdateAndAddToScene();
	}
}

void CDbObjInterface::RemoVeDimNodes()
{
	std::vector<osg::Node*> dimVec = GetDimNodes();
	for (int j = 0; j < dimVec.size(); j++)
	{
		m_pOSG->m_pUserData->DeleteObj(dimVec[j]->getName().c_str(), OT_DIM);

		//1.删除图形包围框
		m_pOSG->clearBoundingBoxObj(dimVec[j]->getName().c_str());

		m_pOSG->delObject(dimVec[j]);
	}
}

void CDbObjInterface::DoUpdate()
{
	if (m_pOSG)
	{
		m_pOSG->clearBoundingBoxObj(GetName());
	}

	std::vector<osg::Node*> nodeVec;
	GetChildByName2(nodeVec, GetName(), m_pOSG->GraphicsGroup, GetRTType());
	for (int j = 0; j < nodeVec.size(); j++)
	{
		osg::Node* pNode = nodeVec[j]; 
		if(m_pOSG)
			m_pOSG->addUpdateObject(this , pNode);
	}
	if (m_pOSG)
	{
		m_pOSG->addUpdateDim(this);
	}


	//zmz
	if (m_pParentObj)
	{
		m_pParentObj->DoUpdate();
	}

 }
 
osg::Node* CDbObjInterface::CreateGraphObject()
{
	osg::ref_ptr<osg::Node> pNode = new osg::Geode;
	return pNode.release();
}

void CDbObjInterface::UpdateAndAddToScene()
{
	osg::Node* pNode = CreateGraphObject();
	if (!GetVisible())
		pNode->setNodeMask(0x0);

	OnUpdate(pNode);
	if (m_pOSG)
	{
		m_pOSG->addObject(this, pNode);
	}

	//add 陈刚 2017-3-29
	std::vector<CDbObjInterface*> subObjects = CreateSubObjList();
	for (int i = 0; i < subObjects.size(); i++)
	{
		CDbObjInterface* pSub = subObjects[i];
		m_pOSG->m_pUserData->AddObj(pSub);
		pSub->UpdateAndAddToScene();
	}


	//add 陈刚 2015-11-9
	std::vector<CDbObjInterface*> subDims = CreateDimObjects();
	for (int i = 0; i < subDims.size(); i++)
	{
		CDbObjInterface* pDim = subDims[i];
		m_pOSG->m_pUserData->AddObj(pDim);
		pDim->UpdateAndAddToScene();		
	}
} 

osg::Vec4 RGBToVec4(COLORREF color)
{
	return osg::Vec4(1.0*GetRValue(color) / 255.0, 1.0*GetGValue(color) / 255.0, 1.0*GetBValue(color) / 255.0, 1.0);

}
void CDbObjInterface::OnUpdate(osg::Node*  pNode)
{
	osg::Geode* pGeode = pNode->asGeode();
	if (pGeode)
	{		
		pGeode->setName(cdb_string_tools::CString_to_string(GetName()));
		pGeode->setUserValue("type", cdb_string_tools::CString_to_string(GetRTType()));

		std::map<CString, CDb_PrimitiveInfo*>::iterator it = m_meshInfo.begin();
		for (; it != m_meshInfo.end(); it++)
		{
			CDb_PrimitiveInfo* pPrimitive = it->second;

			CDb_MeshInfo* pMesh = dynamic_cast<CDb_MeshInfo*>(pPrimitive);
			if (pMesh)
			{
				osg::Geometry* pGeometry = new osg::Geometry;

				osg::Vec4Array* colors = new osg::Vec4Array;
				colors->push_back(RGBToVec4(pMesh->color));
				pGeometry->setColorArray(colors);
				pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

				osg::Vec3Array *pVertices = new osg::Vec3Array;
				for (CDb_Point3& pt : pMesh->m_Ptsvec)
				{
					pVertices->push_back(osg::Vec3(pt.x, pt.y, pt.z));
				}
				pGeometry->setVertexArray(pVertices);

				osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
				for (int i : pMesh->m_Indexsvec)
				{
					if (i == -1)
					{
						continue;
					}
					else
					{
						indices->push_back(i);
					}
				}
				pGeometry->addPrimitiveSet(indices.get());

				//pGeometry->setColorArray();
				osgUtil::SmoothingVisitor::smooth(*pGeometry, M_PI_2);

				pGeode->addDrawable(pGeometry);
			}
			else
			{
				CDb_LineInfo* pLine = dynamic_cast<CDb_LineInfo*>(pPrimitive);
				if (pLine)
				{				
					int nCount = pLine->m_Ptsvec.size();

					osg::ref_ptr<osg::Geometry> pGeometry = new osg::Geometry();

					osg::Vec3Array *pVertices = new osg::Vec3Array;
					for (CDb_Point3& pt : pLine->m_Ptsvec)
					{
						pVertices->push_back(osg::Vec3(pt.x, pt.y, pt.z));
					}
					pGeometry->setVertexArray(pVertices);


					osg::Vec4Array* colors = new osg::Vec4Array;
					colors->push_back(RGBToVec4(pLine->color));
					pGeometry->setColorArray(colors);
					pGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

					osg::ref_ptr<osg::DrawArrays> pyramidBase = NULL;
					if (pLine->meshType == mt_lines)
					{
						pyramidBase = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, nCount);
					}
					else if (pLine->meshType == mt_line_strip)
					{
						pyramidBase = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, nCount);
					}
					else
					{
						pyramidBase = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, nCount);
					}
					pGeometry->addPrimitiveSet(pyramidBase.get()); 
					{
						osg::StateSet* stateset = pGeometry->getOrCreateStateSet();
						osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
						linewidth->setWidth(pLine->lineWidth);
						stateset->setAttributeAndModes(linewidth.get(), osg::StateAttribute::ON);
						stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
					} 

					pGeode->addDrawable(pGeometry.get() );
				}
			}		
 		}
	}
}


void CDbObjInterface::AddMeshInfo(CDb_PrimitiveInfo* pOneMesh)
{
	if (pOneMesh)
	{
		m_meshInfo[pOneMesh->strName] = pOneMesh;
	}
}

void CDbOSGDataCenter::DeleteObjByID(DWORD id)
{
	m_dataMutex.Lock();

	std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
	for (; it != dataMap.end(); it++)
	{
		std::vector<CDbObjInterface*>::iterator it2 = it->second.begin();
		for (; it2 != it->second.end(); it2++)
		{
			if ((*it2)->GetID() == id)
			{
				delete (*it2);
				it->second.erase(it2);
				m_dataMutex.Unlock();
				return;
			}
		}
	}
	m_dataMutex.Unlock();
}

void CDbOSGDataCenter::DeleteObj(const CString& objName, const CString& type )
{
	m_dataMutex.Lock();

	if(type != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(type  );
		if( it != dataMap.end() )
		{
			std::vector<CDbObjInterface*>::iterator it2 = it->second.begin();
			for( ; it2 != it->second.end() ; it2++)
			{
				if ((*it2)->GetName() == objName)
				{
					delete (*it2);
					it->second.erase(it2);
					m_dataMutex.Unlock();
					return;
				}
			}
		}
	}
	else
	{
 		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end()   ; it++)
		{
			std::vector<CDbObjInterface*>::iterator it2 = it->second.begin();
			for( ; it2 != it->second.end()    ; it2++)
			{
				if ((*it2)->GetName() == objName)
				{
					delete (*it2);
					it->second.erase(it2);

					m_dataMutex.Unlock();

					return;
 				}
			}
		}
	}
	m_dataMutex.Unlock();
}



void CDbOSGDataCenter::AddObj(CDbObjInterface* pData )
{
	m_dataMutex.Lock();
	dataMap[pData->GetRTType()].push_back(pData);
	m_dataMutex.Unlock();
}

void CDbOSGDataCenter::Clear()
{
	m_dataMutex.Lock();

	std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
	for(;  it != dataMap.end() ; it++)
	{
		for(int i=0; i< it->second.size() ; i++)
		{
			delete it->second[i];		
		}
	}
	dataMap.clear();

	m_dataMutex.Unlock();
}



std::vector<CDbObjInterface*> CDbOSGDataCenter::GetObjectByTitle(const CString& strTitle)
{
	m_dataMutex.Lock();

	std::vector<CDbObjInterface*> findObjcts;

	std::map<CString, std::vector<CDbObjInterface*> >::iterator  it = dataMap.begin();
	for (; it != dataMap.end(); it++)
	{
		for (int j = 0; j < it->second.size(); j++)
		{
			CDbObjInterface* pCurData = it->second[j];
			if (pCurData->GetTitle() == strTitle)
			{
				findObjcts.push_back(pCurData);
			}
		}
	}
	m_dataMutex.Unlock();

	return findObjcts;
}
CDbObjInterface* CDbOSGDataCenter::FindObjByID(const DWORD& objName, const CString& type)
{
	m_dataMutex.Lock();

	if (type != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(type);
		if (it != dataMap.end())
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetID() == objName)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}
			}
		}
	}
	else
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetID() == objName)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}
			}
		}
	}
	m_dataMutex.Unlock();
	return NULL;
}

CDbObjInterface* CDbOSGDataCenter::FindObj(const CString& objName, const CString& type)
{
	m_dataMutex.Lock();
	if (type != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(type);
		if (it != dataMap.end())
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetName() == objName)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}
			}
		}
	}
	else
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetName() == objName)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}
			}
		}
	}
	m_dataMutex.Unlock();
	return NULL;
}


std::vector<CDbObjInterface*> CDbOSGDataCenter::FindObjByTypes(std::vector<CString>& objTypes)
{
	m_dataMutex.Lock();

	std::vector<CDbObjInterface*> objs;
	for (int i = 0; i < objTypes.size(); i++)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(objTypes[i]);
		if (it != dataMap.end())
		{
			objs.insert(objs.end(), it->second.begin(), it->second.end());
		}
	}
	m_dataMutex.Unlock();

	return objs;
}

std::vector<CString>            CDbOSGDataCenter::GetObjTitlesByType(const CString& objType)
{
	std::vector<CString> titles;

	m_dataMutex.Lock();
	if (objType != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(objType);
		if (it != dataMap.end())
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				titles.push_back(it->second[i]->GetTitle());
			}
 		}
	}
	else
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				titles.push_back(it->second[i]->GetTitle());
			}
 		}
	}
	m_dataMutex.Unlock();
	return titles;
}

std::vector<CDbObjInterface*> CDbOSGDataCenter::FindObjByType(const CString& type)
{
	m_dataMutex.Lock();
	std::vector<CDbObjInterface*> objs;
	if (type != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(type);
		if (it != dataMap.end())
		{
			objs.insert(objs.end(), it->second.begin(), it->second.end());
		}
	}
	else
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end(); it++)
		{
			objs.insert(objs.end(), it->second.begin(), it->second.end());
		}
	}
	m_dataMutex.Unlock();
	return objs;
}


CDbObjInterface* CDbOSGDataCenter::FindObjByTitle(const CString& strTitle, const CString& type)
{
	m_dataMutex.Lock();
	if (type != OT_ALL)
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.find(type);
		if (it != dataMap.end())
		{
			for (int i = 0; i < it->second.size(); i++)
			{				
				if (it->second[i]->GetTitle() == strTitle)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}				
			}
		}
	}
	else
	{
		std::map<CString, std::vector<CDbObjInterface*> >::iterator it = dataMap.begin();
		for (; it != dataMap.end(); it++)
		{
			for (int i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetTitle() == strTitle)
				{
					m_dataMutex.Unlock();
					return it->second[i];
				}
			}
		}
	}
	m_dataMutex.Unlock();
	return NULL;
}