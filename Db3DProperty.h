#pragma once
#include "Db3DPropertyData.h"

class CDb3DProperty
{
public:
	CDb3DProperty();
	~CDb3DProperty();
	virtual  bool ApplyProperty(const CDb3DPropertyData& data){ return false; };
	virtual  void CreateProperty(CDb3DPropertyData& data){};
};




