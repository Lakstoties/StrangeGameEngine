#pragma once
//
//  This include is to standardize the methods for declaring how functions are exported and imported
//


#if defined(_WIN32) && defined(SGE_BUILD_DLL)
//
//  Windows OS and building this project as a DLL
//

#define SGEAPI __declspec(dllexport)

#elif defined(_WIN32) && defined(SGE_USE_DLL)
//
// 
//
#define SGEAPI __declspec(dllimport)

#else
//
//  Static library, so we aren't importing or exporting anything.
//

#define SGEAPI
#endif