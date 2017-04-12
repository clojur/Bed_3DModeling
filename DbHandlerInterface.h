#pragma once

#include "osgGA/GUIEventAdapter"
#include "osgGA/GUIActionAdapter"
#include "osg/Node"
#include <osgviewer/View>
class CDbObjInterface;
class CViewControlData;

enum CommandSource{OSG_CMDLINE = 1,OSG_LBTNDOWN = 2,OSG_MOUSEMOVE = 4,OSG_LBTNUP = 8};

//�Զ����������
class DB_3DMODELING_API  CDbHandlerInterface
{
public:
	CDbHandlerInterface(CViewControlData* pOSG);
	/*
	�������
	*/
	virtual void finish();
	/*
	��������
	*/
	virtual void begin()=0;
	/*
	�����
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

//��֪���㣬��������ֵ������ֵ��Ӧ���꣬
//strPos��@�Ǵ����������
//stPosû��@���ţ��С������ָ�ʱ����������룬�ޡ���������Ե�ǰ����

void DB_3DMODELING_API GetCurrentPos(osg::Vec3d& p11, osg::Vec3d& p22, CString& strPos, BOOL bRelative = FALSE);

void DB_3DMODELING_API CopyObjectAndAdd(CViewControlData* m_pOSG, CDbObjInterface* pCreateData);

 