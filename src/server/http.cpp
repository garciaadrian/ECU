#include "http.h"
#include <Windows.h>
#include <base/debug.h>
#include <fstream>
#include <limits>
#include <tchar.h>

char* get_file_name() {
  LPDWORD bytes_read = 0;
  HANDLE file = CreateFile(L"bin/result.html", GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (file == INVALID_HANDLE_VALUE) {
    DEXIT_PROCESS();
  }

  DWORD file_size = GetFileSize(file, NULL);
  LPSTR file_text;
  file_text = (LPSTR)GlobalAlloc(GPTR, file_size + 1);

  if (file_text != NULL) {
    if (ReadFile(file, file_text, file_size, bytes_read, NULL)) {
      file_text[file_size] = 0;
    }
  }
  CloseHandle(file);

  return file_text;
}

int answer_to_connection(void* cls, struct MHD_Connection* connection,
                         const char* url, const char* method,
                         const char* version, const char* upload_data,
                         size_t* upload_data_size, void** con_cls) {
  char* page = get_file_name();
  struct MHD_Response* response;
  int ret;

  response = MHD_create_response_from_buffer(strlen(page), (void*)page,
                                             MHD_RESPMEM_PERSISTENT);

  ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

  MHD_destroy_response(response);

  return ret;
}
