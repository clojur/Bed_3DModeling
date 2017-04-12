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
	CString strTitle;       //��������
	CString strName;       //·������
	DWORD id;
	osg::Vec4 color;        //��ɫ
	bool      visible;      //�ɼ���

	CString strRTName;  //��������
public:
	CString strOldName; //������
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