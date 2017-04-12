#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
对象copy命令
*/

class DB_3DMODELING_API CDbDistHandler:public CDbHandlerInterface
{
public:
	CDbDistHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;	 
		strName ="mirror";
		pCamera=NULL;
		pGeometry=NULL;
	}
	osg::Vec3 p2, p1;
	osg::Vec2 p11, p22 ;

	osg::Node* pCamera; //橡皮将对象
	osg::Geometry* pGeometry ;//橡皮几何数据


	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa); 
};