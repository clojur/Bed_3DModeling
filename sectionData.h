#pragma  once

#include "thmath/baseTypes.h"
#include "thmath/Polyline2.h"
#include "thmath/Arc2.h"

#include "osgObject.h"
 
//这里的代码是历史版本
class DB_3DMODELING_API C2DPoint:public C3DInterface 
{
public:
	C2DPoint(CViewControlData*pOSG);
	C2DPoint(double dx, double dy, CViewControlData*pOSG = NULL);
	C2DPoint(double dx, double dy, double dr, CViewControlData*pOSG = NULL);
 
	virtual std::vector<osg::Vec3d> GetStretchPoint();
	bool Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset);
 	virtual void OnUpdate(osg::Node*  pGraph,const CString& showType);
 
	virtual C3DInterface* Clone();
	virtual bool Move( osg::Vec3d& offset );
	virtual BOOL OnDblClick(){return FALSE;}
	virtual BOOL Mirror(osg::Matrix*);
	CDblPoint GetPoint();
	void  SetPoint(CDblPoint& pt);
	void  SetPoint(double x, double y, double z = 0);

	//BOOL      bEmpty;


	osg::Vec3 point_data;
	osg::Vec3 bakpoint_data;          //临时数据


 	CString strX;
	CString strY;

	CString strXPt;
	CString strYPt;
	// refactor wr
	//CXTPPropertyGridItem* pGridItem;
	//virtual void create_property_item(CXTPPropertyGridView& grid);
	//virtual  bool applyGridChanged(std::vector<CXTPPropertyGridItem*>& allModifyItems);
};

class DB_3DMODELING_API C2DLine :public C3DInterface
{
public:
	C2DLine(CViewControlData*pOSG);
	virtual std::vector<osg::Vec3d> GetStretchPoint();
	bool Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset);
 	virtual void OnUpdate(osg::Node*  pGraph,const CString& showType);
 	virtual C3DInterface* Clone();
	virtual bool Move(osg::Vec3d& offset);
	virtual BOOL OnDblClick(){return FALSE;}
	virtual BOOL Mirror(osg::Matrix*);
	CLine2 line_data;
};

