#pragma  once

#include "thmath/baseTypes.h"
#include "thmath/Polyline2.h"
#include "thmath/Arc2.h"

#include "osgObject.h"

class CDb4xBeam;
class CDBMObject;
class CDb4xBridgeTowerType;


class CTHCSArc2;
class DB_3DMODELING_API  C2DArc :public C3DInterface
{
public:
	C2DArc(CViewControlData*pOSG);
	~C2DArc();
 	std::vector<osg::Vec3> GetPoints();
 	virtual void OnUpdate(osg::Node* pGraph, const CString& showType);
 	virtual C3DInterface* Clone();
	virtual bool Move(osg::Vec3d& offset);
	virtual BOOL OnDblClick(){ return FALSE; }

	//extend
	virtual BOOL Extend(double t1, C3DInterface* pBorder, BOOL bExtendStat){ return FALSE; }
	//
	//裁剪Trim
	virtual BOOL Trim(double t1, double t2, std::vector< C3DInterface*>& result);

	virtual double FirstParameter();

	virtual double LastParameter();

	virtual double GetParameter(const osg::Vec3& p);

	//计算与otherObject的交点
	virtual BOOL GetCrossPoints(C3DInterface* pOtherObject, std::vector<osg::Vec3d>& crossPts, BOOL bExtendSelf = FALSE);

	//返回对象和矩形（p1，P2）的交点pt；
	virtual BOOL HitPositin(osg::Vec3& p1, osg::Vec3& p2, osg::Vec3& pt);

private:
	double    radius; //半径
	osg::Vec3 centerPt; //圆心
	double startAngle;
	double sweepAngle;

public:
	void Set(const osg::Vec3 &c, const double r, const double start, const double sweep);

private:
	CTHCSArc2 *m_pTHCurveArc2;
};
