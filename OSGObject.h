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
	CString   m_strTitle;         //标题，显示名称。可以重复
	CString   m_strName;     //内部唯一标示， 对象的路径，和桥博对象配伍
	DWORD       m_id;
 	CString     m_type;
	osg::Vec4   m_color;
	bool        m_visible;
	bool        m_bClose;
	CDbObjInterface* m_pParentObj;
	CViewControlData*            m_pOSG;
private:
	bool   m_bSelected;
	bool   m_bDrawHandler; //选中是否绘制夹点 
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
	返回对象夹点
	*/
	virtual std::vector<osg::Vec3d> GetStretchPoint();

	/*
	stretch操作
	*/
	virtual bool Stretch(std::vector<int>& stretchIndex, std::vector<osg::Vec3d>& oldPositions, osg::Vec3d& offset){ return false; }
 
	/*
	move操作对象的数据修改
	*/
	virtual bool Move( osg::Vec3d& offset ){return false;}


	/*
	 返回对象和矩形（p1，P2）的交点pt；
	*/
	virtual BOOL HitPositin(osg::Vec3& p1, osg::Vec3& p2, osg::Vec3& pt){ return FALSE; }

	 /*
	 裁剪Mirror
	 */
	 virtual BOOL Mirror(osg::Matrix*) { return FALSE; }

	 /*
	 offset 
	 */
	 virtual bool offset(double dOffset, bool bReal = true);
	 /*
	 判断点在对象的哪一侧
	 */
	 virtual ORIETATION Orietation(const osg::Vec3& p){ return ONCURVE; }

	 /*
	 该对象能否作为边界，并且比较稳定，//例如节点不能作为边界（不稳定），应为所属构件修改后，该指针可能无效
	 */
	 virtual BOOL CanBeBorder() { return FALSE; }

	 /*
	 extend
	 */
	 virtual BOOL Extend(double t1, CDbObjInterface* pBorder, BOOL bExtendStat){ return FALSE; }
	

	 /*
	 裁剪Trim
	 */
	 virtual BOOL Trim(double t1, double t2, std::vector< CDbObjInterface*>& result) { return FALSE; }
	 

	 virtual double FirstParameter(){ return 0; };
	 virtual double LastParameter(){ return 1; };
	 virtual double GetParameter(const osg::Vec3& p) { return 0; }


	 /*
	 计算与otherObject的交点
	 */
	 virtual BOOL GetCrossPoints(CDbObjInterface* pOtherObject, std::vector<osg::Vec3d>& crossPts, BOOL bExtendSelf = FALSE){ return FALSE; }

	 /*
	 copy操作， 默认为执行Move,构件的copy操作可能不同
	 */
	 virtual bool Copy(osg::Vec3d& offset);       

	 /*
	 对象双击
	 */
	 virtual BOOL OnDblClick(){ return FALSE; }

	 /*
	 整列，默认为执行copy,构件的copy操作可能不同
	 */
	 virtual bool Array(osg::Vec3d& offset); 

	 /*
	 应用属性表修改
	 */
	 virtual  bool ApplyProperty(const CDb3DPropertyData& data){ return false; };

	 /*
	 返回对象的属性表
	 */
	 virtual  void CreateProperty(CDb3DPropertyData& data){};
public:

	 virtual CDbObjInterface* Clone() = 0;

	 /*
	 创建osg图形对象，默认是osg::Geode,可能是osg::Group
	 */
	 virtual osg::Node* CreateGraphObject();

 
	 /*
	 返回对象的选择效果图元，bSelect：是增加高亮效果还是选择效果
	 */
	 virtual osg::Node* AddSelectEffect(osg::Node* pSelf, BOOL bSelect = TRUE);   
	 
	 /*
	 清除选择效果
	 */
	 virtual void ClearSelectEffect(osg::Node* pSelf, BOOL bSelect = TRUE);             //bSelect：是删除高亮效果还是选择效果

	 /*
	 如果使用了AddMeshInfo加入了数据，该函数不需要改写
	 */
	 virtual void OnUpdate(osg::Node*  pNode);

	 /*
	 获取构件的基本三维信息
	 */
	 void AddMeshInfo(CDb_PrimitiveInfo* oneMesh);
	 
	 /*
	 创建子标注对象
	 */
	 virtual std::vector<CDbObjInterface*> CreateDimObjects();

	 /*
	 根据子标注对象更新数据
	 */
	 virtual BOOL OnUpdateByDim(const CString& strDimFlag, const CString& strDimName, const CString& strDimValue);

	 /*
	 获取子对象，删除对象是需要删除所有子对象
	 */
	virtual std::vector<CDbObjInterface*>             GetSubObjList(BOOL bIncludeSefl);

	/*
	创建子对象
	*/
	virtual std::vector<CDbObjInterface*> CreateSubObjList();
	
	/*
	根据子对象更新数据
	*/
	virtual bool OnUpdateByChild(CDbObjInterface* pChild);

 
	/*
	图形数据修改了如果修改后台数据库中的数据
	*/
	virtual void UpdateDBData(db_changeMode changeMode);


	/*执行修改操作， 和OnUpdate区别 ， OnUpdate只改变自己 ， DoUpdate改变子对象、关联对象；
	*/
	virtual void DoUpdate();

	
	/*
	删除对象图形节点
	*/
	virtual void DeleteObjectNode();
  
	/*
	数据检查
	*/
 	virtual void CheckData(){ ; }

	
public:

	/*
	设置和获取定义数据
	*/
	void setUserValue(const CString &strKey, const CString &strValue);
	void getUserValue(const CString &strKey, CString &strValue) const;

	/*
	添加到场景图
	*/
	void UpdateAndAddToScene();

	/*
	获取夹点序号
	*/
	int GetIndex(osg::Vec3& pt);

	/*
	获取标注子节点
	*/
	std::vector<osg::Node*> GetDimNodes();


	void RemoVeDimNodes();
	void AddDimNodes();
	void SetDimVisible(const CString& strDimType, BOOL bVisible); 

	/*
	获取子对象节点
	*/
	std::vector<osg::Node*> GetChildNodes();
};

////图形对象数据中心
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
	注意：此处加上了构件的子标注对象
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
