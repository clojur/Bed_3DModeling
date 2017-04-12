#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

class DB_3DMODELING_API CDbMoveObjhandler:public CDbHandlerInterface
{
public:
	CDbMoveObjhandler(CViewControlData* pOSG);

	void finish();
	void begin();
	void action(bool bCancle);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void AppalyBasePT(osg::Vec3 inputPt);

	osg::Vec3d p11; //基点
	osg::Vec3d p22; //第二个点
 
	osg::Vec2 startPT; //起点，窗口坐标
	osg::Vec2 endPT;//橡皮筋终点，窗口坐标

	osg::Node* pCamera; //橡皮将对象
	osg::Geometry* pGeometry ;//橡皮几何数据

	osg::ref_ptr<osg::MatrixTransform> pParentNode;
 
	bool bQuickModel;
};


