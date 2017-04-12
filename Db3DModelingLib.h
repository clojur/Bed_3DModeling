#pragma once

#ifdef _DEBUG
   #pragma comment(lib,"Db3DModeling_d.lib") 
   #pragma message("Automatically linking with Db3DModeling_d.dll")
#else
   #pragma comment(lib,"Db3DModeling.lib") 
   #pragma message("Automatically linking with Db3DModeling.dll") 
#endif
