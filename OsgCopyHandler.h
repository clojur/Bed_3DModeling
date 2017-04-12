#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"
/*
对象copy命令
*/

class DB_3DMODELING_API CDbCopyHandler:public CDbHandlerInterface
{
public:
	CDbCopyHandler(CViewControlData* pOSG);
 	void finish();
	void begin();
	void action(bool bCancle);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void mCopyImplement();

	osg::Vec3d p11; //基点
	osg::Vec3d p22; //第二个点
 
	osg::Vec2 startPT; //起点，窗口坐标
	osg::Vec2 endPT;//橡皮筋终点，窗口坐标

	osg::Node* pCamera;
	osg::Geometry* pGeometry;


	//临时对象的根节点
	osg::MatrixTransform* pParentNode;
};