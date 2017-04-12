
#pragma  once

template <class TYPE>
void DBSerializeVector(CArchive& ar, vector<TYPE>& vecObj)	 //适合于TYPE1类中定义了Serialize函数
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
			_ASSERTE(("SerializeVector读入的数组个数过大,请检查序列化", FALSE));
			nCount = 0;//2008-07-30
		}
		vecObj.resize(nCount);
	}
	for (int i = 0; i < static_cast<int>(vecObj.size()); i++)
	{
		vecObj[i].Serialize(ar);
	}
}