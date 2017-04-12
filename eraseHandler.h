#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

/*
∂‘œÛcopy√¸¡Ó
*/

class DB_3DMODELING_API CDbEraseHandler:public CDbHandlerInterface
{
public:
	CDbEraseHandler(CViewControlData* pOSG) :CDbHandlerInterface(pOSG)
	{
		status=0;	 
		strName ="Erase";
 	}
	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa); 

	static void DoEraseObjects(CViewControlData* m_pOSG);
};