#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

class DB_3DMODELING_API CDbOffsethandler:public CDbHandlerInterface
{
public:
	CDbOffsethandler(CViewControlData* pOSG);

	void finish();
	void begin();
	void action(bool bCancle);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void AppalyBasePT(double dx , double dy);

 	osg::Vec3     pickPoint;
	static double dOffset;
	ORIETATION bUp;
   };


