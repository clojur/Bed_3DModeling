#include "stdafx.h"
 
#include "osg/Node"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/linewidth"
#include "osg/PolygonMode"
#include "osg/Material"
#include "osg/LineStipple"
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Quat>
#include <osg/LightModel>
#include <osg/Point>
  
 
#include "osgtools.h" 
#include "ModelingUIData.h"
#include "BaseQiaoObject.h"
#include "string_tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString OSG_Get_App_Path_With_XG()
{
	TCHAR pPath[MAX_PATH];
	::GetModuleFileName(NULL, pPath, MAX_PATH);

	CString strPath(pPath);
	if (strPath.IsEmpty()) return strPath;

	int nPos = strPath.ReverseFind(_T('\\'));
	int nPos2 = strPath.ReverseFind(_T('/'));
	nPos = max(nPos, nPos2);
	if (-1 != nPos) strPath.Delete(nPos + 1, strPath.GetLength() - 1 - nPos);

	return strPath;
}

CDbBaseObject::CDbBaseObject(CViewControlData*pOSG) :CDbObjInterface(pOSG)
{
	m_pUIData = NULL;

 }
CDbBaseObject::CDbBaseObject()
{
	m_pUIData = NULL;
  }

CDbBaseObject::~CDbBaseObject()
{
	if (m_pUIData)
	{
		delete m_pUIData;
		m_pUIData = NULL;
	}
} 

bool CDbBaseObject::GetVisible()
{ 
	return m_pUIData->visible;
};
void CDbBaseObject::SetVisible(bool flag)
{ 
	m_pUIData->visible = flag;
};

CString CDbBaseObject::GetName()
{
	return m_pUIData->strName;
}
void CDbBaseObject::SetName(const CString& newName)
{
	m_pUIData->strName = newName;
}

osg::Vec4  CDbBaseObject::GetColor()
{ 
	return m_pUIData->color;
};
void CDbBaseObject::SetColor(osg::Vec4  flag)
{
	m_pUIData->color = flag;
};

DWORD CDbBaseObject::GetID()
{
	return m_pUIData->id;
};
void CDbBaseObject::SetID(DWORD newID)
{ 
	m_pUIData->id = newID;
};

CString CDbBaseObject::GetTitle()
{ 
	return m_pUIData->strTitle;
};
void CDbBaseObject::SetTitle(const CString& newTitle)
{
	BOOL bFlag = FALSE;
	CString strName(newTitle);

	int ID = 0;
	while (!bFlag)
	{
		bFlag = TRUE;

		 std::vector<CDbObjInterface*>findObjects = m_pOSG->m_pUserData->GetObjectByTitle(strName);
	 
		 for (int j = 0; j <findObjects.size(); j++)
		 {
			 CDbObjInterface* pCurData = findObjects[j];
			 if (pCurData != this)
			 {
				 bFlag = FALSE;
				 break;
			 }
		 }

		if (!bFlag)
		{
			CString strNewName;	
			ID++;
			strNewName.Format("%s%d", newTitle, ID);
			strName = strNewName;
		}
	}

	m_pUIData->strTitle = strName;
};


CString CDbBaseObject::GetRTType()
{
	return m_pUIData->strRTName;
};
void CDbBaseObject::SetRTType(const CString& newTitle)
{
	m_pUIData->strRTName = newTitle;
};



GJLightPosCallback::GJLightPosCallback(CViewControlData* pNode, int type, CUIBaseData* pGJ)
{
	pOSG = pNode;
	m_type = type;
	if (pGJ)
	{
		m_pGJ = pGJ;
		nID = pGJ->id;
		strType = pGJ->strRTName;
	}
	else
	{
		m_pGJ = NULL;
		nID = -1;
		strType = "error";
	}
}

void GJLightPosCallback::operator()(osg::Uniform* uniform, osg::NodeVisitor* nv)
{
	if (m_type == 1)
	{
		uniform->set(pOSG->lightposition);
	}
	else
	{
		m_pGJ =pOSG->FindUIData(nID, strType);
		if (!m_pGJ)
			return; 
		if (m_type == 5)
			uniform->set(m_pGJ->Ka);
		else if (m_type == 6)
			uniform->set(m_pGJ->color);
		else if (m_type == 7)
			uniform->set(m_pGJ->Ks);
		else if (m_type == 8)
		{
			float fValue = m_pGJ->Shininess;
			uniform->set(fValue);
		}
	
	}
}

void MakeLightEffect(osg::Geode* pGJGeode, CViewControlData* pOSG, CUIBaseData* pUIData)
{
	return;

	if (!pOSG->ShaderIsSupport())
	{
		return;
	}
	osg::StateSet* stateset = pGJGeode->getOrCreateStateSet();
	if (stateset)
	{
		osg::ref_ptr<osg::Program> program = new osg::Program;
		std::string strDir = cdb_string_tools::CString_to_string(OSG_Get_App_Path_With_XG());

		std::string vertFile(strDir);
		vertFile.append("shader\\fdn.vert");
		osg::Shader* pVertObj = new osg::Shader(osg::Shader::VERTEX);
		LoadShaderSource(pVertObj, vertFile.c_str());
		std::string fragFile(strDir);
		fragFile.append("shader\\fdn.frag");
		osg::Shader* pFragObj = new osg::Shader(osg::Shader::FRAGMENT);
		LoadShaderSource(pFragObj, fragFile.c_str());
		program->addShader(pVertObj);
		program->addShader(pFragObj);
		stateset->setAttributeAndModes(program.get());

		osg::Vec3 LightPosition(0, 0, 10);
		osg::Uniform* pLightPosition = new osg::Uniform("LightPosition", LightPosition);
		pLightPosition->setUpdateCallback(new GJLightPosCallback(pOSG, 1, pUIData));
		stateset->addUniform(pLightPosition);

		if (pUIData)
		{
			osg::Uniform* pKa = new osg::Uniform("Ka", pUIData->Ka);
			pKa->setUpdateCallback(new GJLightPosCallback(pOSG, 5, pUIData));
			stateset->addUniform(pKa);

			osg::Uniform* pKd = new osg::Uniform("Kd", pUIData->color);
			pKd->setUpdateCallback(new GJLightPosCallback(pOSG, 6, pUIData));
			stateset->addUniform(pKd);

			osg::Uniform* pKs = new osg::Uniform("Ks", pUIData->Ks);
			pKs->setUpdateCallback(new GJLightPosCallback(pOSG, 7, pUIData));
			stateset->addUniform(pKs);

			float shininess = pUIData->Shininess;
			osg::Uniform* pShininess = new osg::Uniform("Shininess", shininess);
			pShininess->setUpdateCallback(new GJLightPosCallback(pOSG, 8, pUIData));
			stateset->addUniform(pShininess);
		}
		else
		{ 
			osg::Uniform* pKa = new osg::Uniform("Ka", osg::Vec4(1,1,1,1));
 			stateset->addUniform(pKa);

			osg::Uniform* pKd = new osg::Uniform("Kd", osg::Vec4(0, 1, 1, 1));
 			stateset->addUniform(pKd);

			osg::Uniform* pKs = new osg::Uniform("Ks", osg::Vec4(1, 1, 1, 1));
 			stateset->addUniform(pKs);

 			osg::Uniform* pShininess = new osg::Uniform("Shininess", 100);
 			stateset->addUniform(pShininess);
		}
	}
}