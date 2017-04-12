#pragma  once

#include "DbHandlerInterface.h"
#include "osg/vec3"
#include "osg/Node"
#include "osg/Geometry"
 
class CViewControlData;
class selector_interface;
/*
框选功能对象
*/


class DB_3DMODELING_API  CDbBoxSelector
{
public:
	CDbBoxSelector(CViewControlData* pOSG);
 
	osg::Vec3 p11; //基点
	osg::Vec3 p22; //第二个点

	osg::Vec2 startPT; //起点，窗口坐标
	osg::Vec2 endPT;//橡皮筋终点，窗口坐标

	double x1;
	double y1;


 	CViewControlData* m_pOSG;
	int status;
	int selectType; // 1 ，单次框选选择 ， 2多次框选循环选择,   8单次点选 16 循环夹点选择 

	std::vector<CString> m_selectFilter;
	std::map<CString, osg::ref_ptr<osg::Node>> selectetSet;
	std::map<CString, osg::ref_ptr<osg::Node>> selectetSetBak;

	std::map<CString, std::vector<int> > selectObjStretchIndex;//选中对象的选中的捕捉点
public:
	void finish();
	//refacor wr
	void begin(const CString& strProp = "");
	//void begin(const CString& strProp = "", selector_interface* pWnd = NULL);
	void begin(int type, const CString& strProp = "");

	bool IsFinish();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	bool IsSelected(const CString& strObj);
	bool addToSet(CString tempSelectObjName, osg::Node* tempSelectObj);
	void clearSelectSet();
	void onlyClearSelectSet();
	void getSelectSet(std::map<CString, osg::ref_ptr<osg::Node>>&);
	void GetCloneSelectObj(std::vector<osg::Node*>& tempObj, int type); // type 0 default , 1 move , 2 copy
	void GetCloneStretchObj(std::vector<osg::Node*>& tempObj, int type); // type 0 default , 1 move , 2 copy

	CString GetSelectNodeName();
	int GetSelectNodeCount();

 	osg::ref_ptr<osg::Node> GetLastSelectNode(const CString& strObj);

	int GetSelectHotPointCount();
	std::vector<int> GetStretchIndex(const CString& strObj);

	std::vector<osg::Node*> UpdateStretchHandler(osg::Vec2 p1, osg::Vec2 p2);
	BOOL  bInStretchHandler(osg::Vec2 p1, osg::Vec2 p2);

	//
	bool DoSinglePick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

	//设置选择过滤器
	void setSelectFilter(std::vector<CString> selectFilter);
	void clearSelectFilter();

	bool ApplySelect(osg::Node* pNode);
	bool ApplySelect(CDbObjInterface* pData);
	bool ApplySelect(osg::Node* pNode, BOOL bBox);

	// 从选择集中查找指定类型对象
	std::vector<CDbObjInterface*> GetSpeSelectObjects(const CString& objType);
	std::vector<CDbObjInterface*> GetSpeSelectObjects(std::vector<CString> selectFliter);

	static double GetX(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);
	static double GetY(const osgGA::GUIEventAdapter& ea,osgViewer::View* viewer);

	std::vector<osg::Node*> GetAllSelectNode();

	//
	std::vector<osg::Node*> stretchNodes; 
};

/*
框选命令
*/
class DB_3DMODELING_API  CDbBoxSelectorHandler:public CDbHandlerInterface
{
public:
	CDbBoxSelectorHandler(CViewControlData* pOSG):CDbHandlerInterface(pOSG)
	{
		status=0;
		pCamera=NULL;
		pGeometry=NULL;
	}
	float _mx,_my;
	osg::Vec3 p11;
	osg::Vec3 p22;
	osg::Node* pCamera;
	osg::Geometry* pGeometry ;

	void finish();
	void begin();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
};