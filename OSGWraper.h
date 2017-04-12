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

//MFC�������OSG�Ľӿ���
class DB_3DMODELING_API CDbOSGManager : public CDbOSGManagerInterface
{
public:
	CDbOSGManager(CView* pView );
	virtual ~CDbOSGManager(); 
	//��ʼ��osg
	void InitOSG(CViewControlData* pControlData );
	//��ʼ��������
	void InitViews(void);
	//��ʼ�����
	void SetupWindow();

	//�������������Ӧ
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
