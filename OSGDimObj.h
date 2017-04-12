#pragma  once
#include "osg/Vec4"
#include "osg/Vec3"
#include "osg/Node"
#include "osgText/TextBase"
#include "osgText/Text"
 

enum DB_DIMTYPE
{
	TEXT_DIM = 0,
	LINE_DIM = 1,
	CAD_DIM = 2,
};


class DB_3DMODELING_API CDbDimObj : public CDbObjInterface
{
public:
	CDbDimObj(CViewControlData* pOSG, CDbObjInterface* pParent, const CString& dimName, const CString& dimValue, const CString& dimFlag, DB_DIMTYPE dimType);
 public:	
	/*
	绘制对象
	*/
	virtual void OnUpdate(osg::Node* pGraph );

	/*
	clone function
	*/
	virtual CDbObjInterface* Clone();

	/*
	double click
	*/
	virtual BOOL OnDblClick();
	/*
	create graph
	*/
	virtual osg::Node* CreateGraphObject();
      
	//refactor by wangrui
	//virtual void create_property_item(CXTPPropertyGridView& grid);

public:
	CDbObjInterface* GetParentObj();
public:
	CString           m_strParentObjName;
	CString           m_strParentObjType;
	DB_DIMTYPE    m_dimType;

	CString   m_dimFlag; //一个对象可能有多类标注，例如：节点：名称标注， 特征位置标注， 施工缝标注，等等

	BOOL m_bReadOnly;//是否只读

	double m_dAngle; //文字标注角度

	CString  m_dimName;//标注名称
	CString  m_dimValue;//标注值
	CString  m_dimText;//标注文字
	osg::Vec4 m_color;//标注颜色

	//text dim
	osg::Vec3  m_insertPoint;//标注位置
	double     m_dimTextSize;//标注文字大小
	BOOL       m_bAutoScale;//是否自动缩放
	BOOL       m_bEnChart;//是否英文字符
	osgText::Text::AlignmentType m_alignment;//文字对齐方式
	//line dim
	std::vector<CString> m_dimLable;//连续标注文字
	osg::Vec3  m_startPoint;               //标注起点位置
	osg::Vec3  m_endPoint;                //标注终点位置
	double     m_dimArrarSize;           //标注箭头大小
	double     m_dimLineLength;        //标注线长度
	double m_dimTextOffset;             //标注位置偏移
 };
