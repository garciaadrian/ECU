#include "http.h"
#include <Windows.h>
#include <base/debug.h>
#include <fstream>
#include <limits>
#include <tchar.h>
#include <Shlwapi.h>
#include <Strsafe.h>

#define MAX_PATH_UNICODE 32767

int answer_to_connection(void* cls, struct MHD_Connection* connection,
                         const char* url, const char* method,
                         const char* version, const char* upload_data,
                         size_t* upload_data_size, void** con_cls) {
  
  if (!strcmp(url, "/")) {
    HANDLE file = CreateFile(L"reactjs/index.html", GENERIC_READ, 0, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL);

    if (file == INVALID_HANDLE_VALUE)
      LOGF(FATAL, "failed to open file handle");

    int file_size = GetFileSize(file, NULL);
    DWORD bytes_read = 0;
    
    char *page = (char*)malloc(file_size+1);
    SecureZeroMemory(page, file_size+1);
  
    ReadFile(file, page, file_size, &bytes_read, NULL);

    struct MHD_Response* response;
    int ret;

    /* TODO: use SizeofResources vs strlen? */
    response = MHD_create_response_from_buffer(strlen((char*)page), page,
                                               MHD_RESPMEM_PERSISTENT);
  
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    
    MHD_destroy_response(response);

    return ret;
  }

  wchar_t url_wcs[MAX_PATH_UNICODE];
  mbstowcs(url_wcs, url, MAX_PATH_UNICODE);

  wchar_t dest[MAX_PATH_UNICODE] = L"reactjs/";
  StringCchCat(dest, MAX_PATH_UNICODE, url_wcs);

  HANDLE file = CreateFile(dest, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL, NULL);

  struct MHD_Response* response;
  int ret;

  if (file != INVALID_HANDLE_VALUE) {

    int file_size = GetFileSize(file, NULL);
    DWORD bytes_read = 0;

    char *page = (char*)malloc(file_size+1);
    SecureZeroMemory(page, file_size+1);

    ReadFile(file, page, file_size, &bytes_read, NULL);
    
    response = MHD_create_response_from_buffer(strlen((char*)page), page,
                                               MHD_RESPMEM_PERSISTENT);
  
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    
    MHD_destroy_response(response);

    return ret;

  }
  

  return 404;
  /* TODO: return 404 response, empty responses makes resource load time
   * 200-300 ms for favico.ico and style.css each*/
}
