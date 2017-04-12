#pragma  once

class CDbHandlerInterface;
class CDbBoxSelector;
class CDbPickModeHandler;
class CDbHandlerInterface;
class CDbOSGDataCenter;
class CDbOSGManagerInterface;
class CDbObjInterface;
#include <atomic>
#include "osg/Node"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Vec3"
#include <osgText/Font>
#include "osgViewer/Viewer"
#include <osg/MatrixTransform>
#include "SetupScene.h"


#include "OSGObject.h"
#include <mutex>
 
#define  DB_ADDDATA    WM_USER+9000
#define  DB_DELETEDATA WM_USER+9001
#define  DB_UPDATEDATA WM_USER+9002


//�����ʽ
enum OSG_CURSOR_TYPE
{
	OSG_InheritCursor,
	OSG_NoCursor,
	OSG_RightArrowCursor,
	OSG_LeftArrowCursor,
	OSG_InfoCursor,
	OSG_DestroyCursor,
	OSG_HelpCursor,
	OSG_CycleCursor,
	OSG_SprayCursor,
	OSG_WaitCursor,
	OSG_TextCursor,
	OSG_CrosshairCursor,
	OSG_HandCursor,
	OSG_UpDownCursor,
	OSG_LeftRightCursor,
	OSG_TopSideCursor,
	OSG_BottomSideCursor,
	OSG_LeftSideCursor,
	OSG_RightSideCursor,
	OSG_TopLeftCorner,
	OSG_TopRightCorner,
	OSG_BottomRightCorner,
	OSG_BottomLeftCorner,
	OSG_NORMAL_CUSSOR=30,
	OSG_INPUT_CUSSOR=31,
	OSG_SELECT_CUSSOR=32,
	OSG_SELECT_PAN=33,
	OSG_SELECT_ZOOM=34,
	OSG_SELECT_ROTATE=35,
};

//��Ƥ����
struct rubbleLine
{
	osg::Vec2 p1;
	osg::Vec2 p2;
	osg::Geometry* pGeometry;
};

//��Ƥ�����
struct rubbleBox
{
	osg::Vec2 p1;
	osg::Vec2 p2;     //���� ��԰
	osg::Vec2 p3;    //��Բ��
	osg::Geometry* pGeometry;
};

struct viewSet
{
	viewSet()
	{
		titleSize     = 60;
		bDefaultFocus = false;
	}
	CString strTitle;
	CString strPath;
	double x;
	double y;
	double width;
	double height;
	double titleSize;
	bool   bDefaultFocus;
};

struct SNAPINFO //����
{
	DWORD firstNo;
	DWORD secondNo;
	std::vector<osg::Vec3d> intersectionArr;
};


/////������
class property_list_MultiObject : public CDb3DProperty
{
public:
	property_list_MultiObject();
	virtual ~property_list_MultiObject();
	int objCount;
	std::vector<DWORD> mulObjIDs;
	bool bSame;
	CViewControlData* pOSG;
public: 
	virtual  bool ApplyProperty(const CDb3DPropertyData& data){ return false; };
	virtual  void CreateProperty(CDb3DPropertyData& data){};
};

//��άͼ�ӿ�
class DB_3DMODELING_API CViewControlData
{
public:
	CDbOSGDataCenter*              m_pUserData;	            //ͼ����������
	CDbBoxSelector*                       m_pSelector;         //ѡ�񹤾�
	CDbHandlerInterface*          m_pCurHandler;            //��ǰ�������
	CDbPickModeHandler*              m_pPickModelHandler;  //��������
public:
	int       viewType;                          //��ͼ���� ��1-3�� 2DͶӰ  4͸��ͼ   ,  9 �Զ�, 10�û�����ϵ
	osg::Vec3 localOrg;
	osg::Vec3 localXDir;
	osg::Vec3 localYDir;
private:
	CString   strCurCoordSys;
	CString   strLastCoordSys;
public:
	BOOL bEnableSnap;
	DWORD   m_snapType;            // դ��׽�� �ڵ㲶׽�� ��Ԫ�е㲶׽�� 
	int   m_command;       //��ǰ״̬���Ƿ�����������
	int   manipulatorType; //��ǰ�������� �� 1ƽ�ƣ�2 ��ת ��3���ţ�0Ĭ�� ѡ�� 4�ı䴰�ڴ�С��
	int   selectStatus;        //1 ѡ�к���ʾ��Χ�� 2ȡ��ѡ�У� ɾ����Χ��3 Ĭ��
	BOOL m_bOrtho;                                      //����ģʽ��
	osg::Vec4 backColor;                            //������ɫ
	osg::Vec4 offsetBkColor;					//�Ҳ���ͼ������ɫ
	bool bParalled;                                     //�Ƿ�ƽ��ͶӰ
	BOOL bNeedClearSelect;                     //�Ƿ���Ҫ���ѡ��,0�������ѡ�� ��1���ѡ�� 2 �ȼ�¼�ϴ�ѡ����������ѡ�񼯣���ѡ���ϴζ����൱��ˢ�����Ա�

	CString m_showType;

	BOOL m_bInstallValid; //�Ƿ�ʱˢ�� 

	int unitFactor;    ///��ģ�ĵ�λ������1Ϊmm �� mΪ1000��


	int  bReDrawCount;                                   //��Ⱦ�Ƿ����
	osg::ref_ptr<osg::Group> mRoot;                //�ܸ��ڵ�
	osg::ref_ptr<osg::Group> GraphicsGroup;   //��ЧͼԪ�ܽ��
	osg::ref_ptr<osg::Group> tempObjs;           //��ʱ�����ܽ��

	osg::ref_ptr<osg::Group> StretchHandlers;  //�е������ڵ�
	osg::ref_ptr<osg::Group> SelectEffectRoot;     //ѡ�ж���Ч�����ڵ�
	osg::ref_ptr<osg::Group> HeightLightEffectRoot;     //��������Ч�����ڵ�
	osg::ref_ptr<osg::Group> validGraph;           //�û���ͼ�θ��ڵ㣬��mRoot������ ��������coordObj �� �������,��ʱ���� �����Χ���������
	osg::ref_ptr<osg::Group> allText;                  //���е�����

	osg::ref_ptr<osg::Group> pTerrain;                  //����

	osg::ref_ptr<osg::Group> snapGraph;                //��׽��

	osg::ref_ptr<osg::Group> mOffsetViewRoot;                //������ͼ�ܸ��ڵ�


	osg::ref_ptr<osg::Group> dimObjs;  //��ע����


	BOOL       bClearSnap;     //�Ƿ������׽��
	BOOL       bUpdateSnap;     //�Ƿ���²�׽��
	int            snapValidType; //��׽������
	osg::Vec3 snapPos;         //��׽��λ��

	//��ӵ�
	CMutex m_AddOverLayoutMutex;
	std::vector<osg::Node*> overLayoutObjects;  //��ʱ���� : ��Ƥ��,ѡ���
	std::mutex m_AddTempMutex;
	std::vector<osg::Node*> tempObjects;           //�û���������ʱ����

	//��Ҫɾ����
	CMutex m_DelOverLayoutMutex;
	std::vector<osg::Node*> delOverLayoutObjects; //��ʱ���� : ��Ƥ��,ѡ���

	CMutex m_DelTempMutex;
	std::vector<osg::Node*> delTempObjects;           //�û���������ʱ����

	//////////////////////////////////////////////////////////////////////////////
	CMutex m_AddMutex;
	CMutex m_DelMutex;
	CMutex m_UpdateMutex;
	std::map<CDbObjInterface*, std::vector<osg::Node*>>  updateObjects;           	//��Ҫ�޸ĵĶ���
	std::map<CDbObjInterface*, std::vector<osg::Node*> > addObjects;           //�û������ķ���ʱ����
	std::vector<osg::Node*>     delObjects2;           //�û������ķ���ʱ����


	osg::Node* pRubBoxCamera;
	osg::Geometry* pRubBoxGeometry;
	void UpdateRubbox( osg::Vec2 p1, osg::Vec2 p3);
	void InitRubbox(osg::Vec2 p1, osg::Vec2 p3);
	void SetRubboxVisible(BOOL bVisible);

 
	std::vector<rubbleLine> rubbleline;//��Ƥ����
	std::vector<rubbleBox> rubblebox;//��Ƥ����
	std::vector<rubbleBox> rubbleCircle;//��Ƥ����
	std::vector<rubbleBox> rubbleEllipse;//��Ƥ����

	//////��Χ��
	std::vector<CString> addBoundingBox;                 //��Ҫ��Ӱ�Χ��Ķ���
	std::vector<CString> removeBoundingBox;           //��Ҫ�Ƴ���Χ��Ķ���

	//��׽�ų�����
	std::vector<CString> snapExincludes;

	//��¡��������ʱ����
	std::vector<CDbObjInterface*> delCloneObjes;


	int MINRADIUS;
	int defaultRaidus;

	double girdFactor;

	osg::Texture2D* GetPointTexture(int ptType); //	int ptType; //0�����ؼ��� 1.֧��λ�� 2 ����ê�� 3�����ο��ߵ� 4Ӧ������ 5 ��Ч��ȵ�,6�����

	osg::Texture2D* GetFunctionPtTexture(osg::ref_ptr <osg::Texture2D>& pTextTure, const std::string& strTexturePath);
 

	osg::ref_ptr <osg::Texture2D> pointTexture;
	osg::ref_ptr <osg::Texture2D> zzPtTexture;
	osg::ref_ptr <osg::Texture2D> mgPtTexture;
	osg::ref_ptr <osg::Texture2D> steelPtTexture;
	osg::ref_ptr <osg::Texture2D> checkPtTexture;
	osg::ref_ptr <osg::Texture2D> vwPtTexture;
	osg::ref_ptr <osg::Texture2D> allignPtlTexture;

	osgText::Font* GetEnTextFont(); //�����ε� texture
	osgText::Font* GetCnTextFont(); //�����ε� texture


	osgText::Font* GetEnTextFontForColor(osg::Vec4 color); //�����ε� texture
	osgText::Font* GetCnTextFontForColor(osg::Vec4 color); //�����ε� texture


	osg::ref_ptr<osgText::Font>      textFontEn;
	osg::ref_ptr<osgText::Font>      textFontCn;
	CString strFontFileEn;
	CString strFontFileCn;

	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >     colorTextFontEnMap;
	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >     colorTextFontCnMap;

	//
	BOOL bReforceUpdateCallBack;


	BOOL m_bNeedChangeShowType;           //�Ƿ��л�����ʾģʽ ���ص�������ʹ�ã�
	virtual void OnChangeShowType(){ ; }
	virtual void OnMenuCommand(UINT id){ ; }
	//copyǰ����
	virtual std::vector<CDbObjInterface*> OnPreCopy(const std::map<osg::Node*, CDbObjInterface*>& selectetSetMap);

	//��ʼ����
	virtual void beginOperation(const CString& strOperation);
	//��������
	virtual void endOperation();
	//ֱ���޸�
	virtual void doOperation(const CString& strOperation,CDbObjInterface* pData, BOOL bUpdateDB=TRUE);

public:
	CViewControlData();
	virtual ~CViewControlData();

	void Init();
	/*
	�����ڲ���·�����Ʋ��Ҷ������ó����ڲ�
	*/
	CDbObjInterface* FindObj(const CString& objName, const CString& type);
	CDbObjInterface* FindObjByID(const DWORD& objID, const CString& type);
	CDbObjInterface* FindObjByTitle(const CString& objTitle, const CString& type);

	/*
	��������
	*/
	virtual void addData(CDbObjInterface* pData, BOOL bUpdateDB = TRUE);
	virtual void addDataOperation(const CString& strOperation, CDbObjInterface* pData, BOOL bUpdateDB = TRUE);

	/*
	����ɾ��
	*/
	virtual void preDelData(std::vector<CDbObjInterface*>& pData);
	virtual void delData(std::vector<CDbObjInterface*> pData, BOOL bUpdateDB = TRUE);
	virtual void postDelData(std::vector<CDbObjInterface*> pData, BOOL bUpdateDB = TRUE);
	/*
	�������
	*/
	virtual void updateData(CDbObjInterface* sectionData, BOOL bUpdateDB = TRUE);


	/*
	�����ܷ�ɾ��
	*/
	virtual bool canBeDel(CDbObjInterface* pData){ return true; }

	/*
	����������
	*/
	void SetHandler(CDbHandlerInterface* pHandler);


	/*
	��õ�ǰѡ�ж�����Ŀ
	*/
	int GetSelectObjCount();

	/*
	���ص�ǰѡ�ж�������
	*/
	CString GetSelectNodeName();


	//���ص�ǰѡ�����
	std::vector<CDbObjInterface*> GetCurSelectObject();
	/*
	�����Ƿ�ѡ��
	*/
	BOOL IsSelected(const CString& strObj);

	BOOL IsHeightLight(const CString& strObj);

	/*ѡ�����
	*/
	virtual void PostObjSelected(osg::Node* pNode);
	virtual void PostObjDClick(osg::Node* pNode);


	/*
	��ȡ���ڴ�С
	*/
	void GetWindowSize(double& dx, double& dy);

	void GetOffsetWindowSize(double& dx, double& dy);

	/*
	��ȡ��굱ǰλ�ã���������
	*/
	CPoint GetCursorPos();

	/*
	������Ļ������������������
	*/
	osg::Vec3d getWorldPos(double dx, double dy, BOOL bSnap);
	osg::Vec3d getWorldPos(osg::Vec2& windPT, BOOL bSnap){ return getWorldPos(windPT.x(), windPT.y(), bSnap); }

	/*
	��������������������Ļ����
	*/
	osg::Vec2 getWindosPos(double dx, double dy, double dz);
	osg::Vec2 getWindosPos(const osg::Vec3d&wordPT){ return getWindosPos(wordPT.x(), wordPT.y(), wordPT.z()); }

	/*
	������׽
	*/
	virtual BOOL getSnapPT(osg::Vec3d& inPutPT, const osg::Matrixd&VPW, const osg::Matrixd& InverseVPW);

	/*
	��ȡ���󸸽ڵ�
	*/
	virtual osg::Group* GetDataParent(CDbObjInterface* pData);

	//�û���ʱ�������
	void addTempObject(osg::Node* pNode);
	void clearTempObject(osg::Node* pNode);
	//�û���ʱ�������
	void addOverLayoutObject(osg::Node* pNode);
	void clearOverLayoutObject(osg::Node* pNode);

	//��Χ�����
	void addBoundingBoxObj(const CString& strObj);
	void clearBoundingBoxObj(const CString& strObj);

	//��Ҫ��������ˢ�µĶ���
	void addUpdateObject(CDbObjInterface* pData, osg::Node* pNode);
	void removeUpdateObject(CDbObjInterface* pData);//zmz


	//��Ҫˢ��dim�Ķ���
	std::vector<CDbObjInterface*> updateDimObjects;
	//ˢ�±�ע
	void addUpdateDim(CDbObjInterface* pData);

	//��Ҫ��������ˢ�µĶ���
	void addObject(CDbObjInterface* pData, osg::Node* pNode);
	//��Ҫ��������ˢ�µĶ���
	void delObject(osg::Node* pNode);

	//��Ƥ�����
	void addRubbline(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbbox(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbCircle(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbEllipse(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3);

	/*
	����Ӷ���
	*/
	void clearAllChild(osg::Group* pNode);
	//
	void RemoveName(CDbObjInterface* pData);
	/*
	��ѡ�Ŵ�
	*/
	void BoxZoom(osg::Vec2 pt1, osg::Vec2 pt2);

	/*
	ȫ��,��ȫ����־ѹ��ص�����
	*/
	void DoFullSceen();

	void DoMax();

	void DoDelView();



	//ʵ��ȫ������
	void FullSceenImpl();

	//��ʾ�л�, ������ͼ����
	void ChangeViewType();
 	/*
	��ѡ�����
	*/
	osg::Node* pickSingle(double x, double y, osgViewer::View* viewer, BOOL _usePolytopeIntersector, BOOL _useWindowCoordinates, osg::Vec3& insectPoint);

	/*
	��ѡ����
	*/
	std::vector<osg::Node*> BoxPick(float mx0, float my0, float mx1, float my1, osgViewer::View* viewer,BOOL bReserveSelect);

	/*
	����ѡ�ж���ѡ�ме���Ŀ
	*/
	int GetSelectHotPointCount();

	/*
	���ض���е�����
	*/
	std::vector<int> GetStretchIndex(const CString& strObj);

	void SetEnableSnap(BOOL flag);

	//����ͼ����άͶӰ
	void TopView();

	//����ͼ
	void LeftView();
	//ǰ��ͼ
	void FrontView();

	//��άͶӰ��ͼ
	void TopView2();
	void LeftView2();
	void FrontView2();

	void AutoLayout();
	// �Զ���ֲ�����ϵ 2D
	void LocalView();

	//͸��ͶӰ��ͼ
	void PerspectiveView();

	/*
	��ݼ�
	*/
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	/*
	���������������
	*/
	virtual bool OnCommand(const CString& strCommand);
	virtual	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool bInCommand();
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);

	virtual void InitCommandString();
	/*
	��̬����դ��
	*/
	virtual void UpdateGrid();

	void SetBackColor(const osg::Vec4& color);
	void SetOffsetViewBkColor(const osg::Vec4& color);
	/*
	ѡ���Ƿ�����ϴε�ѡ����
	*/
	bool SetSelectFlag(bool bClearPreSel);

	/*
	��ȡ��ǰ��ͼ
	*/
	CView* GetCurrentCWnd();

	void SetReDraw(BOOL flag);

	//
	void DoUpdateCallBack();

	void  SetBehavorType(int type);
	int    DoGetBehavorType();
	//boundingBox
	virtual osg::BoundingBox computeBoundBox() const;
	virtual osg::BoundingSphere CalBoundBox();


	void  ClearHandlers(osg::Node* pNode);
	void AddHandlers(osg::Node* pNode);
	BOOL IsHandlers(osg::Node* pNode);

	void  addSelectEffect(osg::Node* pNode, osg::Vec4 color);
	void  removeSelectEffect(osg::Node* pNode);

	BOOL IsAddSelectEffect(osg::Node* pNode);

	//���Ӹ�����ʾЧ�� �� ����ƶ�����������ʱ��
	void  addHightLigntEffect(osg::Node* pGeode, osg::Vec4 color);
	//ɾ��������ʾЧ�� �� ��������ǣ�
	void removeHightLigntEffect(osg::Node* pGeode);


	//�������ִ�С
	void SetTextSize(const CString& type, int size);

	//�������ִ�С
	int  GetTextSize(const CString& type);

	//�������ִ�С
	void SetObjSize(const CString& type, int size);

	//�������ִ�С
	int  GetObjSize(const CString& type);

	//��ʼ�����ִ�С
	virtual void InitSize(){ ; }

	//�������ִ�С
	void SetObjColor(const CString& type, osg::Vec4 color);

	//�������ִ�С
	osg::Vec4  GetObjColor(const CString& type);

	//��ʼ��������ɫ��С
	virtual void InitColor(){ ; };

	virtual void InitVisible(){ ; };

	virtual double GetNameTxSize(){ return 1500; };
public:
	//��ʾ�����ر��
	virtual void ShowGrid();

	//��ʾ������������
	void ShowAxis();

	//դ��׽
	void GridSnap(const osg::Vec3& resPT,osg::Vec3& snapPT);

	/*
	������׽
	*/
	BOOL OrthoPoint(osg::Vec3d& resPt, osg::Vec3d &point, const osg::Matrixd& VPW, const osg::Matrixd& InverseVPW);

	//
	void setManipulator(int viewType , osg::ref_ptr<osg::Group> pNode, osgViewer::View* pView,int width);
	//��Data��Ӧ�ó������ݿ���ɾ�� ��
	virtual void ImplementDelDatas(std::vector<CString>& objNames);

	/*
	���������
	*/
	virtual void OnUpdateDBObject(const CString &strObjectPath, int modifyType, const std::vector<CString> &vecVariable = std::vector<CString>(0));

	void InitAxis();

	/*
	�����ӿڴ�С , ���ߺ���
	*/
	void SetViewPort( int type, int vptSpace ,double width , double height);

	/*
	���õ�ǰ���
	*/
	void SetCursorType(int type);

	/*
	��ʼ����������
	*/
	virtual void InitSceneGraph();
	virtual void InitPickManipulator();
	//��������ͼ���� ��pUserRootDataָ��ΪGraphicsGroup���û�ͼ������
	virtual void CreateSreen(){;}

	/*
	��ʼ����ʾ�����û���Чͼ�ζ���rootָ��ΪvalidGraph�����û������������ͼ�ζ��󣬿ɲ�ʵ��
	*/
	virtual void InitGraph(osg::ref_ptr<osg::Group>& pNode){;}

	 

	void LoadFromXML( CMenu& menu , const std::wstring& strType);
	virtual void OnRButtonDown();
	virtual bool MenuItemIsValid(const std::wstring& strMenuItem);
 

	std::vector<CDbObjInterface*> FindObjByType(const CString& objType);

	std::vector<CString> GetObjTitlesByType(const CString& objType);

	//�������Ͳ������еĶ��� ������صģ�������
	std::vector<CDbObjInterface*> FindObjByTypes(std::vector<CString>& objTypes);

	//ͨ���������ж϶����Ƿ����
	BOOL  TitleisValid(const CString& objType, CString& strTitle);

	//û��ѡ���κζ���ʱ�����Ա����ʾ����
	virtual CDbObjInterface* GetIdleObj(){ return NULL; }

	void DeleteObjectNow();
	void AddObjectNow();
	void UpatateLocalMatrix();

	virtual void setCameraManipulator(osgViewer::View* pView);


	CString GetDefaultTitle(const CString& strType);


	BOOL RepeatLastCommand();

	void SelectChanged();

	void setUpdateCallback(osg::NodeCallback* pCallBack);
public:
	BOOL          bShowGrid;
	osg::ref_ptr<osg::Group> grids;	

	BOOL          bShowAxis;
 
	osg::ref_ptr<osg::Group> hudAxis;
	LoadManager T_setup;
	double osgZ;

	osg::Vec3 coordPos;  //��ǰ����

	int optimizationOptions; //�Ż�ѡ��

	//
	double zoomFactor;
	BOOL bFirst;
	BOOL bInit;
	BOOL bNeedFullSceen;
	int bNeedChangeViewType;
 	BOOL bNeedOptimize;
	BOOL m_bOptimize;
	std::map<CString , int > objSizeMap;           //first Ϊ���� �� sencond Ϊ���ִ�С
	std::map<CString , int > objTextSizeMap;
	std::map<CString , osg::Vec4 > objColorMap;

	//�û������֧�ֲ�׽����
	std::vector<CString> userDefineSnapObjType;
	////
	bool bfactor; //�ӿڴ�С�Ǿ��Դ�С��������Ա����� 
	 

	viewSet mViewSet;
	///////
	osgViewer::View* mView;

	osgViewer::ViewerBase* mViewer;
	CDbOSGManagerInterface*      m_Osg;
	CView*                    mCView;


	bool m_bOffset;//�Ƿ���ʾ�Ҳ�ָ���ͼ
	double m_dOffset;//�Ҳ�ָ���ͼ�ĺ�����
	double GetOffset();
	void SetbOffset(bool bOffSet);


	CString strCoord;

	osg::Vec4 pixelSize;

	bool bRotate;     //ѡת��־��

	osg::Vec3  lightposition;

	bool bCancle;

	osg::Matrix* GetWorldMatrix();
	osg::Matrix m_worldMatrix;



	mutable std::vector<SNAPINFO>  m_IntersectionSnapPoints;//�����߽��㻺��
	void UpdateSNAPINFOWhenDeleteObj(DWORD uID);
	virtual void UpdateSNAPINFOWhenAddObj(DWORD uID);
	void UpdateSNAPINFOWhenModifyObj(DWORD uID);

	//�ⲿ֧�ֲ�׽����
	void ModiftSnapType(const CString& objType, BOOL bAdd);

	virtual void RemoveSelectNodeEffect(osg::Node* pNode);

	//�ƶ�Ԥ���� ��ͬʱѡ�нڵ�/����/������ʱ ֻ��Ҫ�ƶ�������
	virtual void OnPreMove(std::vector< CDbObjInterface*>& selectetSetMap, osg::Vec3d& offfset){ ; }

	std::vector<std::pair<CString, CString> > strCommandMap;//�����ע��

 
	std::vector<CString> needSelectObjects;

	property_list_MultiObject multiObject;

	virtual CUIBaseData* FindUIData(DWORD id, CString strRTName){ return NULL; }


	virtual void PreStretch();
	virtual CDbObjInterface* PreTrim(CDbObjInterface* pData);
	virtual void PostTrimNew(CDbObjInterface* pData, const CString& strOldTitle);

	int m_operationFlag;

	//
	bool cameraAnimation;
	double animationFactor;

	osg::Quat rotation1;
	osg::Quat rotation2;

	osg::Quat rotationUp1;
	osg::Quat rotationUp2;
	bool upValid;
	double m_radius;
	osg::Vec3 m_center;
	osg::Vec3 m_up;
	osg::Vec3 m_lookDir;
	void OnCameraAnimation();

	virtual void ChangeViewByType(const osg::Vec3& center, double radius, const osg::Vec3& lookDir, const osg::Vec3& up, osgViewer::View* mView);
	
	void CreateAxis2(osg::ref_ptr<osg::Group> rootNode);

	static CDbHandlerInterface* GetGlobalHandler();
	static void SetGlobalHandler(CDbHandlerInterface* pHandler);

	double axisSize;
	static CDbHandlerInterface* g_Handler;
public:
	 float GetGLVersion();
	 float GetGLSLVersion();
	 bool ShaderIsSupport();

 public:
	 void SetCurCroodSys(const CString& strCur);
	 CString GetCurCroodSys();
	 void SetLastCroodSys(const CString& strCur);
	 CString GetLastCroodSys();
 public:
	 virtual void OnIdle();
	 //�������Ƿ��޶���
	 virtual BOOL SceenIsEmpty();


 };
 
  

//��ѡ������
class ClearSelectionNodeVisitor : public osg::NodeVisitor
{
public:
	ClearSelectionNodeVisitor(CViewControlData* pOSG): NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_pOSG(pOSG)
	{	  }
	virtual void apply( osg::Node& node );
	CViewControlData* m_pOSG;
};

 

//��
class MyPickVisitor : public osgUtil::IntersectionVisitor
{
public:
	MyPickVisitor(osgUtil::Intersector* intersector) :osgUtil::IntersectionVisitor(intersector)
	{}
	virtual void apply(osg::Group& group); 
};


DB_3DMODELING_API void FinishCommand();
