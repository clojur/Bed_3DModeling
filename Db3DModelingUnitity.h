#pragma once

#include "osg/Geometry"
#include <osgViewer/view>
#include <osg/BoundingBox>
#include <osg/Texture2D>



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.785398163397448309616
#endif


#define OT_ALL "all obj"
#define OT_UNKNOWN  "unknown obj"

#define OSG_TRUSS "�߿�" //��ϵ��ʾ
#define OSG_SOLID "ʵ��"  //��άʵ����ʾ  


#define  NODE_SIZE_SMALL 6
#define  NODE_SIZE_BIG 10

const int handlersize = 10;
const float  TEXTABOVENODE= 1000;
const float  LINEWIDTH= 1.0f  ;
const float  LINEWIDTH_MID= 2.0f  ;
const float  LINEWIDTH_BIG= 5.0f  ;

const osg::Vec4 eleColor1   = osg::Vec4(0.05f,   0.05f,  0.05f,   1.0f);
const osg::Vec4 eleColor2   = osg::Vec4(0.05f,   0.05f,  0.05f,   1.0f); //����

const osg::Vec4 redColor    = osg::Vec4(1.0f,   0.0f,  0.0f,   1.0f);//��ɫ
const osg::Vec4 greenColor = osg::Vec4(0.0f,   1.0f,  0.0f,   1.0f);//��
const osg::Vec4 blueColor   = osg::Vec4(0.0f,   0.0f,  1.0f,   1.0f);//��
const osg::Vec4 yellowColor = osg::Vec4(1.0f,   1.0f,  0.0f,   1.0f);//��
const osg::Vec4 backColor = osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);//������ɫ �� ��
const osg::Vec4 whiteColor = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);//������ɫ �� ��

const osg::Vec4 linkNodeNameColor = osg::Vec4(1.0f,   0.0f,  0.0f,   1.0f);//���ڵ�������ɫ �� ��
const osg::Vec4 InnerNodeNameColor = osg::Vec4(1.0f,   0.0f,  1.0f,   1.0f);//�ڲ��ڵ�������ɫ�� ��ɫ
const osg::Vec4 ElementNameColor = osg::Vec4(1.0f,   1.0f,  0.0f,   1.0f);//��Ԫ������ɫ�� ��ɫ
const osg::Vec4 ComponentNameColor = osg::Vec4(1.0f,   1.0f,  0.0f,   1.0f);//����������ɫ�� ��ɫ
const osg::Vec4 ElementColor = osg::Vec4(1.0f,   1.0f,  0.0f,   1.0f);//��Ԫ��ɫ�� ��
const osg::Vec4 TendonColor = osg::Vec4(1.0f,   0.0f,  0.0f,   1.0f);//������ɫ�� ��
const osg::Vec4 TendonNameColor = osg::Vec4(1.0f,   0.0f,  0.0f,   1.0f);//����������ɫ�� ��
const osg::Vec4 HandlerColor = osg::Vec4(0.0f,  0.0f,  1.0f,   1.0f);//�е���ɫ�� ��
const osg::Vec4 SnapHandlerColor = osg::Vec4(1.0f,  1.0f,  0.0f,   1.0f);//��׽����ɫ�� ��

const osg::Vec4 SectionColor = osg::Vec4(1.0f,  0.0f,  1.0f,   1.0f);//������ɫ�� ��
const osg::Vec4 ValidSectionColor = osg::Vec4(1.0f,  0.0f,  1.0f,   1.0f);//������ɫ�� ��
const osg::Vec4 SectionIndexColor = osg::Vec4(0.0f,  1.0f,  0.0f,   1.0f);//��������������ɫ�� ��

const osg::Vec4 LoadColor = osg::Vec4(1.0f,  1.0f,  0.0f,   1.0f);//������ɫ�� ��
const osg::Vec4 SupportColor = osg::Vec4(1.0f,  1.0f,  0.0f,   1.0f);//֧����ɫ ����
const osg::Vec4 stretchHandlerColor = osg::Vec4(1.0f,  0.0f,  0.0f,   1.0f);//��ק�е���ɫ ����


enum SNAPTYPE
{
	SNAP_NO     =0x00000001,
	SNAP_GRID  =0x00000002,
	SNAP_NODE=0x00000004,
	SNAP_MID   = 0x00000008,
 	SNAP_INTERSECTION = 0x00000010,//�����ߵĽ���

};


//ָ��Ӧ�ó��������·��(�����ļ�������׺,��E:\BackUp\BridgeWizard\bin)
DB_3DMODELING_API CString OSGGetApplicationSelfPath();
DB_3DMODELING_API void AutoNormal(osg::Geometry* pGeomery ,double angle=M_PI);
DB_3DMODELING_API BOOL CalBoundBoxWH(double& bW,double& bH ,double& bL,double& cX,double& cY, double& cZ,osgViewer::View* pViw, osg::BoundingBox& bb);
DB_3DMODELING_API BOOL CalBoundBoxWH2(double& bW,double& bH ,double& bL,double& cX,double& cY, double& cZ,osgViewer::View* pViw, osg::BoundingBox& bb);


DB_3DMODELING_API CString GetObjType( osg::Node* pNode ); //���ؽڵ����� ������getDescriptions����Ҫ��������

DB_3DMODELING_API void MakeTexture(BOOL bCreate , osg::Texture2D* pTexture , const CString& strFile ,int type );

//���ݽڵ������ҵ��ڵ�
DB_3DMODELING_API osg::Node	* GetChildByName(const CString& name, osg::Node* pParent);

DB_3DMODELING_API void GetChildByName2(std::vector<osg::Node*>& nodeVec, const CString& name, osg::Group* pParent, const CString& nodeType);
DB_3DMODELING_API void  UpdateSnaper( int snapTYpe , const osg::Vec3& lPT ,const osg::Vec4& color, osg::Group* pGraph , int size=NODE_SIZE_SMALL);


DB_3DMODELING_API void GetChildDimByName(std::vector<osg::Node*>& nodeVec, const CString& name, osg::Group* pParent);


DB_3DMODELING_API void updateLineGeode(osg::Geometry* pGeometry , osg::Vec2 pt1 , osg::Vec2 pt2);
DB_3DMODELING_API void updatePolyLineGeode(osg::Geometry* pGeometry , std::vector<osg::Vec3>& nextPTs);
DB_3DMODELING_API void  CreateHandler(osg::Vec3 lPT ,osg::Vec4 color, osg::Geode* pGeode , int size=NODE_SIZE_SMALL);

//����ѡ���
DB_3DMODELING_API void updateBoxGeode( osg::Geometry* pGeometry , osg::Vec2 pt1 , osg::Vec2 pt2);
//����ѡ���
DB_3DMODELING_API void updateCircleGeode(osg::Geometry* pGeometry, osg::Vec2 pt1, osg::Vec2 pt2);
//����ѡ���
DB_3DMODELING_API void updateEllipseGeode(osg::Geometry* pGeometry, osg::Vec2 pt1, osg::Vec2 pt2,osg::Vec2 pt3);

//������������
DB_3DMODELING_API osg::Matrix computeWindowMatrix(osg::Viewport* pViewport) ;


DB_3DMODELING_API osg::Node* createSkyBox();


DB_3DMODELING_API osg::Node* createIsland(const osg::Vec3& center, float radius);
DB_3DMODELING_API CString MakeFixLength(const CString& input, int len);