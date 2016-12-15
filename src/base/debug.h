#ifndef DEBUG_H
#define DEBUG_H

#include <Windows.h>
#include <cstdio>

#define DEXIT_PROCESS(string, exit)                             \
  write_log(string, exit, __FILE__, __LINE__);                  \
  MessageBoxW(NULL, string, NULL, MB_ICONERROR);                \
  CreateMiniDump(exit);

#define DEBUG_OUT(string, buffer, ...)                                  \
  swprintf_s(buffer, sizeof(buffer), (wchar_t*)string, __VA_ARGS__);    \
  OutputDebugString(buffer);

/*
 * Output ASCII only, for libs like sqlite3.
 * see Microsoft.VisualStudio.VCProjectEngine.UseFullPaths to programmatically
 * disable full paths (Linker switch /FC) for cleaner debug output.
 */
#ifdef _DEBUG
#define DEBUG_OUTA(string, buffer, ...)       \
  sprintf_s(buffer, string, __VA_ARGS__);     \
  OutputDebugStringA(buffer);
#else
#define DEBUG_OUTA(string, buffer, ...)
#endif

char* DExitProcess(char* file, char* func_signature, int line);

void CreateMiniDump(int exit);

void write_log(wchar_t *text, DWORD error, char *file, int line);

#endif
