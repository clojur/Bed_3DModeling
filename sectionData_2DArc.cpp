#include "stdafx.h"

#include "sectionData_2DArc.h"
 
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
//#include <osg/ValueObject>

#include "osgtools.h"

#include "thmath/TH_Math_API.h"
#include "thmath/Polygon2.h"
#include "THAlgor/THAlgorInc.h"
 #include "sectionData.h"
 
#include "THMathLib/THMathLibShare.h"
#include "THCurveLib/THCurveLibShare.h"
#include "sectionData_2dArc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

C2DArc::C2DArc(CViewControlData* pOSG) :C3DInterface(pOSG)
{
	type = OT_2D_ARC;
	id = COSGDataCenter::GetID();
	strName = "arc";
	color = whiteColor;

	//initialize my data
	m_pTHCurveArc2 = NULL;
	Set(osg::Vec3(0.0, 0.0, 0.0), 100.0, 0.0, PI / 2.0);
}

C2DArc::~C2DArc()
{
	delete m_pTHCurveArc2, m_pTHCurveArc2 = NULL;
}
 

bool C2DArc::Move(osg::Vec3d& offset)
{
	centerPt += offset;

	return true;
}
 

std::vector<osg::Vec3> C2DArc::GetPoints()
{
	std::vector<osg::Vec3> allPT;

	double alfa = 2 * M_PI / 36;

	int nCount = sweepAngle / alfa;
	alfa = sweepAngle / nCount;

	for (int i = 0; i < nCount; i++)
	{
		osg::Vec3 PT1 = centerPt + osg::Vec3(radius*cos(i*alfa), radius*sin(i*alfa), 0);
		allPT.push_back(PT1);
	}
	return allPT;
}



void C2DArc::OnUpdate(osg::Node* pGraph, const CString& showType)
{
 	if (pGraph)
	{
		osg::Geode* pGeode = pGraph->asGeode();
		pGeode->removeDrawables(0, pGeode->getNumDrawables());
		// refactor wr
		/*	pGeode->setUserValue("type", string_tools::CString_to_string(GetRTType()));
			pGeode->setUserValue("showType", string_tools::CString_to_string(showType));

			pGeode->setName( string_tools::CString_to_string(GetName() ));*/

		std::vector<osg::Vec3> allPTs = GetPoints();

		osg::Geometry* pGeomery = createPLineGeode(allPTs, blueColor, TRUE);
		pGeode->addDrawable(pGeomery);
		pGeode->dirtyBound();
	}
}

 
 
C3DInterface* C2DArc::Clone()
{
	C2DArc* pPoint = new C2DArc(m_pOSG);

	pPoint->id = id;
	pPoint->strName = strName;
	pPoint->type = type;
	pPoint->color = color;

	pPoint->Set(centerPt, radius, startAngle, sweepAngle);

	return pPoint;
}

////////////////////////////////////////////yph//////////////////////////////////////////////////////////////

//裁剪Trim
BOOL C2DArc::Trim(double t1, double t2, std::vector< C3DInterface*>& result)
{
	ASSERT(m_pTHCurveArc2 != NULL);
#define PARA_DIFF	1.0e-3		//refer to C2DPLine
	double min_t = 0.0, max_t = 0.0;
	m_pTHCurveArc2->ParaRange(min_t, max_t);

	if (THAlgorTempl::LessEq(t1, min_t, PARA_DIFF) && THAlgorTempl::GreatEq(t2, max_t, PARA_DIFF))
	{
		return FALSE;
	}
	if (THAlgorTempl::Equal(t1, t2, PARA_DIFF))
	{
		return FALSE;
	}

	if (sweepAngle >= 0.0)
	{
		if (t1 >= min_t + PARA_DIFF)
		{
			C2DArc *pNewArc = new C2DArc(m_pOSG);
			result.push_back(pNewArc);
			pNewArc->Set(centerPt, radius, min_t, t1 - min_t);
		}
		if (t2 <= max_t - PARA_DIFF)
		{
			C2DArc *pNewArc = new C2DArc(m_pOSG);
			result.push_back(pNewArc);
			pNewArc->Set(centerPt, radius, t2, max_t - t2);
		}
	}
	else	//sweepAngle < 0.0
	{
		if (t2 <= max_t - PARA_DIFF)
		{
			C2DArc *pNewArc = new C2DArc(m_pOSG);
			result.push_back(pNewArc);
			pNewArc->Set(centerPt, radius, max_t, -(max_t - t2));
		}
		if (t1 >= min_t + PARA_DIFF)
		{
			C2DArc *pNewArc = new C2DArc(m_pOSG);
			result.push_back(pNewArc);
			pNewArc->Set(centerPt, radius, t1, -(t1 - min_t));
		}
	}

	return TRUE;
}

double C2DArc::FirstParameter()
{
	ASSERT(m_pTHCurveArc2 != NULL);
	double t1 = 0.0, t2 = 0.0;
	m_pTHCurveArc2->ParaRange(t1, t2);
	return t1;
}

double C2DArc::LastParameter()
{
	ASSERT(m_pTHCurveArc2 != NULL);
	double t1 = 0.0, t2 = 0.0;
	m_pTHCurveArc2->ParaRange(t1, t2);
	return t2;
}

double C2DArc::GetParameter(const osg::Vec3& p)
{
	ASSERT(m_pTHCurveArc2 != NULL);
	CDblPoint ptCurve; double t = -1.0, length = -1.0;
	m_pTHCurveArc2->ShortestDistance(CDblPoint(p.x(), p.y()), ptCurve, t, length);
	return t;
}

//计算与otherObject的交点
BOOL C2DArc::GetCrossPoints(C3DInterface* pOtherObject, std::vector<osg::Vec3d>& crossPts, BOOL bExtendSelf)
{

	//ASSERT(m_pTHCurveArc2 != NULL);
	//CDblPointArray inter_points;
	//if (pOtherObject->type == OT_2D_PLINE)
	//{
	//	C2DPLine *pPLine = (C2DPLine*)pOtherObject;
	//	CTHCurve curve;
	//	pPLine->Convert2THCurve(curve);
	//	curve.Intersect(*m_pTHCurveArc2, inter_points);
	//}
	//else if (pOtherObject->type == OT_2D_ARC)
	//{
	//	C2DArc *pArc = (C2DArc*)pOtherObject;
	//	m_pTHCurveArc2->Intersect(*pArc->m_pTHCurveArc2, inter_points);
	//}
	//else
	//{

	//}

	//int count = inter_points.GetSize();
	//crossPts.resize(count);
	//for (int i = 0; i < count; i++)
	//{
	//	crossPts[i] = osg::Vec3(inter_points[i].x, inter_points[i].y, 0.0);
	//}

	//return count > 0;

	return FALSE;
}

//返回对象和矩形（p1，P2）的交点pt；
BOOL C2DArc::HitPositin(osg::Vec3& p1, osg::Vec3& p2, osg::Vec3& pt)
{
	ASSERT(m_pTHCurveArc2 != NULL);
	CTHCSLine2 line(_T(""), CDblPoint(p1.x(), p1.y()), CDblPoint(p2.x(), p2.y()));
	CDblPointArray points;
	m_pTHCurveArc2->Intersect(line, points);
	if (points.GetSize() > 0)
	{
		pt = osg::Vec3(points[0].x, points[0].y, 0.0);
		return TRUE;
	}
	return FALSE;
}

void C2DArc::Set(const osg::Vec3 &c, const double r, const double start, const double sweep)
{
	centerPt = c, radius = r, startAngle = CTHMathLib1::normalize_angle(start), sweepAngle = CTHMathLib1::normalize_sweep_angle(sweep);
	bClose = (fabs(fabs(sweepAngle) - 2.0 * PI) < 1.0e-4);

	delete m_pTHCurveArc2;
	m_pTHCurveArc2 = new CTHCSArc2(_T(""), CDblPoint(centerPt.x(), centerPt.y()), radius, startAngle, sweepAngle);
}

////////////////////////////////////////////yph//////////////////////////////////////////////////////////////
