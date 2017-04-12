#pragma  once
#include "osg/Vec4"
#include "osg/Vec3"
  
 class CDBM_SynModel;

//#define DEFINECURVE 


class DB_3DMODELING_API CUIBaseData
{
public:
	CUIBaseData();
	virtual ~CUIBaseData();
	virtual void Serialize(CArchive& ar);
	virtual void Copy(CUIBaseData*);
	void SetID(DWORD dID);
public:
	CString strTitle;       //标题名称
	CString strName;       //路径名称
	DWORD id;
	osg::Vec4 color;        //颜色
	bool      visible;      //可见性

	CString strRTName;  //类型名称
public:
	CString strOldName; //老名称
	BOOL    bMainCable;
	CDBM_SynModel* pModel;
public:
	virtual void Init(CDBM_SynModel* m_pModel){ ; }
public:
	double Shininess;
	osg::Vec4 Ka;
	osg::Vec4 Ks;
};

void DB_3DMODELING_API SerializeColor(CArchive& ar, osg::Vec4& color);
void DB_3DMODELING_API SerializeVec3(CArchive& ar, osg::Vec3& position);
void DB_3DMODELING_API SerializeVec4(CArchive& ar, osg::Vec4& position);