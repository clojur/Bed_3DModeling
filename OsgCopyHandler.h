#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"
/*
����copy����
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

	osg::Vec3d p11; //����
	osg::Vec3d p22; //�ڶ�����
 
	osg::Vec2 startPT; //��㣬��������
	osg::Vec2 endPT;//��Ƥ���յ㣬��������

	osg::Node* pCamera;
	osg::Geometry* pGeometry;


	//��ʱ����ĸ��ڵ�
	osg::MatrixTransform* pParentNode;
};