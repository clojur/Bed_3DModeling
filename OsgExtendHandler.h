#pragma  once

#include "DbHandlerInterface.h"

#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"


class DB_3DMODELING_API CDbExtendHandler:public CDbHandlerInterface
{
public:
	CDbExtendHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;
		bTrim=TRUE;
		strName ="trim";
	}
	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void Action( BOOL bTrim);

	void extentObjects( );
	void trimObjects( );
	//计算对象pObject与边界m_Borders的交点
	void  GetBorderCrossPoints_Extend( CDbObjInterface* pObject, std::vector<double> &leftCrossPts,std::vector<double> &rightCrossPt);
	void  GetBorderCrossPoints_Trim(     CDbObjInterface* pObject, std::vector<double> &CrossPtParam);

	std::vector< CDbObjInterface* > borderObjects;  //边界对象
	std::vector< CDbObjInterface* > curOperatorObjects; //操作对象

	osg::Vec3 curPT1,curPT2;
	BOOL bTrim;
};