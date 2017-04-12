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

//��άͼ�ӿ�
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
	�ര��ʱ����������ڴ��ڣ������ڷ���Ψһ����
	*/
	virtual osgViewer::View* GetCurrentView(){return NULL;}
	virtual osgViewer::ViewerBase* GetCurrentViewer(){return NULL;}
	virtual void PreFrameUpdate(void){;}
	virtual void PostFrameUpdate(void){;}

	/*
	ȫ��,��ȫ����־ѹ��ص�����
	*/
	void FullSceen();

	virtual void DoMultiViewMax();
	virtual void DelView(CViewControlData* pControlData);
	virtual void AddView(CViewControlData*);


 
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
	//͸��ͶӰ��ͼ
	void PerspectiveView();

	 
	/*
	��ݼ�
	*/
	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	/*
	���������������
	*/
	bool OnCommand(const CString& strCommand);
	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);
	bool pre_do_new_command(const CString& domain, const CString& command, const CString& param);
	int GetCommandStatus();
	void SetReDraw(bool bReDraw);

	 /*
	 ��ȡ��ǰ��ͼ
	 */
	 CView* GetCurrentCWnd();

 	 /*
	 ���õ�ǰ���
	 */
	 void SetCursorType(int type);
	 /*
	 ���ص�ǰ���
	 */
	 int GetCursorType(){return userCursorType;}
	 
	 HCURSOR GetCursor(int type,std::map<int,HCURSOR>& _mouseCursorMap);
	 void  SetBehavorType(int type);
	 int    GetBehavorType();   

 
public:
	 CView*   m_pParentView;            //�������ڵ�View 
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

//����������
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
