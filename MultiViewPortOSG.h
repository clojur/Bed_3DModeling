#pragma once

#include <osgViewer/Viewer>
#include <osgviewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <string>

#include "osgInterface.h"


class CViewControlData;

class DB_3DMODELING_API CDbCompositeViewOSG : public CDbOSGManagerInterface
{
public:
    CDbCompositeViewOSG(CView* pView);
    ~CDbCompositeViewOSG();

	void InitOSG( );

    virtual void InitCameraConfig(void);

  
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);

	void DeleteAllView();

	void DoAddView(CViewControlData* oneViewSet,bool bTemp=false);
 
	void AddView(CViewControlData*);
	void DelView(CViewControlData* );

	void SetViewCurrent(osgViewer::View* pCurrentView);

	CViewControlData* FindControlData(const CString& strTitle="");
 
	void OnUpdateDBObject(const CString &strObjectPath, int modifyType, const CString& subTitle, const std::vector<CString> &vecVariable = std::vector<CString>(0));

	virtual osgViewer::View* GetCurrentView();
	virtual osgViewer::ViewerBase* GetCurrentViewer();

	//自动重新计算各个视图的大小
	void ResetSize();

	//splitter
	void ResetViewSize(double cx,double cy,double dx,double dy,bool bVert);


	void OnSize(int cx, int cy);
	//增加边框效果 
	void InitBoundingBoxEffect(CViewControlData* pControlData,osgViewer::View* view);


	//设置视图之间的间隙
	void SetSpace(int nSpace);

	void DoMultiViewMax();

	void AutoCalSize(int nBlockCount , std::vector<viewSet>& newValues);

	osgViewer::View* GetCurosrView(POINT point);
public:
 	osg::Vec4 m_backColor;   // osg背景颜色

    osgViewer::CompositeViewer* mViewer;
	osg::ref_ptr<osg::GraphicsContext> gc;

	std::map<CString , CViewControlData*>  m_ControlDataMap;

public:
	std::vector<CViewControlData*> addViews;
	std::vector<CString> removeViews;

	int width ;
	int height ;

	int vptSpace ;

	//
	bool bFullSceen;

	//new to be max views
	std::vector<CViewControlData*> fullSceenViews;
	//old views'status
	std::map<CString , viewSet> oldViewStatus;

	//删除的视图
	std::vector<CViewControlData*> delViews;

	bool bInit;

	bool bResize;

	int layoutPattern;

	int m_bInit;

 };


class WndOperationHandler : public osgGA::GUIEventHandler
{
public:

	enum TOUCHPOINT
	{
		TP_UNKNOW = 0,
		TP_LEFT,
		TP_RIGHT,
		TP_BOTTOM,
		TP_TOP,
		TP_BOTTOMLEFT,
		TP_TOPLEFT,
		TP_BOTTOMRIGHT,
		TP_TOPRIGHT
	};
	WndOperationHandler(CDbCompositeViewOSG* pOSG ,osgViewer::View*  pView,osgViewer::CompositeViewer* pComView)
	{
		m_pOSG = pOSG;
		_tp = TP_UNKNOW;
		 cx = 0;
		 cy = 0;

 	}
	~WndOperationHandler() {}

	bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	TOUCHPOINT getTouchPoint(float x, float y, osg::Viewport* viewport);
	void SetCursor(TOUCHPOINT tp );

	double GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);
	double GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);

protected:
	CDbCompositeViewOSG* m_pOSG;
	double cx;
	double cy;

	float _x;//鼠标按下时的x
	float _y;//鼠标按下时的y
	float _vx;//鼠标按下时视口的x
	float _vy;//鼠标按下时视口的y
	TOUCHPOINT _tp;//鼠标按下时的位置
};

 