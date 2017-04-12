
#pragma once

class CViewControlData;

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>

//自定义操纵器，增加了键盘处理消息
class DB_3DMODELING_API  CDb3DManipulator:public osgGA::TrackballManipulator
{
public:
	CDb3DManipulator( osg::Camera* pCamera, CViewControlData* pOSG,int flags = DEFAULT_SETTINGS );
public:

	virtual bool performMovement();

	//重载相机的pan操作 ，默认的panModel函数不能和鼠标同步
	virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );

	virtual void zoomModel( const float dy, bool pushForwardIfNeeded = true );

	//屏蔽掉右键
	virtual bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );

	//屏蔽掉space键；
	virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

 

public:
	void DoRotate(int delta, float dx, float dy);
private:

	//动态缩放
	void centerZoom( const float dy );

	osg::Camera* m_pCamera;
	CViewControlData* myOSG;

	osg::Vec3 focusCenter;
};
