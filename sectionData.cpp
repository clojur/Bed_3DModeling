#include "stdafx.h"
 
#include "osg/Node"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/linewidth"
#include "osg/PolygonMode"
#include "osg/Material"
#include "osg/LineStipple"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osg/LightModel>
#include <osg/Point>
#include "osg/BlendFunc"
#include <osgutil/Tessellator>
#include "osgutil/SmoothingVisitor"
#include <osgUtil/DelaunayTriangulator>
 
#include "osgtools.h"

#include "thmath/TH_Math_API.h"
#include "thmath/Polygon2.h"
#include "THAlgor/THAlgorInc.h"
 #include "sectionData.h"

#include "THMathLib/THMathLibShare.h"
#include "THCurveLib/THCurveLibShare.h"
 
  
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




C2DPoint::C2DPoint(CViewControlData* pOSG) :C3DInterface(pOSG)
{
	type =OT_POINT;
	id= COSGDataCenter::GetID();
 	strName.Format("%s%d", type, id);
	color = redColor;
  	strTitle.Format("%s%d",type, id);


  }

C2DPoint::C2DPoint(double dx, double dy, CViewControlData* pOSG) :C3DInterface(pOSG) 
{
	type =OT_POINT;
	id= COSGDataCenter::GetID();
	strName.Format("%s%d", type, id);
 
 
	strTitle.Format("%s%d",type, id);

	color = redColor;
 

 	point_data.set(dx, dy,0);
	// refactor wr
	//strXPt = string_tools::num_to_string(dx);
	//strYPt = string_tools::num_to_string(dy);

	//pGridItem = NULL;
 
}

C2DPoint::C2DPoint(double dx, double dy, double dr, CViewControlData* pOSG) :C3DInterface(pOSG), point_data(dx, dy, dr)
{
	type = OT_POINT;
	id = COSGDataCenter::GetID();
	strName.Format("%s%d", type, id);
	strTitle.Format("%s%d", type, id);
	color = redColor;
	// refactor wr
 //	pGridItem = NULL;
	//strXPt = string_tools::num_to_string(dx);
	//strYPt = string_tools::num_to_string(dy);

 
}

 

std::vector<osg::Vec3d> C2DPoint::GetStretchPoint()
{
	std::vector<osg::Vec3d> allPT;
	allPT.push_back(point_data);
	return allPT;
}
 
BOOL C2DPoint::Mirror(osg::Matrix* mirrorMatrix)
{
	if(mirrorMatrix)
	{
		osg::Vec3 pt =point_data*(*mirrorMatrix);
		point_data = pt;
		// refactor wr
		//strXPt = string_tools::num_to_string(point_data.x());
		//strYPt = string_tools::num_to_string(point_data.y());

		return TRUE;
	}
	return FALSE;
}

bool C2DPoint::Move( osg::Vec3d& offset )
{
	point_data += offset;
	// refactor wr
	//strXPt = string_tools::num_to_string(point_data.x());
	//strYPt = string_tools::num_to_string(point_data.y());
	return true;
}
bool C2DPoint::Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset)
{
	for(int i=0; i< stretchIndex.size(); i++ )
	{
		if( stretchIndex[i] ==0 )
		{
			point_data = oldPositions[0] +  offset;
			// refactor wr
	/*		strXPt = string_tools::num_to_string(point_data.x());
			strYPt = string_tools::num_to_string(point_data.y());*/
		}
	}
	return true;
}
void  C2DPoint::SetPoint(CDblPoint& pt)
{
	point_data[0] = pt.x;
	point_data[1] = pt.y;
	// refactor wr
	//strXPt = string_tools::num_to_string(point_data.x());
	//strYPt = string_tools::num_to_string(point_data.y());

}
void  C2DPoint::SetPoint(double x, double y, double z )
{
	point_data[0] = x;
	point_data[1] = y;
	point_data[2] = z;
	// refactor wr
// 	strXPt = string_tools::num_to_string(point_data.x());
// 	strYPt = string_tools::num_to_string(point_data.y());

}

CDblPoint C2DPoint::GetPoint()
{
	return CDblPoint(point_data.x(), point_data.y());
}

void C2DPoint::OnUpdate(osg::Node*  pGraph, const CString& showType)
{
 	if(pGraph)
	{
		osg::Geode* pGeode = pGraph->asGeode();
		if(pGeode)
		{
			// refactor wr
		/*	pGeode->setName( string_tools::CString_to_string(GetName())  );
			pGeode->setUserValue("type", string_tools::CString_to_string(GetRTType()));
			pGeode->setUserValue("showType", string_tools::CString_to_string(showType));*/

			double size = m_pOSG->GetObjSize(OT_POINT);

			CreateNode3DModel3(0, m_pOSG->GetPointTexture(0), point_data, pGeode, redColor, size);
		}
	}
}

//void  C2DPoint::create_property_item(CXTPPropertyGridView& pGridItem)
//{ 
// }
//
//bool C2DPoint::applyGridChanged(std::vector<CXTPPropertyGridItem*>& allModifyItems)
//{
//	return true;
//} 

C3DInterface* C2DPoint::Clone()
{
	C2DPoint* pPoint = new C2DPoint(m_pOSG);
	pPoint->id = id;
	pPoint->strName=strName;
	pPoint->type=type;
	pPoint->color=color;
	pPoint->point_data = point_data; 
	pPoint->strXPt = strXPt;
	pPoint->strYPt = strYPt;
 	return pPoint;
}



C2DLine::C2DLine(CViewControlData* pOSG) :C3DInterface(pOSG)
{
	type = OT_2D_LINE;
	id = COSGDataCenter::GetID();
	strName.Format("%s%d", type, id);
	strTitle = strName;
	color = whiteColor;
}

std::vector<osg::Vec3d> C2DLine::GetStretchPoint()
{
	std::vector<osg::Vec3d> allPT;
	allPT.push_back(osg::Vec3d(line_data.start.x, line_data.start.y, 0));
	allPT.push_back(osg::Vec3d(line_data.end.x, line_data.end.y, 0));

	return allPT;
}
BOOL C2DLine::Mirror(osg::Matrix*)
{
	return FALSE;
}

bool C2DLine::Move(osg::Vec3d& offset)
{
	line_data.start.x += offset.x();
	line_data.start.y += offset.y();
	line_data.end.x += offset.x();
	line_data.end.y += offset.y();

	return true;
}
bool C2DLine::Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset)
{
	for (int i = 0; i < stretchIndex.size(); i++)
	{
		int index = stretchIndex[i];

		if (index == 0)
		{
			line_data.start.x = oldPositions[index].x() +  offset.x();
			line_data.start.y = oldPositions[index].x() +  offset.y();
		}
		else if (index == 1)
		{
			line_data.end.x =oldPositions[index].x() + offset.x();
			line_data.end.y =oldPositions[index].x() + offset.y();
		}
	}
	return true;
}

 
void C2DLine::OnUpdate(osg::Node*  pGraph, const CString& showType)
{
 	if (pGraph)
	{
		osg::Geode* pGeode = pGraph->asGeode();
		pGeode->removeDrawables(0, pGeode->getNumDrawables());

		// refactor wr
		//pGeode->setUserValue("type", string_tools::CString_to_string(GetRTType()));
		//pGeode->setUserValue("showType", string_tools::CString_to_string(showType));

		osg::Geometry* pGeomery = createLineGeode(osg::Vec3(line_data.start.x, line_data.start.y, 0), osg::Vec3(line_data.end.x, line_data.end.y, 0), GetColor() ) ;
		pGeode->setName(GetName());
		pGeode->addDrawable(pGeomery);

		pGeode->dirtyBound();
	}
}

 


C3DInterface* C2DLine::Clone()
{
	C2DLine* pPoint = new C2DLine(m_pOSG);
	pPoint->id = id;
	pPoint->strName = strName;
	pPoint->type = type;
	pPoint->color = color;
	pPoint->line_data = line_data;

	return pPoint;
}