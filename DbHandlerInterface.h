#pragma once

#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"
#include "osg/Node"
#include <osgviewer/View>
class CDbObjInterface;
class CViewControlData;

enum CommandSource{OSG_CMDLINE = 1,OSG_LBTNDOWN = 2,OSG_MOUSEMOVE = 4,OSG_LBTNUP = 8};

//自定义操作基类
class DB_3DMODELING_API  CDbHandlerInterface
{
public:
	CDbHandlerInterface(CViewControlData* pOSG);
	/*
	命令结束
	*/
	virtual void finish();
	/*
	命令启动
	*/
	virtual void begin()=0;
	/*
	命令处理
	*/
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)=0;

	CViewControlData* GetOSG(){return m_pOSG;}
	void clearCloneObject();
	void SetConformPoint(osg::Vec3d confirmPT);
	void SetSendPointFlag(int flag)                    {SendPointFlag= flag;}

	double GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);
	double GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);

protected:
	CString strCmdLineInput;
	BOOL m_OldbOrtho;
public:
	CViewControlData* m_pOSG;
	CString strName;
	int status;
	int         SendPointFlag;
	osg::Vec3d lastConfirmPoint;
	BOOL lastConfirmPointBeValid;
	std::map<osg::Node* , CDbObjInterface*> selectetSetMap;
};

//已知基点，根据输入值求输入值对应坐标，
//strPos有@是代表相对输入
//stPos没有@符号，有“，”分割时代表绝对输入，无“，”是相对当前方向

void DB_3DMODELING_API GetCurrentPos(osg::Vec3d& p11, osg::Vec3d& p22, CString& strPos, BOOL bRelative = FALSE);

void DB_3DMODELING_API CopyObjectAndAdd(CViewControlData* m_pOSG, CDbObjInterface* pCreateData);

 