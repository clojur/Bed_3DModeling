#pragma  once

#include "DbHandlerInterface.h"

#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"


class DB_3DMODELING_API CDbArrayHandler:public CDbHandlerInterface
{
public:
	CDbArrayHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;
		tempGroup=NULL;
		col=1;
		row=1;
		layers=1;
		colValue=0;
		rowValue=0;
		layersValue=0;
		strName ="array";
}
	int row,col,layers;

	double rowValue;
	double colValue;
	double layersValue;

	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void Action();
	void arrayObject(bool bInit );

	osg::Group*             tempGroup;
	std::vector<osg::Node*> tempObject;
};