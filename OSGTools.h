#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/Optimizer>
#include <osg/Vec3>
#include <osg/Texture2D>
#include <osg/AutoTransform>
#include <osg/Group>
#include "osg/Vec4"
#include <osg/ValueObject>

#include "OSGInterface.h"
#include "OSGObject.h"

 

#include "Db3DModelingUnitity.h"  
class CDbOSGManagerInterface;

class CDbOSGManager;
 class CDbOSGManagerInterface;
 //osg���ߺ����ۺ�

#define DB_DBLCLICKOBJ WM_USER+7000
#define DB_SHOWDIALOG  WM_USER+7001

enum DIALOGPARAM
{
	DB_SET=1,
	DB_NODEDCLICK=2,
	DB_SETSTYLE = 3,
	DB_UCS = 4,
	DB_IMMODEL = 5,
	DB_BGRID = 9,

};
enum MOUSEMODE
{
	MM_REPEATLASTCOMMAND = 0,
	MM_SHOW_MENU = 1,
	MM_CONFIRM = 2,
};

//�Ƿ�escape������
DB_3DMODELING_API bool IsEscaped(const osgGA::GUIEventAdapter& ea);

DB_3DMODELING_API bool IsLButtonDown(const osgGA::GUIEventAdapter& ea);
DB_3DMODELING_API bool IsRButtonDown(const osgGA::GUIEventAdapter& ea);

//�س����Ϳո���Ƿ��� ��keydown and key =enter
DB_3DMODELING_API bool IsReturn(const osgGA::GUIEventAdapter& ea);

//����س����Ϳո�����������ε����⣬ͨ��status�����ƣ���begin��statusΪ0�� initEnter��+1
DB_3DMODELING_API bool InitEnter(const osgGA::GUIEventAdapter& ea , int& status);

//��ȡosg��������λ�õ���������
DB_3DMODELING_API osg::Vec3 GetObjWorldPos(osg::Node* pNode ,  osg::Node* pRoot);
DB_3DMODELING_API bool GetObjWorldPos(osg::Node* pNode ,  osg::Node* pRoot , osg::Vec3& pt1 , osg::Vec3& pt2);

//�����߿�ͼ������
DB_3DMODELING_API void SetBlanking(osg::Node* pNode );

//�����߼Ӵ�Ч��
DB_3DMODELING_API void SetLunKuoEffect(osg::Node* pNode,int width,osg::Vec4 color );

//�����������ߵ㻮��Ч��
DB_3DMODELING_API void SetLunKuoCenterEffect(osg::Node* pNode ,osg::Vec4 color);

//�����߼Ӵ�Ч��
DB_3DMODELING_API void SetLunKuoEffect2(osg::Node* pNode, int width,osg::Vec4 color );

//����ʵ�幹����ɫ,
DB_3DMODELING_API void SetNodeColorExtend(osg::Node* pNode ,osg::Vec4 ambientColor,osg::Vec4 diffuseColor,BOOL bUseGLColor =TRUE);



DB_3DMODELING_API BOOL IsTempObject(osg::Node* pNode);

//���´���ֱ�߱�ע
DB_3DMODELING_API std::vector<osg::Drawable *> createLineDim( osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const std::wstring& text, osg::Vec4 color, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER);
DB_3DMODELING_API std::vector<osg::Drawable *> createLineDim2(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bAutoScale = FALSE);
DB_3DMODELING_API std::vector<osg::Drawable *> createLineDim3(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, osg::Vec3 p1, osg::Vec3 p2, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bAutoScale = FALSE);
DB_3DMODELING_API std::vector<osg::Drawable *> createLineDim4(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, std::vector<osg::Vec3> dimPos, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bAutoScale = FALSE);
// ����ͷ�����߱�ע:vecPnts����������ڵ���2��vecPnts[0]:��ͷ�ķ���; ��ע�����ݷ�����vecPnts[vecPnts.size()  -2]�� vecPnts[vecPnts.size()  -1]���ߴ�
DB_3DMODELING_API std::vector<osg::Drawable *> createSingleArrowLineDim3(double textbellow, osgText::Font* pCnFont, double textSize, double halfH, double arrorSize, std::vector<osg::Vec3>&vecPnts, const CString& text, osg::Vec4 color, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bAutoAScale = FALSE);
DB_3DMODELING_API std::vector<osg::Drawable *> createSingleArrowLine(double arrorSize, std::vector<osg::Vec3>&vecPnts, osg::Vec4 color);

//���㾲̬����ֱ��
DB_3DMODELING_API osg::Drawable* createLineGeodeByRadius(osg::Vec3 pt1, osg::Vec3 pt2, double dRadisu, osg::Vec4 color);

//���㾲̬����ֱ��
DB_3DMODELING_API osg::Geometry* createLineGeode(osg::Vec3 pt1 , osg::Vec3 pt2 ,osg::Vec4 color);

//���㶯̬����ֱ��
DB_3DMODELING_API osg::Geometry* createDynamicLineGeode(osg::Vec2 pt1, osg::Vec2 pt2, osg::Vec4 color);


DB_3DMODELING_API osg::Geometry* createCircle(osg::Vec3 pt1 , double radius ,osg::Vec4 color,int size);
 
DB_3DMODELING_API osg::Geometry* createEllipse(osg::Vec3 pt1, double a,double b, osg::Vec4 color, int size);
 

//������ͷ x,yƽ��
DB_3DMODELING_API osg::Geometry* createArror(osg::Vec3 pt1 , double dSize,   double dAngle , osg::Vec4 color,double dAngle2=M_PI_4);
//������ͷ x,zƽ��

DB_3DMODELING_API osg::Geometry* createArror2(osg::Vec3 pt1, double dSize, double dAngle, osg::Vec4 color, double dAngle2 = M_PI_4);

//������ͷ
DB_3DMODELING_API osg::Geometry* createArror(const osg::Vec3 &basePoint, double length, double arrow_length, const osg::Vec3 &vec, const osg::Vec3 &pern_vec, const osg::Vec4 &color, double arrow_angle = M_PI / 6, int nArrowType = 0, bool bStart = true, bool bDoubleArrow = false);

//���������ֱ��
DB_3DMODELING_API osg::Geometry* createPLineGeode(  std::vector<osg::Vec3>&   allPTs ,osg::Vec4 color, BOOL bClose, float nWidth = 1);
DB_3DMODELING_API osg::Geometry* createDPLineGeode(std::vector<osg::Vec3d>& allPTs, osg::Vec4 color, BOOL bClose, float nWidth = 1);

DB_3DMODELING_API osg::Geometry* createMultiLineGeode(std::vector<std::vector<osg::Vec3> > & allPTs, osg::Vec4 color, BOOL bClose);

 
//���㴴��̧ͷ��ʾֱ�ߣ���������
DB_3DMODELING_API osg::Node* createHudLine(CViewControlData* pOSG,  osg::Vec2 p1 , osg::Vec2 p2, osg::Geometry*& pGeometry );

//һ��˳��㴴������
DB_3DMODELING_API osg::Geometry* createLinesGeode(osg::Vec3Array *points, osg::Vec4 color);


//���㴴��ѡ���
DB_3DMODELING_API osg::Geometry* createBoxGeode(osg::Vec3 pt1 , osg::Vec3 pt2);



//���㴴��̧ͷ��ʾѡ��򣬴�������
DB_3DMODELING_API osg::Node* createHudSelectBOX(CViewControlData* pOSG,  osg::Vec2 p1 , osg::Vec2 p2, osg::Geometry*& pGeometry );
//���㴴��̧ͷ��ʾѡ��򣬴�������
DB_3DMODELING_API osg::Node* createHudSelectCircle(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Geometry*& pGeometry);
//���㴴��̧ͷ��ʾѡ��򣬴�������
DB_3DMODELING_API osg::Node* createHudSelectEllipse(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3,osg::Geometry*& pGeometry);
//���㴴��̧ͷ��ʾѡ��򣬴�������
DB_3DMODELING_API osg::Node* createHudSelectTorus(CViewControlData* pOSG, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3,osg::Geometry*& pGeometry);


//�ַ���ת����
DB_3DMODELING_API osg::Vec3 GetPosition(const CString& strValue);
 
DB_3DMODELING_API std::vector<CString> GetStrPosition(const CString& strValue);

//�����ڵ㴴���ַ�����ʽ �� ���磺 0,0,0�� �������,start����5,5,5��x,y,z��������,delta����2 0��������������,nCount����
DB_3DMODELING_API void analyzeStringFormatOfCreateNode(const CString& strValue, osg::Vec3& start,osg::Vec3& delta , int& nCount);
 

 
DB_3DMODELING_API osg::Node	* GetChildGroupByName(const CString& name, osg::Node* pParent);


 
 //��������
DB_3DMODELING_API osgText::Text* create3DText(        osgText::Font*, const osg::Vec3& position, float characterSize, const CString& message,     const osg::Vec4& color = redColor, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER);
DB_3DMODELING_API osgText::Text* create3DWTextNoScale(osgText::Font*, const osg::Vec3& position, float characterSize, const std::wstring& message, const osg::Vec4& color = redColor, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bEnChart = TRUE);
DB_3DMODELING_API osgText::Text* create3DTextNoScale( osgText::Font*, const osg::Vec3& position, float characterSize, const CString& message,      const osg::Vec4& color = redColor, osgText::Text::AlignmentType Alignment = osgText::Text::CENTER_CENTER, BOOL bEnChart = TRUE);

 
 
//���������Χ��
DB_3DMODELING_API void CalcOBB(std::vector<osg::Vec3>& arr, 	osg::Vec3  & base_pnt, osg::Vec3  & side_0, osg::Vec3  & side_1, osg::Vec3  & side_2 );


//����һ��HUD����
DB_3DMODELING_API osg::Camera* createHUDObj(osg::Geode* pChild,BOOL bUsedWinSize=FALSE,CViewControlData* pOSG=NULL);
DB_3DMODELING_API osg::Camera* createHUDCamera(BOOL bUsedWinSize=FALSE,CViewControlData* pOSG=NULL);
DB_3DMODELING_API osg::Camera* createOffsetHUDCamera(BOOL bUsedWinSize=FALSE,CViewControlData* pOSG=NULL);

//��ȡ��Ԫ�����ҽ�����Ϣ
//lx,�����Y�����ƫ��ֵ��rx ���ҽ���Y�����ƫ��ֵ(Y���ĺ�Ymax�Ĳ�ֵ)bVertSection =1Ϊ����Ϊ��ֱ���棬 2Ϊ�� 0Ϊ������ֱ����
DB_3DMODELING_API void GetElementOutLine(BOOL bVertSection, double factor, osg::Vec3 axis, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB);

DB_3DMODELING_API void GetElementOutLineNew(BOOL bVertSection, double factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB);

DB_3DMODELING_API void GetElementOutLineConsiderBrokenLine(double angle, BOOL bVertSection, double factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, double lx, double rx, osg::Vec3& lpt, osg::Vec3&rpt, double& dLSectionH, double& dRSectionH, BOOL bBrokenLineL, BOOL bBrokenLineR, osg::Vec3& lT, osg::Vec3&lB, osg::Vec3&rT, osg::Vec3& rB);

   
//���ɵ�Ԫʵ��ģ��
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplement(const osg::Vec4& color,  const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids,std::vector<bool> rightSolids,	   const std::vector<std::vector<osg::Vec4>> &leftColors,  const std::vector<std::vector<osg::Vec4>> &rightColors);
//���ɵ�Ԫʵ��ģ��
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplement4(const osg::Vec4& color, const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, const std::vector<std::vector<osg::Vec4>> &leftColors, const std::vector<std::vector<osg::Vec4>> &rightColors,BOOL bDrawLeft,BOOL bDrawRight);

//���ɵ�Ԫʵ��ģ��
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplement1(const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, const std::vector<osg::Vec4> &leftColors, const std::vector<osg::Vec4> &rightColors);

//���ɵ�Ԫʵ��ģ��
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplement2(const osg::Vec4& leftColor, const osg::Vec4& rightColor, const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids);

//���ɵ�Ԫ�߿�ģ��
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplementSimple(const osg::Vec4& color, const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, BOOL bdrawLeft, BOOL bDrawRight);
DB_3DMODELING_API std::vector<osg::Geometry*> CreateElementImplementSimple1(const std::vector<std::vector<osg::Vec3>> &leftSecs, const std::vector<std::vector<osg::Vec3>> &rightSecs, std::vector<bool> leftSolids, std::vector<bool> rightSolids, const std::vector<osg::Vec4> &leftColors, const std::vector<osg::Vec4> &rightColors);


//ѡ�ж��󣬼��ϰ�Χ��
DB_3DMODELING_API void MakeObjectSelected(std::vector<osg::Node*>& allObj ,CViewControlData* m_pOSG);
DB_3DMODELING_API void MakeObjectUnSelected(std::vector<osg::Node*>& allObj, CViewControlData* m_pOSG);

//�õ��������״
DB_3DMODELING_API osg::Geometry* GetSectionGeometry(const std::vector<std::vector<osg::Vec3>> &leftSecs2, osg::Vec4 color, BOOL bLeft);
DB_3DMODELING_API osg::Geometry* GetSectionGeometry1(const std::vector<std::vector<osg::Vec3>> &leftSecs2, const std::vector<osg::Vec4>& leftColors, BOOL bLeft);
DB_3DMODELING_API osg::Geometry* GetSectionGeometry2(const std::vector<std::vector<osg::Vec3>> &leftSecs2, std::vector<std::vector<osg::Vec4>>& leftColors);

DB_3DMODELING_API BOOL PointIsInRect(osg::Vec2 point , osg::Vec2 p1 , osg::Vec2 p2 );
DB_3DMODELING_API void  CreateHotHandler(osg::Vec3 lPT ,osg::Vec4 color, osg::Geode* pGeode , int size=NODE_SIZE_SMALL);


DB_3DMODELING_API void CreateNode3DModel3(int type, osg::Texture2D * texture, osg::Vec3 lPT, osg::Node* pGeode, osg::Vec4 color, int size = NODE_SIZE_SMALL);


  //��ʾ���Ա�
DB_3DMODELING_API void  DisPlayProperty(CViewControlData* pOSG ,std::vector<osg::Node*>& allobjs);

//������Դ
DB_3DMODELING_API osg::ref_ptr<osg::Group> createLight( osg::Vec3& direction, osg::ref_ptr<osg::Node> node);

//������Դ
DB_3DMODELING_API void modifyLight( osg::Vec3& direction, osg::ref_ptr<osg::Node> node, osg::Light*  pLight);


//��������������Χ����Ȱ�Χ��������Χ�򣬾��Ƚϸ�
DB_3DMODELING_API osg::Node*  GetObjOBB(osg::Node* selected_node);

//��������������Χ����Ȱ�Χ��������Χ�򣬾��Ƚϸ�
DB_3DMODELING_API osg::Node*  GetObjOBB2(osg::Node* selected_node);


//��������ͼ�ζ����ڲ�����createMultiLineGeode
//DB_3DMODELING_API osg::Geometry* AddGroupLine(const std::map<CString, CDbCurve3D>& curves, COLORREF color = RGB(0, 0, 255));

//����ָ�����Ƶ�drawable
DB_3DMODELING_API osg::Drawable*  GetDrawable(osg::Geode* pGeode , const CString& strName);


DB_3DMODELING_API void CreateConstraint3DModel(const osg::Vec3& position, osg::Geode*  pGeode, double supportSize = 60, osg::Vec4 zzcolor = osg::Vec4(1, 0, 1, 1));

 

 
DB_3DMODELING_API void SetVisible(osg::Node* pNode, bool bVisible = true);
DB_3DMODELING_API void  LoadShaderSource(osg::Shader* shader, const CString& fileName);

DB_3DMODELING_API void CreateLightShader(osg::StateSet* pSet, const osg::Vec4& vec4);


//�������ϵĵ�ת��Ϊ�ռ��ϵĵ�
/*
worldN �����߷���
axis   ����X�᷽��
basePt  ���氲װ��,
cx ��������X����
cy ��������Y����
dx ���������x����
dy ���������y����
������������ռ�����
int type 0 �� 1��ֱ���� 2 ������
*/

DB_3DMODELING_API osg::Vec3 GetPointFromSectionToSpace(int type, osg::Vec3 axis, osg::Vec3 worldN, osg::Vec3 basePT, double cx, double cy, double dx, double dy, double alfa);

//���������Ӵ�Ч��

DB_3DMODELING_API osg::Node* createOutLineEffect(CViewControlData* pOSG, osg::Node* subgraph, const osg::Vec4& clearColour, double tex_width, double tex_height);

 
 
DB_3DMODELING_API void GetAllChildNode(std::vector<osg::Node*>& nodes, osg::Node* pParent);

DB_3DMODELING_API osg::Vec4 GetColorByIndex(int colorIndex);
DB_3DMODELING_API int GetColorIndexByColorValue(osg::Vec4 color);

DB_3DMODELING_API osg::Vec3 GetVertexDir(std::vector<osg::Vec3>& vertexs, int index, BOOL bClose, BOOL bClockWise);

DB_3DMODELING_API osg::Geometry*  CreateDimArror(osg::Vec4 color, osg::Vec3 pt1, osg::Vec3 lineDir, osg::Vec3 nodeDir, double lineL, double nodeL);


DB_3DMODELING_API osg::Geometry* CreateRectByTexture(const CString& strTexture, osg::Vec3 top_left, osg::Vec3 bottom_left, osg::Vec3 bottom_right, osg::Vec3 top_right);


//����Ԫ�ֲ�����ת������������
//factor ���� ��lpt������ڵ� ��rpt�ҽڵ㣬leftSecs2�����ľֲ�����ϵ���꣬
//bBeam �Ƿ�Ϊ������
//bVertSection �Ƿ�Ϊ��ֱ����
DB_3DMODELING_API void GetSectionMainAxis(double angle,  BOOL bVertSection, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, int factor, osg::Vec3 lpt, osg::Vec3 rpt,
	BOOL bL, BOOL bR, osg::Vec3d& localLXN, osg::Vec3d& localLYN, osg::Vec3d& localRXN, osg::Vec3d& localRYN);

DB_3DMODELING_API void CreateElementGeometry( BOOL bVertSection,osg::Vec3 xaxis, int factor, osg::Vec3 lpt, osg::Vec3 rpt,
	std::vector<std::vector<osg::Vec3>>& leftSecs2,std::vector<std::vector<osg::Vec3>>& rightSecs2,double lcx, double rcx, double lcy, double rcy);
DB_3DMODELING_API void CreateElementGeometryNew(double angle, BOOL bVertSection, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, int factor, osg::Vec3 lpt, osg::Vec3 rpt,
	std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec3>>& rightSecs2, double lcx, double rcx, double lcy, double rcy);
DB_3DMODELING_API void CreateElementGeometryNewByBrokenLine(double angle, BOOL bVertSection, int factor, osg::Vec3 leftAxis, osg::Vec3 rightAxis, osg::Vec3 leftTangent, osg::Vec3 rightTangent, osg::Vec3 lpt, osg::Vec3 rpt,
	BOOL bL,BOOL bR,std::vector<std::vector<osg::Vec3>>& leftSecs2, std::vector<std::vector<osg::Vec3>>& rightSecs2, double lcx, double rcx, double lcy, double rcy);

#define IsSHIFTpressed()    ( (GetKeyState(VK_SHIFT)             & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsCTRLpressed()     ( (GetKeyState(VK_CONTROL)      & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsALTpressed()      ( (GetKeyState(VK_MENU)             & (1 << (sizeof(SHORT)*8-1))) != 0 )

DB_3DMODELING_API void CreateTextEffect(osg::Node* pNode, osg::Vec4 color, osgText::Text* pObj,CViewControlData* pOSG);

DB_3DMODELING_API void CreateStatusEffect(CViewControlData* pOSG, osg::Node* pNode, bool bselect);

 DB_3DMODELING_API void SetNodeVisible(osg::Node* pDimObj, BOOL bVisible);


 DB_3DMODELING_API void RestoreView(osgViewer::View* mView, osg::Matrixd ProjectionMatrix, osg::Vec3 eye, osg::Vec3 center, osg::Vec3 up);
