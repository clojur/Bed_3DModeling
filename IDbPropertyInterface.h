#pragma once

class CDb3DProperty;

class DB_3DMODELING_API IDbPropertyInterface
{
public:
	IDbPropertyInterface();
	virtual ~IDbPropertyInterface();

 
	virtual void populate(CDb3DProperty* pObj);
};

DB_3DMODELING_API void SetDbProperty(IDbPropertyInterface* pCommandLine);
DB_3DMODELING_API IDbPropertyInterface* GetDbProperty();

