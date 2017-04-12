
#ifndef H_COOKBOOK_COMMONFUNCTIONS
#define H_COOKBOOK_COMMONFUNCTIONS

 DB_3DMODELING_API BOOL IsNameExist(CString strName, const std::vector<CString>& vecName);
 DB_3DMODELING_API CString GetNonExistName(const std::vector<CString>& vecName, const CString &strSrcName, const CString& strPre);

#endif
