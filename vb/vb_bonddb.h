#ifndef VB_BONDDB_H
#define VB_BONDDB_H

// The following ifdef block is the standard way of creating macros which 
// make exporting from a DLL simpler.  The VB_BONDDB.cpp file is compiled with 
// the symbol VB_BONDDB_EXPORTS defined at the top of DLL2.cpp.  This symbol 
// should *not* be defined in any project that uses VB_BONDDB.  This way any 
// other project whose source files include VB_BONDDB.h will see DLL2_API defined 
// as __declspec(dllimport), whereas within VB_BONDDB.cpp, DLL2_API is defined as
// __declspec(dllexport).

#ifdef VB_BONDDB_EXPORTS
    #define VB_BONDDB_API __declspec(dllexport)
#else
    #define VB_BONDDB_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////////////////////////////////
// only need exports here for C language... not for VB.
// This function is exported from the VB_BONDDB.dll
//VB_BONDDB_API int __stdcall GetCpuSpeed();
//VB_BONDDB_API BSTR __stdcall GetMagicWord();
//VB_BONDDB_API BSTR __stdcall system_init(BSTR name);
//VB_BONDDB_API void* __stdcall system_init(BSTR name);
//VB_BONDDB_API BSTR __stdcall config_getvalue(BSTR name);
#endif //VB_BONDDB_H
