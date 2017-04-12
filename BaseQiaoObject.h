#pragma  once

 
#include "osgObject.h"
 

class CUIBaseData;

class DB_3DMODELING_API CDbBaseObject :public CDbObjInterface  //构件对象
{
public:
	CUIBaseData* m_pUIData;
public:
	CDbBaseObject( );
	CDbBaseObject(CViewControlData*pOSG);
	~CDbBaseObject();
public:
	virtual bool GetVisible();
	virtual void SetVisible(bool flag);

	virtual CString GetName();
	virtual void SetName(const CString& newName);

	virtual osg::Vec4  GetColor();
	virtual void SetColor(osg::Vec4  flag);

	virtual DWORD GetID();
	virtual void SetID(DWORD newID);

	virtual CString GetTitle();
	virtual void SetTitle(const CString& newTitle);

	virtual CString GetRTType();
	virtual void SetRTType(const CString& newTitle);

};



class DB_3DMODELING_API GJLightPosCallback : public osg::Uniform::Callback
{
public:
	GJLightPosCallback(CViewControlData* pNode, int type, CUIBaseData* pGJ);

	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv);
public:
	int m_type;
	CViewControlData* pOSG;
	CUIBaseData* m_pGJ;
	DWORD nID;
	CString strType;
};



void DB_3DMODELING_API MakeLightEffect(osg::Geode* pGJGeode, CViewControlData* pOSG, CUIBaseData* pGJ);