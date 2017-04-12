#pragma once
//属性表数据类
class DB_3DMODELING_API CDb3DPropertyData
{
public:
	CDb3DPropertyData();
	~CDb3DPropertyData();
public:
	std::vector<CDb3DPropertyData> m_children;
public:
	CString m_strCaption;
	CString m_dataType;
	CString m_dataValue;
};