#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

class CDbOSGManager;

class DB_3DMODELING_API  CDbBoxZoomHandler:public CDbHandlerInterface
{

public:
	CDbBoxZoomHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;
		pNode=NULL;
		pGeometry=NULL;
	}

	float _mx,_my;
	osg::Vec2 p11,p22;
	osg::Vec2 p1, p2;
	osg::Node* pNode;
	osg::Geometry* pGeometry ;
	void finish();
	void begin();
	void boxZoom();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};
