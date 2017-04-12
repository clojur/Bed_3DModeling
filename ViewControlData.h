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


//光标样式
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

//橡皮筋线
struct rubbleLine
{
	osg::Vec2 p1;
	osg::Vec2 p2;
	osg::Geometry* pGeometry;
};

//橡皮筋矩形
struct rubbleBox
{
	osg::Vec2 p1;
	osg::Vec2 p2;     //矩形 ，园
	osg::Vec2 p3;    //椭圆用
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

struct SNAPINFO //交点
{
	DWORD firstNo;
	DWORD secondNo;
	std::vector<osg::Vec3d> intersectionArr;
};


/////辅助类
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

//三维图接口
class DB_3DMODELING_API CViewControlData
{
public:
	CDbOSGDataCenter*              m_pUserData;	            //图形数据中心
	CDbBoxSelector*                       m_pSelector;         //选择工具
	CDbHandlerInterface*          m_pCurHandler;            //当前命令对象
	CDbPickModeHandler*              m_pPickModelHandler;  //交互工具
public:
	int       viewType;                          //视图类型 ，1-3： 2D投影  4透视图   ,  9 自动, 10用户坐标系
	osg::Vec3 localOrg;
	osg::Vec3 localXDir;
	osg::Vec3 localYDir;
private:
	CString   strCurCoordSys;
	CString   strLastCoordSys;
public:
	BOOL bEnableSnap;
	DWORD   m_snapType;            // 栅格捕捉， 节点捕捉， 单元中点捕捉； 
	int   m_command;       //当前状态，是否有命令启动
	int   manipulatorType; //当前操作类型 ， 1平移，2 旋转 ，3缩放，0默认 选择 4改变窗口大小？
	int   selectStatus;        //1 选中后显示包围框， 2取消选中， 删除包围框，3 默认
	BOOL m_bOrtho;                                      //正交模式；
	osg::Vec4 backColor;                            //背景颜色
	osg::Vec4 offsetBkColor;					//右侧视图背景颜色
	bool bParalled;                                     //是否平行投影
	BOOL bNeedClearSelect;                     //是否需要清除选择集,0，不清除选择集 ，1清除选择集 2 先纪录上次选择对象，再清空选择集，再选择上次对象（相当于刷新属性表）

	CString m_showType;

	BOOL m_bInstallValid; //是否及时刷新 

	int unitFactor;    ///建模的单位比例，1为mm ， m为1000；


	int  bReDrawCount;                                   //渲染是否挂起
	osg::ref_ptr<osg::Group> mRoot;                //总根节点
	osg::ref_ptr<osg::Group> GraphicsGroup;   //有效图元总结点
	osg::ref_ptr<osg::Group> tempObjs;           //临时对象总结点

	osg::ref_ptr<osg::Group> StretchHandlers;  //夹点对象根节点
	osg::ref_ptr<osg::Group> SelectEffectRoot;     //选中对象效果根节点
	osg::ref_ptr<osg::Group> HeightLightEffectRoot;     //高亮对象效果根节点
	osg::ref_ptr<osg::Group> validGraph;           //用户的图形根节点，和mRoot的区别 ，不包含coordObj ， 背景表格,临时对象； 否则包围框计算会错误；
	osg::ref_ptr<osg::Group> allText;                  //所有的文字

	osg::ref_ptr<osg::Group> pTerrain;                  //地形

	osg::ref_ptr<osg::Group> snapGraph;                //捕捉点

	osg::ref_ptr<osg::Group> mOffsetViewRoot;                //附属视图总根节点


	osg::ref_ptr<osg::Group> dimObjs;  //标注对象


	BOOL       bClearSnap;     //是否清除捕捉点
	BOOL       bUpdateSnap;     //是否更新捕捉点
	int            snapValidType; //捕捉点类型
	osg::Vec3 snapPos;         //捕捉点位置

	//添加的
	CMutex m_AddOverLayoutMutex;
	std::vector<osg::Node*> overLayoutObjects;  //临时对象 : 橡皮筋,选择框
	std::mutex m_AddTempMutex;
	std::vector<osg::Node*> tempObjects;           //用户产生的临时对象；

	//需要删除的
	CMutex m_DelOverLayoutMutex;
	std::vector<osg::Node*> delOverLayoutObjects; //临时对象 : 橡皮筋,选择框

	CMutex m_DelTempMutex;
	std::vector<osg::Node*> delTempObjects;           //用户产生的临时对象；

	//////////////////////////////////////////////////////////////////////////////
	CMutex m_AddMutex;
	CMutex m_DelMutex;
	CMutex m_UpdateMutex;
	std::map<CDbObjInterface*, std::vector<osg::Node*>>  updateObjects;           	//需要修改的对象
	std::map<CDbObjInterface*, std::vector<osg::Node*> > addObjects;           //用户产生的非临时对象；
	std::vector<osg::Node*>     delObjects2;           //用户产生的非临时对象；


	osg::Node* pRubBoxCamera;
	osg::Geometry* pRubBoxGeometry;
	void UpdateRubbox( osg::Vec2 p1, osg::Vec2 p3);
	void InitRubbox(osg::Vec2 p1, osg::Vec2 p3);
	void SetRubboxVisible(BOOL bVisible);

 
	std::vector<rubbleLine> rubbleline;//橡皮筋线
	std::vector<rubbleBox> rubblebox;//橡皮矩形
	std::vector<rubbleBox> rubbleCircle;//橡皮矩形
	std::vector<rubbleBox> rubbleEllipse;//橡皮矩形

	//////包围框
	std::vector<CString> addBoundingBox;                 //需要添加包围框的对象
	std::vector<CString> removeBoundingBox;           //需要移除包围框的对象

	//扑捉排除对象
	std::vector<CString> snapExincludes;

	//克隆出来的临时对象
	std::vector<CDbObjInterface*> delCloneObjes;


	int MINRADIUS;
	int defaultRaidus;

	double girdFactor;

	osg::Texture2D* GetPointTexture(int ptType); //	int ptType; //0函数关键点 1.支座位置 2 拉索锚固 3钢束参考线点 4应力验算 5 有效宽度点,6对齐点

	osg::Texture2D* GetFunctionPtTexture(osg::ref_ptr <osg::Texture2D>& pTextTure, const std::string& strTexturePath);
 

	osg::ref_ptr <osg::Texture2D> pointTexture;
	osg::ref_ptr <osg::Texture2D> zzPtTexture;
	osg::ref_ptr <osg::Texture2D> mgPtTexture;
	osg::ref_ptr <osg::Texture2D> steelPtTexture;
	osg::ref_ptr <osg::Texture2D> checkPtTexture;
	osg::ref_ptr <osg::Texture2D> vwPtTexture;
	osg::ref_ptr <osg::Texture2D> allignPtlTexture;

	osgText::Font* GetEnTextFont(); //正方形点 texture
	osgText::Font* GetCnTextFont(); //正方形点 texture


	osgText::Font* GetEnTextFontForColor(osg::Vec4 color); //正方形点 texture
	osgText::Font* GetCnTextFontForColor(osg::Vec4 color); //正方形点 texture


	osg::ref_ptr<osgText::Font>      textFontEn;
	osg::ref_ptr<osgText::Font>      textFontCn;
	CString strFontFileEn;
	CString strFontFileCn;

	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >     colorTextFontEnMap;
	std::map<osg::Vec4, osg::ref_ptr<osgText::Font> >     colorTextFontCnMap;

	//
	BOOL bReforceUpdateCallBack;


	BOOL m_bNeedChangeShowType;           //是否切换了显示模式 ，回调函数中使用；
	virtual void OnChangeShowType(){ ; }
	virtual void OnMenuCommand(UINT id){ ; }
	//copy前处理
	virtual std::vector<CDbObjInterface*> OnPreCopy(const std::map<osg::Node*, CDbObjInterface*>& selectetSetMap);

	//开始操作
	virtual void beginOperation(const CString& strOperation);
	//结束操作
	virtual void endOperation();
	//直接修改
	virtual void doOperation(const CString& strOperation,CDbObjInterface* pData, BOOL bUpdateDB=TRUE);

public:
	CViewControlData();
	virtual ~CViewControlData();

	void Init();
	/*
	根据内部的路径名称查找对象，适用程序内部
	*/
	CDbObjInterface* FindObj(const CString& objName, const CString& type);
	CDbObjInterface* FindObjByID(const DWORD& objID, const CString& type);
	CDbObjInterface* FindObjByTitle(const CString& objTitle, const CString& type);

	/*
	对象增加
	*/
	virtual void addData(CDbObjInterface* pData, BOOL bUpdateDB = TRUE);
	virtual void addDataOperation(const CString& strOperation, CDbObjInterface* pData, BOOL bUpdateDB = TRUE);

	/*
	对象删除
	*/
	virtual void preDelData(std::vector<CDbObjInterface*>& pData);
	virtual void delData(std::vector<CDbObjInterface*> pData, BOOL bUpdateDB = TRUE);
	virtual void postDelData(std::vector<CDbObjInterface*> pData, BOOL bUpdateDB = TRUE);
	/*
	对象更新
	*/
	virtual void updateData(CDbObjInterface* sectionData, BOOL bUpdateDB = TRUE);


	/*
	对象能否被删除
	*/
	virtual bool canBeDel(CDbObjInterface* pData){ return true; }

	/*
	设置新命令
	*/
	void SetHandler(CDbHandlerInterface* pHandler);


	/*
	获得当前选中对象数目
	*/
	int GetSelectObjCount();

	/*
	返回当前选中对象名称
	*/
	CString GetSelectNodeName();


	//返回当前选择对象
	std::vector<CDbObjInterface*> GetCurSelectObject();
	/*
	对象是否被选择
	*/
	BOOL IsSelected(const CString& strObj);

	BOOL IsHeightLight(const CString& strObj);

	/*选择后处理
	*/
	virtual void PostObjSelected(osg::Node* pNode);
	virtual void PostObjDClick(osg::Node* pNode);


	/*
	获取窗口大小
	*/
	void GetWindowSize(double& dx, double& dy);

	void GetOffsetWindowSize(double& dx, double& dy);

	/*
	获取鼠标当前位置，窗口坐标
	*/
	CPoint GetCursorPos();

	/*
	根据屏幕坐标来计算世界坐标
	*/
	osg::Vec3d getWorldPos(double dx, double dy, BOOL bSnap);
	osg::Vec3d getWorldPos(osg::Vec2& windPT, BOOL bSnap){ return getWorldPos(windPT.x(), windPT.y(), bSnap); }

	/*
	根据世界坐标来计算屏幕坐标
	*/
	osg::Vec2 getWindosPos(double dx, double dy, double dz);
	osg::Vec2 getWindosPos(const osg::Vec3d&wordPT){ return getWindosPos(wordPT.x(), wordPT.y(), wordPT.z()); }

	/*
	对象扑捉
	*/
	virtual BOOL getSnapPT(osg::Vec3d& inPutPT, const osg::Matrixd&VPW, const osg::Matrixd& InverseVPW);

	/*
	获取对象父节点
	*/
	virtual osg::Group* GetDataParent(CDbObjInterface* pData);

	//用户临时对象管理
	void addTempObject(osg::Node* pNode);
	void clearTempObject(osg::Node* pNode);
	//用户临时对象管理
	void addOverLayoutObject(osg::Node* pNode);
	void clearOverLayoutObject(osg::Node* pNode);

	//包围框对象
	void addBoundingBoxObj(const CString& strObj);
	void clearBoundingBoxObj(const CString& strObj);

	//需要根据数据刷新的对象
	void addUpdateObject(CDbObjInterface* pData, osg::Node* pNode);
	void removeUpdateObject(CDbObjInterface* pData);//zmz


	//需要刷新dim的对象
	std::vector<CDbObjInterface*> updateDimObjects;
	//刷新标注
	void addUpdateDim(CDbObjInterface* pData);

	//需要根据数据刷新的对象
	void addObject(CDbObjInterface* pData, osg::Node* pNode);
	//需要根据数据刷新的对象
	void delObject(osg::Node* pNode);

	//橡皮筋更新
	void addRubbline(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbbox(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbCircle(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p3);
	void addRubbEllipse(osg::Geometry* pGeomory, osg::Vec2 p1, osg::Vec2 p2, osg::Vec2 p3);

	/*
	清空子对象
	*/
	void clearAllChild(osg::Group* pNode);
	//
	void RemoveName(CDbObjInterface* pData);
	/*
	框选放大
	*/
	void BoxZoom(osg::Vec2 pt1, osg::Vec2 pt2);

	/*
	全屏,将全屏标志压入回调函数
	*/
	void DoFullSceen();

	void DoMax();

	void DoDelView();



	//实先全屏函数
	void FullSceenImpl();

	//显示切换, 根据视图类型
	void ChangeViewType();
 	/*
	点选择对象
	*/
	osg::Node* pickSingle(double x, double y, osgViewer::View* viewer, BOOL _usePolytopeIntersector, BOOL _useWindowCoordinates, osg::Vec3& insectPoint);

	/*
	框选对象
	*/
	std::vector<osg::Node*> BoxPick(float mx0, float my0, float mx1, float my1, osgViewer::View* viewer,BOOL bReserveSelect);

	/*
	返回选中对象选中夹点数目
	*/
	int GetSelectHotPointCount();

	/*
	返回对象夹点坐标
	*/
	std::vector<int> GetStretchIndex(const CString& strObj);

	void SetEnableSnap(BOOL flag);

	//顶视图，三维投影
	void TopView();

	//左视图
	void LeftView();
	//前视图
	void FrontView();

	//二维投影视图
	void TopView2();
	void LeftView2();
	void FrontView2();

	void AutoLayout();
	// 自定义局部坐标系 2D
	void LocalView();

	//透视投影视图
	void PerspectiveView();

	/*
	快捷键
	*/
	virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	/*
	命令行启动命令处理
	*/
	virtual bool OnCommand(const CString& strCommand);
	virtual	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool bInCommand();
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);

	virtual void InitCommandString();
	/*
	动态更新栅格
	*/
	virtual void UpdateGrid();

	void SetBackColor(const osg::Vec4& color);
	void SetOffsetViewBkColor(const osg::Vec4& color);
	/*
	选择是否清除上次的选择结果
	*/
	bool SetSelectFlag(bool bClearPreSel);

	/*
	获取当前视图
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

	//增加高亮显示效果 ， 鼠标移动到对象身上时；
	void  addHightLigntEffect(osg::Node* pGeode, osg::Vec4 color);
	//删除高亮显示效果 ， 鼠标移走是；
	void removeHightLigntEffect(osg::Node* pGeode);


	//设置文字大小
	void SetTextSize(const CString& type, int size);

	//返回文字大小
	int  GetTextSize(const CString& type);

	//设置文字大小
	void SetObjSize(const CString& type, int size);

	//返回文字大小
	int  GetObjSize(const CString& type);

	//初始化文字大小
	virtual void InitSize(){ ; }

	//设置文字大小
	void SetObjColor(const CString& type, osg::Vec4 color);

	//返回文字大小
	osg::Vec4  GetObjColor(const CString& type);

	//初始化物体颜色大小
	virtual void InitColor(){ ; };

	virtual void InitVisible(){ ; };

	virtual double GetNameTxSize(){ return 1500; };
public:
	//显示、隐藏表格
	virtual void ShowGrid();

	//显示、隐藏坐标轴
	void ShowAxis();

	//栅格捕捉
	void GridSnap(const osg::Vec3& resPT,osg::Vec3& snapPT);

	/*
	正交扑捉
	*/
	BOOL OrthoPoint(osg::Vec3d& resPt, osg::Vec3d &point, const osg::Matrixd& VPW, const osg::Matrixd& InverseVPW);

	//
	void setManipulator(int viewType , osg::ref_ptr<osg::Group> pNode, osgViewer::View* pView,int width);
	//把Data从应用程序数据库中删除 ，
	virtual void ImplementDelDatas(std::vector<CString>& objNames);

	/*
	外界对象更新
	*/
	virtual void OnUpdateDBObject(const CString &strObjectPath, int modifyType, const std::vector<CString> &vecVariable = std::vector<CString>(0));

	void InitAxis();

	/*
	设置视口大小 , 工具函数
	*/
	void SetViewPort( int type, int vptSpace ,double width , double height);

	/*
	设置当前光标
	*/
	void SetCursorType(int type);

	/*
	初始化场景数据
	*/
	virtual void InitSceneGraph();
	virtual void InitPickManipulator();
	//创建场景图对象 ，pUserRootData指针为GraphicsGroup，用户图形数据
	virtual void CreateSreen(){;}

	/*
	初始化显示其他用户有效图形对象，root指针为validGraph，如果没有其他非数据图形对象，可不实现
	*/
	virtual void InitGraph(osg::ref_ptr<osg::Group>& pNode){;}

	 

	void LoadFromXML( CMenu& menu , const std::wstring& strType);
	virtual void OnRButtonDown();
	virtual bool MenuItemIsValid(const std::wstring& strMenuItem);
 

	std::vector<CDbObjInterface*> FindObjByType(const CString& objType);

	std::vector<CString> GetObjTitlesByType(const CString& objType);

	//根据类型查找所有的对象， 如果隐藏的，不返回
	std::vector<CDbObjInterface*> FindObjByTypes(std::vector<CString>& objTypes);

	//通过标题来判断对象是否存在
	BOOL  TitleisValid(const CString& objType, CString& strTitle);

	//没有选择任何对象时，属性表的显示对象
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

	osg::Vec3 coordPos;  //当前坐标

	int optimizationOptions; //优化选项

	//
	double zoomFactor;
	BOOL bFirst;
	BOOL bInit;
	BOOL bNeedFullSceen;
	int bNeedChangeViewType;
 	BOOL bNeedOptimize;
	BOOL m_bOptimize;
	std::map<CString , int > objSizeMap;           //first 为类型 ， sencond 为文字大小
	std::map<CString , int > objTextSizeMap;
	std::map<CString , osg::Vec4 > objColorMap;

	//用户定义的支持捕捉对象
	std::vector<CString> userDefineSnapObjType;
	////
	bool bfactor; //视口大小是绝对大小，还是相对比例； 
	 

	viewSet mViewSet;
	///////
	osgViewer::View* mView;

	osgViewer::ViewerBase* mViewer;
	CDbOSGManagerInterface*      m_Osg;
	CView*                    mCView;


	bool m_bOffset;//是否显示右侧分隔视图
	double m_dOffset;//右侧分隔视图的横向宽度
	double GetOffset();
	void SetbOffset(bool bOffSet);


	CString strCoord;

	osg::Vec4 pixelSize;

	bool bRotate;     //选转标志，

	osg::Vec3  lightposition;

	bool bCancle;

	osg::Matrix* GetWorldMatrix();
	osg::Matrix m_worldMatrix;



	mutable std::vector<SNAPINFO>  m_IntersectionSnapPoints;//辅助线交点缓存
	void UpdateSNAPINFOWhenDeleteObj(DWORD uID);
	virtual void UpdateSNAPINFOWhenAddObj(DWORD uID);
	void UpdateSNAPINFOWhenModifyObj(DWORD uID);

	//外部支持捕捉类型
	void ModiftSnapType(const CString& objType, BOOL bAdd);

	virtual void RemoveSelectNodeEffect(osg::Node* pNode);

	//移动预处理 ，同时选中节点/构件/控制线时 只需要移动控制线
	virtual void OnPreMove(std::vector< CDbObjInterface*>& selectetSetMap, osg::Vec3d& offfset){ ; }

	std::vector<std::pair<CString, CString> > strCommandMap;//命令和注释

 
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
	 //场景中是否无对象
	 virtual BOOL SceenIsEmpty();


 };
 
  

//框选访问器
class ClearSelectionNodeVisitor : public osg::NodeVisitor
{
public:
	ClearSelectionNodeVisitor(CViewControlData* pOSG): NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_pOSG(pOSG)
	{	  }
	virtual void apply( osg::Node& node );
	CViewControlData* m_pOSG;
};

 

//求交
class MyPickVisitor : public osgUtil::IntersectionVisitor
{
public:
	MyPickVisitor(osgUtil::Intersector* intersector) :osgUtil::IntersectionVisitor(intersector)
	{}
	virtual void apply(osg::Group& group); 
};


DB_3DMODELING_API void FinishCommand();
