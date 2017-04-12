#include "stdafx.h"

#include "modelingUIData.h" 
 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void SerializeVec3(CArchive& ar, osg::Vec3& position)
{
	if (ar.IsLoading())
	{
		ar >> position.x();
		ar >> position.y();
		ar >> position.z();
	}
	else
	{
		ar << position.x();
		ar << position.y();
		ar << position.z();
	}
}

void SerializeVec4(CArchive& ar, osg::Vec4& position)
{
	if (ar.IsLoading())
	{
		ar >> position.x();
		ar >> position.y();
		ar >> position.z();
		ar >> position.w();
	}
	else
	{
		ar << position.x();
		ar << position.y();
		ar << position.z();
		ar << position.w();
	}
}


void SerializeColor(CArchive& ar, osg::Vec4& color)
{
	int nVersion = 1;

	if (ar.IsLoading())
	{
		ar >> nVersion;

		if (nVersion == 1)
		{
			ar >> color.x();
			ar >> color.y();
			ar >> color.z();
			ar >> color.w();
		}
	}
	else
	{
		ar << nVersion;

		ar << color.x();
		ar << color.y();
		ar << color.z();
		ar << color.w();
	}
}





CUIBaseData::CUIBaseData()
{
	pModel = NULL;
	color= osg::Vec4(1,0,0,1);        //颜色
	visible=true;      //可见性
	bMainCable = FALSE;
	Shininess = 100;
	Ka = osg::Vec4(1, 1, 1, 1);
	Ks = osg::Vec4(1, 1, 1, 1);

 }

CUIBaseData::~CUIBaseData()
{

}
void CUIBaseData::Copy(CUIBaseData* other)
{
	strTitle = other->strTitle;
	strName = other->strName;
	strRTName = other->strRTName;
	strOldName = other->strOldName;
	id = other->id;
	pModel = other->pModel;
	color = other->color;
	visible = other->visible;
	bMainCable = other->bMainCable;
	Shininess = other->Shininess;
	Ka = other->Ka;
	Ks = other->Ks;
 }
void CUIBaseData::SetID(DWORD dID)
{
	id = dID;
	strName.Format("%s%d", strRTName, id);
	strTitle = strName;
}

void CUIBaseData::Serialize(CArchive& ar)
{
	int nVersion = 4;

	if (ar.IsLoading())
	{
		ar >> nVersion;
		if (nVersion == 1)
		{
			ar >> strTitle;
			ar >> strName;

			CString strTemp;

			ar >> strTemp;
			ar >> id;
		}
		if (nVersion == 2)
		{
			ar >> strTitle;
			ar >> strName;

			CString strTemp;

			ar >> strTemp;
			ar >> id;

			ar >> visible;

			SerializeVec4(ar,color);

	 
		}
		if (nVersion == 3)
		{
			ar >> strTitle;
			ar >> strName;

			CString strTemp;

			ar >> strTemp;
			ar >> id;
			ar >> bMainCable;

			ar >> visible;

			SerializeVec4(ar,color);

		}
		else if (nVersion == 4)
		{
			ar >> strTitle;
			ar >> strName;

			CString strTemp;

			ar >> strTemp;
			ar >> id;
			ar >> bMainCable;

			ar >> visible;

			SerializeVec4(ar, color);
			SerializeVec4(ar, Ka);
			SerializeVec4(ar, Ks);

			ar >> Shininess;
		}
		 
	}
	else
	{
		ar << nVersion;

		ar << strTitle;
		ar << strName;
		ar << strRTName;
		ar << id;
		ar << bMainCable;

		ar << visible;

		SerializeVec4(ar, color);
		SerializeVec4(ar, Ka);
		SerializeVec4(ar, Ks);

		ar << Shininess;
 	}
}
    
 