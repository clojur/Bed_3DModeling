// MFC_OSG.cpp : implementation of the CDbOSGManager class
//
#include "stdafx.h"

#include "MeshInfo.h"
#include "serialize.h"
   
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDb_Point3::CDb_Point3()
{
	x = 0;
	y = 0;
	z = 0;
}
CDb_Point3::CDb_Point3(double dx, double dy, double dz) :x(dx), y(dy), z(dz)
{

}
void CDb_Point3::Serialize(CArchive& ar)
{
	int nVersion = 1;

	if (ar.IsLoading())
	{
		ar >> nVersion;

		if (nVersion == 1)
		{
			ar >> x;
			ar >> y;
			ar >> z;
		}
	}
	else
	{
		ar << nVersion;
		ar << x;
		ar << y;
		ar << z;
	}

}


CDb_PrimitiveInfo::CDb_PrimitiveInfo()
{

}

CDb_PrimitiveInfo::~CDb_PrimitiveInfo()
{

}

void CDb_PrimitiveInfo::Serialize(CArchive& ar)
{
	int nVersion = 1;

	if (ar.IsLoading())
	{
		ar >> nVersion;

		if (nVersion == 1)
		{
			ar >> strName;
			ar >> color;
			int type;
			ar >> type;
			meshType = mesh_type(type);
		}
	}
	else
	{
		ar << nVersion;

		ar << strName;
		ar << color;
		ar << meshType;

	}

}


CDb_LineInfo::CDb_LineInfo()
{
	meshType = mt_lines;
	lineWidth = 1;

}

void CDb_LineInfo::Serialize(CArchive& ar)
{
	int nVersion = 1;

	if (ar.IsLoading())
	{
		ar >> nVersion;

		if (nVersion == 1)
		{
			ar >> lineWidth;
			DBSerializeVector(ar, m_Ptsvec);

		}
	}
	else
	{
		ar << nVersion;

		ar << lineWidth;
		DBSerializeVector(ar, m_Ptsvec);
	}
}


CDb_MeshInfo::CDb_MeshInfo()
{

}

void CDb_MeshInfo::Serialize(CArchive& ar)
{
	int nVersion = 1;

	if (ar.IsLoading())
	{
		ar >> nVersion;

		if (nVersion == 1)
		{

		}
	}
	else
	{
		ar << nVersion;

	}
}