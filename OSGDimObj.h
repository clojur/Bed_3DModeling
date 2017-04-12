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
	���ƶ���
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

	CString   m_dimFlag; //һ����������ж����ע�����磺�ڵ㣺���Ʊ�ע�� ����λ�ñ�ע�� ʩ�����ע���ȵ�

	BOOL m_bReadOnly;//�Ƿ�ֻ��

	double m_dAngle; //���ֱ�ע�Ƕ�

	CString  m_dimName;//��ע����
	CString  m_dimValue;//��עֵ
	CString  m_dimText;//��ע����
	osg::Vec4 m_color;//��ע��ɫ

	//text dim
	osg::Vec3  m_insertPoint;//��עλ��
	double     m_dimTextSize;//��ע���ִ�С
	BOOL       m_bAutoScale;//�Ƿ��Զ�����
	BOOL       m_bEnChart;//�Ƿ�Ӣ���ַ�
	osgText::Text::AlignmentType m_alignment;//���ֶ��뷽ʽ
	//line dim
	std::vector<CString> m_dimLable;//������ע����
	osg::Vec3  m_startPoint;               //��ע���λ��
	osg::Vec3  m_endPoint;                //��ע�յ�λ��
	double     m_dimArrarSize;           //��ע��ͷ��С
	double     m_dimLineLength;        //��ע�߳���
	double m_dimTextOffset;             //��עλ��ƫ��
 };
