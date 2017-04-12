#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
����copy����
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

	osg::Node* pCamera; //��Ƥ������
	osg::Geometry* pGeometry ;//��Ƥ��������


	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa); 
};