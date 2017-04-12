#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
∂‘œÛcopy√¸¡Ó
*/

class DB_3DMODELING_API CDbMirrorHandler:public CDbHandlerInterface
{
public:
	CDbMirrorHandler(CViewControlData* pOSG);
 
	float _mx,_my;
	osg::Vec3 p11, p22;
 
	osg::Vec2 startPT, endPT;

	osg::Node* pCamera;
	osg::Geometry* pGeometry ;


	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void action( );
	void mirrorObject();
	void GetMirrorMatrix(osg::Matrix& pMatrix);

	osg::Vec3 getMirrorPlane( );
	std::map<osg::Node*, osg::Vec3> centerArray;

	osg::Matrix*  pLastMatrix;

	BOOL bKeepSource;

	osg::ref_ptr<osg::MatrixTransform> pParentNode;

};
 