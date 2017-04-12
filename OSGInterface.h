#pragma  once

class CDbHandlerInterface;
class CDbBoxSelector;
class CDbPickModeHandler;
class CDbHandlerInterface;
class CDbOSGDataCenter;
class CDbOSGManagerInterface;
class CDbOSGBaseView;

#include "osg/Node"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Vec3"
#include <osgText/Font>
#include "osgViewer/Viewer"
#include <osg/MatrixTransform>
#include "OSGObject.h"
#include "SetupScene.h"
#include "ViewControlData.h"

//三维图接口
class DB_3DMODELING_API CDbOSGManagerInterface
{
public: 
	int userCursorType;	
	HCURSOR    panCursor;
	HCURSOR    rotateCursor;
public:
	CDbOSGManagerInterface(CView* pView );
	virtual ~CDbOSGManagerInterface();
	
	virtual CViewControlData* FindControlData(const CString& strTitle=""){return NULL;}
  
	/*
	多窗口时返回鼠标所在窗口，单窗口返回唯一窗口
	*/
	virtual osgViewer::View* GetCurrentView(){return NULL;}
	virtual osgViewer::ViewerBase* GetCurrentViewer(){return NULL;}
	virtual void PreFrameUpdate(void){;}
	virtual void PostFrameUpdate(void){;}

	/*
	全屏,将全屏标志压入回调函数
	*/
	void FullSceen();

	virtual void DoMultiViewMax();
	virtual void DelView(CViewControlData* pControlData);
	virtual void AddView(CViewControlData*);


 
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
	//透视投影视图
	void PerspectiveView();

	 
	/*
	快捷键
	*/
	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	/*
	命令行启动命令处理
	*/
	bool OnCommand(const CString& strCommand);
	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);
	int GetCommandStatus();
	void SetReDraw(bool bReDraw);

	 /*
	 获取当前视图
	 */
	 CView* GetCurrentCWnd();

 	 /*
	 设置当前光标
	 */
	 void SetCursorType(int type);
	 /*
	 返回当前光标
	 */
	 int GetCursorType(){return userCursorType;}
	 
	 HCURSOR GetCursor(int type,std::map<int,HCURSOR>& _mouseCursorMap);
	 void  SetBehavorType(int type);
	 int    GetBehavorType();   

 
public:
	 CView*   m_pParentView;            //对象所在的View 
	 HWND   m_hWnd;
	 bool       done;
	 bool       busedMulsample;
 };

class DB_3DMODELING_API CRenderingThread : public OpenThreads::Thread
{
public:
	CRenderingThread( CDbOSGManagerInterface* ptr );
	virtual ~CRenderingThread();

	virtual void run();

	void Do(osgViewer::ViewerBase* viewer);

protected:
	CDbOSGManagerInterface* _ptr;
public:
	bool _done;
	bool finish;
};

class DB_3DMODELING_API CUpdateCallback : public osg::NodeCallback
{
public:
	CUpdateCallback::CUpdateCallback(CViewControlData* pOSG)
	{
		m_pOSG = pOSG;
	}
	virtual void operator()(osg::Node* pNode, osg::NodeVisitor* nv)
	{		
		UpdateBase();	
		traverse(pNode , nv);
	}
	void UpdateBase();
	CViewControlData* m_pOSG;
};

//高亮访问器
class FindHeightLightNodeVisitor : public osg::NodeVisitor
{
public:
	osg::Node* pNode;
public:
	FindHeightLightNodeVisitor(): NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{	
		pNode = NULL;
	}
	virtual void apply( osg::Node& node );
};



UINT DB_3DMODELING_API Render(LPVOID* ptr);
