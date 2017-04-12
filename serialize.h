
#pragma  once

template <class TYPE>
void DBSerializeVector(CArchive& ar, vector<TYPE>& vecObj)	 //�ʺ���TYPE1���ж�����Serialize����
{
	if (ar.IsStoring())
	{
		ar << static_cast<int>(vecObj.size());
	}
	else
	{
		int nCount = 0;
		ar >> nCount;
		if (nCount > 1000000)
		{//2008-07-30
			_ASSERTE(("SerializeVector����������������,�������л�", FALSE));
			nCount = 0;//2008-07-30
		}
		vecObj.resize(nCount);
	}
	for (int i = 0; i < static_cast<int>(vecObj.size()); i++)
	{
		vecObj[i].Serialize(ar);
	}
}