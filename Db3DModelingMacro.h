// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DBPROJECTDATA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DBPROJECTDATA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef Db3DMODELING_EXPORTS
   #define DB_3DMODELING_API __declspec(dllexport)
#else
   #define DB_3DMODELING_API __declspec(dllimport)
#endif

