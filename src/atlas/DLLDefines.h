#ifndef _ATLAS_DLLDEFINES_H_
#define _ATLAS_DLLDEFINES_H_

#if defined (_WIN32)
  #if defined(ATLAS_EXPORTS)
    #define ATLAS_EXPORT __declspec(dllexport)
  #else
    #define ATLAS_EXPORT __declspec(dllimport)
  #endif
#else
  #define ATLAS_EXPORT
#endif


#endif /* _ATLAS_DLLDEFINES_H_ */
