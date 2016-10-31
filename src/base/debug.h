#ifndef DEBUG_H
#define DEBUG_H

#include <Windows.h>
#include <cstdio>

#define DEXIT_PROCESS() DExitProcess(__FILE__, __FUNCSIG__, __LINE__)

#define DEBUG_OUT(string, buffer, ...)                                  \
  swprintf_s(buffer, sizeof(buffer), (wchar_t*)string, __VA_ARGS__);    \
  OutputDebugString(buffer);

/*
 * Output ASCII only, for libs like sqlite3.
 * see Microsoft.VisualStudio.VCProjectEngine.UseFullPaths to programmatically
 * disable full paths (Linker switch /FC) for cleaner debug output.
 */
#define DEBUG_OUTA(string, buffer, ...)       \
  sprintf_s(buffer, string, __VA_ARGS__);     \
  OutputDebugStringA(buffer);

char* DExitProcess(char* file, char* func_signature, int line);



#endif
