#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>

#include <osgUtil/Optimizer>
#include <string>

#include "ViewControlData.h"
#include "osgInterface.h"

#include "osgtools.h"

class CDbPickModeHandler;
class CDbBoxSelector;
class CDbOSGDataCenter;

//MFC窗口类和OSG的接口类
class DB_3DMODELING_API CDbOSGManager : public CDbOSGManagerInterface
{
public:
	CDbOSGManager(CView* pView );
	virtual ~CDbOSGManager(); 
	//初始化osg
	void InitOSG(CViewControlData* pControlData );
	//初始化操纵器
	void InitViews(void);
	//初始化相机
	void SetupWindow();

	//键盘输入命令，响应
	bool OnCommand(const CString& strCommand);
	void OnCommandUpdate(const CString& strCommand, bool& enable, bool& checked);

	virtual CViewControlData* FindControlData(const CString& strTitle=""){return m_pControlData;}
	virtual osgViewer::View* GetCurrentView(){return mViewer;}
	virtual osgViewer::ViewerBase* GetCurrentViewer(){return mViewer;}

 
	void OnUpdateDBObject(const CString &strObjectPath, int modifyType, const std::vector<CString> &vecVariable = std::vector<CString>(0));


private:
	bool mDone;
	int width;
	int height;
protected:
 	osgViewer::Viewer*  mViewer;
	CViewControlData* m_pControlData;
 };
