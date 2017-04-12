// MFC_OSG.cpp : implementation of the CDbOSGManager class
//
#include "stdafx.h"

#include "LocalResource.h"

#include "osg/linestipple"
#include "osgInterface.h"
#include "osgtools.h"
#include "osg/linewidth"
#include "osgUtil/SmoothingVisitor"


#include "string_tools.h"

#include "osgObject.h"
#include "OSGDimObj.h"
#include "OsgDimDlg.h"
#include "inplace_tool_window.h"
   
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
 
 



CDbDimObj::CDbDimObj(CViewControlData* pOSG, CDbObjInterface* pParent, const CString& dimName,
	const CString& dimValue, const CString& dimFlag, DB_DIMTYPE dimType)
	:CDbObjInterface(pOSG)
{
	m_type = OT_DIM;
	m_id = CDbOSGDataCenter::GetID();

	if (pParent)
		m_strName.Format("%s@%d", pParent->GetName(), m_id);
	else
		m_strName.Format("%s%d", m_type, m_id);

	m_bReadOnly = FALSE;

 
 	m_strParentObjName = pParent->GetName();
	m_strParentObjType = pParent->GetRTType();
	m_dimType = dimType;
	
	m_dimFlag = dimFlag;
	m_dimName = dimName;
	m_dimValue =dimValue;

	m_color = greenColor;
	m_bAutoScale = TRUE;
	m_bEnChart   = TRUE;

	m_dimTextSize   = 20;
	m_dimArrarSize  = 0.204;
	m_dimLineLength = 0.3;
	m_dimTextOffset = 0.02;
	m_dAngle = 0;

	m_alignment = osgText::Text::CENTER_CENTER;
} 
  
osg::Node* CDbDimObj::CreateGraphObject()
{
	if (m_dimType == CAD_DIM)
	{
		return new osg::Group;
	}
	else
	{
		return new osg::Geode;
	}

}

void CDbDimObj::OnUpdate(osg::Node* pGraph )
{
	if (pGraph)
	{
		pGraph->setName(cdb_string_tools::CString_to_string(GetName()));
		pGraph->setUserValue("type", cdb_string_tools::CString_to_string(GetRTType()));
		pGraph->setUserValue("dimName", cdb_string_tools::CString_to_string(m_dimName));

		if (m_dimType == TEXT_DIM)
		{
			osg::Geode* pGeode = pGraph->asGeode();
			pGeode->removeDrawables(0, pGeode->getNumDrawables());

			CString strText;
			if (m_dimText.IsEmpty())
				m_dimText = m_dimValue;
			strText = m_dimText;

			if (m_dimText.IsEmpty())
				strText = "*";

			osgText::Text* text = NULL;

			osgText::Font* pFont = m_bEnChart ? m_pOSG->GetEnTextFontForColor(m_color) : m_pOSG->GetCnTextFontForColor(m_color);

			if (m_bAutoScale)
			{
 				text = create3DTextNoScale(pFont, m_insertPoint, m_dimTextSize, strText, m_color, m_alignment, m_bEnChart);
			}
			else
			{
				text = create3DText(pFont, m_insertPoint, m_dimTextSize, strText, m_color, m_alignment);
			}
			if (!m_bReadOnly)
			{
				CreateTextEffect(pGeode, m_color, text, m_pOSG);
			}

			text->setDataVariance(osg::Object::DYNAMIC);
			pGeode->addDrawable(text);

		}
		else
		{
			osgText::Font* pFont = m_bEnChart ? m_pOSG->GetEnTextFont() : m_pOSG->GetCnTextFont();

			osg::Group* pGroup = pGraph->asGroup();
			pGroup->removeChildren(0, pGroup->getNumChildren());

			std::vector<osg::Drawable *> allGeometrys;
			if (m_bAutoScale)
			{
				allGeometrys=createLineDim(
					pFont,
					m_dimTextSize,
					m_dimLineLength,
					m_dimArrarSize,
					m_startPoint,
					m_endPoint,
					cdb_string_tools::CString_to_wstring(m_dimText),
					m_color,
					m_alignment);
			}
			else
			{
				allGeometrys = createLineDim3(
					m_dimTextOffset,
					pFont,
					m_dimTextSize,
					m_dimLineLength,
					m_dimArrarSize,
					m_startPoint,
					m_endPoint,
					m_dimText,
					m_color,
					m_alignment);
			}				
	
			////////////////////////////////////
			int nCount = allGeometrys.size();
			if (nCount > 0)
			{
				osg::Geode* pline = new osg::Geode;
				pline->setName("line");
				for (int i = 0; i < nCount - 1; i++)
				{
					pline->addDrawable(allGeometrys[i]);
				}				 
				pGroup->addChild(pline);


				allGeometrys[nCount - 1]->setDataVariance(osg::Object::DYNAMIC);

				osg::Geode* pText = new osg::Geode;		
				{
					pText->setName(cdb_string_tools::CString_to_string(GetName()));
					pText->setUserValue("type", cdb_string_tools::CString_to_string(GetRTType()));
				}
				pText->addDrawable(allGeometrys[nCount - 1]); 
		
				CreateTextEffect(pText, m_color, NULL,m_pOSG);		
				
				osg::MatrixTransform* xTrans = new osg::MatrixTransform;
				xTrans->addChild(pText);
				osg::Vec3 nodeCenter = pText->getBound().center();//中心点

				double alfa = 0;
				{
					if (abs(m_endPoint.x() - m_startPoint.x()) > 0.01)
					{
						alfa = atan((m_endPoint.y() - m_startPoint.y()) / (m_endPoint.x() - m_startPoint.x()));
					}
					else
					{
						if (m_endPoint.y() > m_startPoint.y())
							alfa = M_PI_2;
						else
							alfa = M_PI_2*3;
					}
				}
				xTrans->postMult(
					osg::Matrix::translate(-nodeCenter)* 
					osg::Matrix::rotate(osg::Quat(alfa, osg::Vec3(0, 0, 1.0))) *
					osg::Matrix::translate(nodeCenter));
				pGroup->addChild(xTrans);	
			}		
		}
 	}
}

CDbObjInterface* CDbDimObj::Clone()
{
	CDbDimObj* pClone = new CDbDimObj(m_pOSG, GetParentObj(), m_dimName, m_dimValue,m_dimFlag,m_dimType);
 
	pClone->m_dimType    =  m_dimType;

	pClone-> m_dimName  =  m_dimName;
	pClone-> m_dimValue =  m_dimValue;

	pClone-> m_insertPoint   =  m_insertPoint;
	pClone-> m_dimTextSize   =  m_dimTextSize;
	pClone-> m_startPoint    =  m_startPoint;
	pClone-> m_endPoint      =  m_endPoint;
	pClone-> m_dimArrarSize  =  m_dimArrarSize;
	pClone-> m_dimLineLength =  m_dimLineLength;

	return pClone;
}


BOOL CDbDimObj::OnDblClick()
{
	CDbObjInterface* m_pParentObj = GetParentObj( );

	if (m_pParentObj&& !m_bReadOnly)
	{
		if (1)
		{
			CString result = m_dimValue;
			if (m_dimLable.size()> 0)
			{
				//refactor by wr
				if (inplace_tool_window::inplace_combobox(result,m_dimLable))
				//{
					m_dimValue = result;
					return  m_pParentObj->OnUpdateByDim(m_dimFlag, m_dimName, result);
				//}
 			}	 
			else
			{
				//refactor by wr
				if (inplace_tool_window::inplace_textbox(result))
				//{
					m_dimValue = result;
					return  m_pParentObj->OnUpdateByDim(m_dimFlag, m_dimName, result);
			//	}
			}
		}
		else
		{
			CLocalResource temp;

			CDbOSGManagerDimDlg dlg;
			dlg.m_strDimName = m_dimName;
			dlg.m_strDimValue = m_dimValue;
			dlg.m_vecLable = m_dimLable;
			POINT point;
			GetCursorPos(&point);

			dlg.m_xPos = point.x;
			dlg.m_yPOs = point.y;

			if (dlg.DoModal() == IDOK)
			{
				return  m_pParentObj->OnUpdateByDim(m_dimFlag, dlg.m_strDimName, dlg.m_strDimValue);
			}
		}
	}
	return FALSE;
}
 

CDbObjInterface* CDbDimObj::GetParentObj()
{
	if (m_pOSG)
	{
		return m_pOSG->FindObj(m_strParentObjName, m_strParentObjType);
	}
	return NULL;
}
//refacor by wr
//void  CDbDimObj::create_property_item(CXTPPropertyGridView& pGridItem)
//{
//	CXTPPropertyGridItem* pBaseInfo = pGridItem.AddCategory("标注信息");
//	if (pBaseInfo)
//	{
//		pBaseInfo->Expand();
//
//		CXTPPropertyGridItem*pChildGridItem = pBaseInfo->AddChildItem(new CXTPPropertyGridItem("标注标识"));
//		pChildGridItem->SetValue(m_dimFlag);
//		pChildGridItem->SetReadOnly(TRUE);
// 
//		pChildGridItem = pBaseInfo->AddChildItem(new CXTPPropertyGridItem("标注名称"));
//		pChildGridItem->SetValue(m_dimName);
//		pChildGridItem->SetReadOnly(TRUE);
//	}
//}
