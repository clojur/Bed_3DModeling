#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
∂‘œÛcopy√¸¡Ó
*/

class DB_3DMODELING_API CDbIDHandler:public CDbHandlerInterface
{
public:
	CDbIDHandler(CViewControlData* pOSG) :CDbHandlerInterface(pOSG)
	{
		status=0;	 
		strName ="id";
 	}
	osg::Vec3  p1;
	osg::Vec2 p11 ;

	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa); 
};