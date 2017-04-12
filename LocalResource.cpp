#include "stdafx.h"
#include "LocalResource.h"


CLocalResource::CLocalResource(void)
{
	extern HINSTANCE g_ResourceHandle;

	m_oldResource = AfxGetResourceHandle();

	AfxSetResourceHandle(g_ResourceHandle);

}

CLocalResource::~CLocalResource(void)
{
	AfxSetResourceHandle(m_oldResource);
}