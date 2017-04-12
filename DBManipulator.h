
#pragma once

class CViewControlData;

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>

//�Զ���������������˼��̴�����Ϣ
class DB_3DMODELING_API  CDb3DManipulator:public osgGA::TrackballManipulator
{
public:
	CDb3DManipulator( osg::Camera* pCamera, CViewControlData* pOSG,int flags = DEFAULT_SETTINGS );
public:

	virtual bool performMovement();

	//���������pan���� ��Ĭ�ϵ�panModel�������ܺ����ͬ��
	virtual bool performMovementMiddleMouseButton( const double eventTimeDelta, const double dx, const double dy );

	virtual void zoomModel( const float dy, bool pushForwardIfNeeded = true );

	//���ε��Ҽ�
	virtual bool performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy );

	//���ε�space����
	virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

 

public:
	void DoRotate(int delta, float dx, float dy);
private:

	//��̬����
	void centerZoom( const float dy );

	osg::Camera* m_pCamera;
	CViewControlData* myOSG;

	osg::Vec3 focusCenter;
};
