#include "debug.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

// impliment namespaces

char DEXIT_BUFFER[4096];

char* DExitProcess(char* file, char* func_signature, int line) {
  sprintf_s(DEXIT_BUFFER, "\n\n%s \n%s || LINE:%d\n\n", file, func_signature,
            line);
  OutputDebugStringA(
      DEXIT_BUFFER); // func_signature outputs in char *, no auto coversion
  ExitProcess(line);
}
