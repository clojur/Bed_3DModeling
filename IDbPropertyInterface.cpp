#include "stdafx.h"
#include "IDbPropertyInterface.h"


IDbPropertyInterface::IDbPropertyInterface()
{
}


IDbPropertyInterface::~IDbPropertyInterface()
{

}
void IDbPropertyInterface::populate(CDb3DProperty* pObj)
{


}


static IDbPropertyInterface*  g_Property = NULL;
void SetDbProperty(IDbPropertyInterface* pProperty)
{
	g_Property = pProperty;
}
IDbPropertyInterface* GetDbProperty()
{
	return g_Property;
}