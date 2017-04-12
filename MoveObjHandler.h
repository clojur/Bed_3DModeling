#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"

class DB_3DMODELING_API CDbMoveObjhandler:public CDbHandlerInterface
{
public:
	CDbMoveObjhandler(CViewControlData* pOSG);

	void finish();
	void begin();
	void action(bool bCancle);

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	void AppalyBasePT(osg::Vec3 inputPt);

	osg::Vec3d p11; //����
	osg::Vec3d p22; //�ڶ�����
 
	osg::Vec2 startPT; //��㣬��������
	osg::Vec2 endPT;//��Ƥ���յ㣬��������

	osg::Node* pCamera; //��Ƥ������
	osg::Geometry* pGeometry ;//��Ƥ��������

	osg::ref_ptr<osg::MatrixTransform> pParentNode;
 
	bool bQuickModel;
};


