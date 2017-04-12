#pragma  once
#include "osg/Vec4"
#include "osg/Vec3"
#include "osg/Node"
#include "Db3DProperty.h"
#include "afxmt.h"
#include "MeshInfo.h"

 class CViewControlData;
class CDbPropertyGridItem;
class CXTPPropertyGridItem;
class CUIBaseData;
class CTHDataBase;
class property_list_MultiObject;

#define OT_DIM "osgDim"

enum db_showMode
{
	sm_component = 0,
	sm_segment = 1,
	sm_element = 2,
};


enum db_changeMode
{
	db_adddata = 0,
	db_deldata = 1,
	db_changedata = 2,
};

enum ORIETATION{ LEFTSIDE = 1, ONCURVE = 0, RIGHTSIDE = -1 };
  

class DB_3DMODELING_API CDbObjInterface : public CDb3DProperty
{
protected:
	CString   m_strTitle;         //���⣬��ʾ���ơ������ظ�
	CString   m_strName;     //�ڲ�Ψһ��ʾ�� �����·�������Ų���������
	DWORD       m_id;
 	CString     m_type;
	osg::Vec4   m_color;
	bool        m_visible;
	bool        m_bClose;
	CDbObjInterface* m_pParentObj;
	CViewControlData*            m_pOSG;
private:
	bool   m_bSelected;
	bool   m_bDrawHandler; //ѡ���Ƿ���Ƽе� 
	std::map<CString, CString>   m_mapUserValue;
	std::map<CString, CDb_PrimitiveInfo*>  m_meshInfo;
public:
	CDbObjInterface();
	CDbObjInterface(CViewControlData* pOSG);
	virtual ~CDbObjInterface();

	void SetViewControlData(CViewControlData* pOSG){ m_pOSG = pOSG; }
	CViewControlData* GetViewControlData(){ return m_pOSG; }
	bool IsClose(){ return m_bClose; };
	bool IsDrawHandler(){ return m_bDrawHandler; };

	virtual bool GetVisible(){ return m_visible; };
	virtual void SetVisible(bool flag){ m_visible = flag; };
	
	virtual osg::Vec4 GetColor(){ return m_color; };
	virtual void SetColor(osg::Vec4 flag){ m_color = flag; };

	virtual CString GetName(){ return m_strName; }
	virtual void SetName(const CString& newName){ m_strName = newName; }

	virtual DWORD GetID(){ return m_id; };
	virtual void SetID(DWORD newID){ m_id = newID; };

	virtual  CString GetTitle(){ return m_strTitle; };
	virtual void SetTitle(const CString& newTitle){ m_strTitle = newTitle; };

	virtual CString GetRTType(){ return m_type; };
	virtual  void SetRTType(const CString& newType){ m_type = newType; };
	
public:

	/*
	���ض���е�
	*/
	virtual std::vector<osg::Vec3d> GetStretchPoint();

	/*
	stretch����
	*/
	virtual bool Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset){ return false; }
 
	/*
	move��������������޸�
	*/
	virtual bool Move( osg::Vec3d& offset ){return false;}


	/*
	 ���ض���;��Σ�p1��P2���Ľ���pt��
	*/
	virtual BOOL HitPositin(osg::Vec3& p1, osg::Vec3& p2, osg::Vec3& pt){ return FALSE; }

	 /*
	 �ü�Mirror
	 */
	 virtual BOOL Mirror(osg::Matrix*) { return FALSE; }

	 /*
	 offset 
	 */
	 virtual bool offset(double dOffset, bool bReal = true);
	 /*
	 �жϵ��ڶ������һ��
	 */
	 virtual ORIETATION Orietation(const osg::Vec3& p){ return ONCURVE; }

	 /*
	 �ö����ܷ���Ϊ�߽磬���ұȽ��ȶ���//����ڵ㲻����Ϊ�߽磨���ȶ�����ӦΪ���������޸ĺ󣬸�ָ�������Ч
	 */
	 virtual BOOL CanBeBorder() { return FALSE; }

	 /*
	 extend
	 */
	 virtual BOOL Extend(double t1, CDbObjInterface* pBorder, BOOL bExtendStat){ return FALSE; }
	

	 /*
	 �ü�Trim
	 */
	 virtual BOOL Trim(double t1, double t2, std::vector< CDbObjInterface*>& result) { return FALSE; }
	 

	 virtual double FirstParameter(){ return 0; };
	 virtual double LastParameter(){ return 1; };
	 virtual double GetParameter(const osg::Vec3& p) { return 0; }


	 /*
	 ������otherObject�Ľ���
	 */
	 virtual BOOL GetCrossPoints(CDbObjInterface* pOtherObject, std::vector<osg::Vec3d>& crossPts, BOOL bExtendSelf = FALSE){ return FALSE; }

	 /*
	 copy������ Ĭ��Ϊִ��Move,������copy�������ܲ�ͬ
	 */
	 virtual bool Copy(osg::Vec3d& offset);       

	 /*
	 ����˫��
	 */
	 virtual BOOL OnDblClick(){ return FALSE; }

	 /*
	 ���У�Ĭ��Ϊִ��copy,������copy�������ܲ�ͬ
	 */
	 virtual bool Array(osg::Vec3d& offset); 

	 /*
	 Ӧ�����Ա��޸�
	 */
	 virtual  bool ApplyProperty(const CDb3DPropertyData& data){ return false; };

	 /*
	 ���ض�������Ա�
	 */
	 virtual  void CreateProperty(CDb3DPropertyData& data){};
public:

	 virtual CDbObjInterface* Clone() = 0;

	 /*
	 ����osgͼ�ζ���Ĭ����osg::Geode,������osg::Group
	 */
	 virtual osg::Node* CreateGraphObject();

 
	 /*
	 ���ض����ѡ��Ч��ͼԪ��bSelect�������Ӹ���Ч������ѡ��Ч��
	 */
	 virtual osg::Node* AddSelectEffect(osg::Node* pSelf, BOOL bSelect = TRUE);   
	 
	 /*
	 ���ѡ��Ч��
	 */
	 virtual void ClearSelectEffect(osg::Node* pSelf, BOOL bSelect = TRUE);             //bSelect����ɾ������Ч������ѡ��Ч��

	 /*
	 ���ʹ����AddMeshInfo���������ݣ��ú�������Ҫ��д
	 */
	 virtual void OnUpdate(osg::Node*  pNode);

	 /*
	 ��ȡ�����Ļ�����ά��Ϣ
	 */
	 void AddMeshInfo(CDb_PrimitiveInfo* oneMesh);
	 
	 /*
	 �����ӱ�ע����
	 */
	 virtual std::vector<CDbObjInterface*> CreateDimObjects();

	 /*
	 �����ӱ�ע�����������
	 */
	 virtual BOOL OnUpdateByDim(const CString& strDimFlag, const CString& strDimName, const CString& strDimValue);

	 /*
	 ��ȡ�Ӷ���ɾ����������Ҫɾ�������Ӷ���
	 */
	virtual std::vector<CDbObjInterface*>             GetSubObjList(BOOL bIncludeSefl);

	/*
	�����Ӷ���
	*/
	virtual std::vector<CDbObjInterface*> CreateSubObjList();
	
	/*
	�����Ӷ����������
	*/
	virtual bool OnUpdateByChild(CDbObjInterface* pChild);

 
	/*
	ͼ�������޸�������޸ĺ�̨���ݿ��е�����
	*/
	virtual void UpdateDBData(db_changeMode changeMode);


	/*ִ���޸Ĳ����� ��OnUpdate���� �� OnUpdateֻ�ı��Լ� �� DoUpdate�ı��Ӷ��󡢹�������
	*/
	virtual void DoUpdate();

	
	/*
	ɾ������ͼ�νڵ�
	*/
	virtual void DeleteObjectNode();
  
	/*
	���ݼ��
	*/
 	virtual void CheckData(){ ; }

	
public:

	/*
	���úͻ�ȡ��������
	*/
	void setUserValue(const CString &strKey, const CString &strValue);
	void getUserValue(const CString &strKey, CString &strValue) const;

	/*
	��ӵ�����ͼ
	*/
	void UpdateAndAddToScene();

	/*
	��ȡ�е����
	*/
	int GetIndex(osg::Vec3& pt);

	/*
	��ȡ��ע�ӽڵ�
	*/
	std::vector<osg::Node*> GetDimNodes();


	void RemoVeDimNodes();
	void AddDimNodes();
	void SetDimVisible(const CString& strDimType, BOOL bVisible); 

	/*
	��ȡ�Ӷ���ڵ�
	*/
	std::vector<osg::Node*> GetChildNodes();
};

////ͼ�ζ�����������
class DB_3DMODELING_API CDbOSGDataCenter
{
public:
	CDbOSGDataCenter( )
	{
	}
	~CDbOSGDataCenter( )
	{
		Clear();
	}
	static int GetID()
	{
		return maxID++;
	}
  
	/*
	ע�⣺�˴������˹������ӱ�ע����
	*/
	void AddObj(CDbObjInterface* pData);

	void DeleteObj(const CString& objName, const CString& type);
	void DeleteObjByID(DWORD id);

	void Clear();

	std::vector<CDbObjInterface*> GetObjectByTitle(const CString& strTitle);

	CDbObjInterface* FindObjByID(const DWORD& objID, const CString& type);
	CDbObjInterface* FindObj(const CString& objName, const CString& type);
	CDbObjInterface* FindObjByTitle(const CString& strTitle, const CString& type);

	std::vector<CDbObjInterface*> FindObjByTypes(std::vector<CString>& objTypes);
	std::vector<CDbObjInterface*> FindObjByType(const CString& objType);
	std::vector<CString>            GetObjTitlesByType(const CString& objType);
 public:
	 CMutex m_dataMutex;
	 std::map<CString, std::vector<CDbObjInterface*>> dataMap;
	static  DWORD maxID;
};
