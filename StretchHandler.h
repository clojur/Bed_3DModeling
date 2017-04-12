#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
框选功能对象
*/

class DB_3DMODELING_API  CDbQuickStretchHandler:public CDbHandlerInterface
{
public:
	CDbQuickStretchHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;
		pCamera=NULL;
		pGeometry=NULL;
		strName ="stretch";
		bStretch = TRUE;
 	}	
	double oldmx,oldmy;
	double newmx,newmy;

	osg::Vec3d p11;
	osg::Vec3d p22;
	osg::Vec3d tempPt;;

	osg::Vec2 startPT;
	osg::Vec2 endPT;
	osg::Node* pCamera;
	osg::Geometry* pGeometry ;

	std::map< CString, std::vector<osg::Vec3d> > m_oldPosition;
	std::vector<osg::Node*> hotHhandlers;
	BOOL bStretch;
public:
	void finish();
	void begin();
	void action( bool bCancle);


	std::vector<osg::Vec3d> FindOldPosition(const CString& strObjName); 

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};
