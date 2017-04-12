/* -*-c++-*- OpenSceneGraph Cookbook
 * Common functions
 * Author: Wang Rui <wangray84 at gmail dot com>
*/
#include "stdafx.h"

#include "CommonFunctions.h"


BOOL IsNameExist(CString strName, const std::vector<CString>& vecName)
{
	BOOL bExist = FALSE;
	for (int i = 0; i < (int)vecName.size(); ++i)
	{
		if (strName.Compare(vecName[i]) == 0)
		{
			bExist = TRUE;
			break;
		}
	}
	return bExist;
}
CString GetNonExistName(const std::vector<CString>& vecName, const CString &strSrcName, const CString& strPre)
{
	if (!strSrcName.IsEmpty() && !IsNameExist(strSrcName, vecName))
	{
		return strSrcName;
	}

	int index = 1;
	CString strName;
	strName.Format("%s%d", strPre, index);
	BOOL bExist = FALSE;
	while (1)
	{
		for (int i = 0; i < (int)vecName.size(); ++i)
		{
			if (strName.Compare(vecName[i]) == 0)
			{
				bExist = TRUE;
				break;
			}
		}
		if (bExist)
		{
			index++;
			strName.Format("%s%d", strPre, index);
			bExist = FALSE;
		}
		else
			return strName;
	}
}
