#include "http.h"
#include <Windows.h>
#include <base/debug.h>
#include <fstream>
#include <limits>
#include <tchar.h>
#include <Shlwapi.h>

int answer_to_connection(void* cls, struct MHD_Connection* connection,
                         const char* url, const char* method,
                         const char* version, const char* upload_data,
                         size_t* upload_data_size, void** con_cls) {
  
  if (!strcmp(url, "/")) {

    HRSRC page_rsrc = FindResource(NULL, MAKEINTRESOURCE(102), RT_HTML);
    HGLOBAL page_handle = LoadResource(NULL, page_rsrc);
    LPVOID page = LockResource(page_handle);

    struct MHD_Response* response;
    int ret;

    /* TODO: use SizeofResources vs strlen? */
    response = MHD_create_response_from_buffer(strlen((char*)page), page,
                                               MHD_RESPMEM_PERSISTENT);

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    MHD_destroy_response(response);

    return ret;
  }

  else if (!strcmp(url, "/style.css")) {
    
    if (PathFileExists(L"style.css")) {
      
      HANDLE style_handle = CreateFile(L"style.css", GENERIC_READ, FILE_SHARE_READ,
                                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      
      if (style_handle == INVALID_HANDLE_VALUE) {
        /* TODO: Logging framework is needed badly, this error is trivial */
        DEXIT_PROCESS(L"style.css is being used by another proccess", 0);
      }
    
      DWORD style_size = GetFileSize(style_handle, NULL);
      DWORD bytes_read = 0;

      char *page = (char*)malloc(style_size+1);
      
      ReadFile(style_handle, page, style_size, &bytes_read, NULL);
      CloseHandle(style_handle);
      page[style_size] = 0;

      struct MHD_Response* response;
      int ret;
    
      /* TODO: use SizeofResources vs strlen? */
      response = MHD_create_response_from_buffer(strlen((char*)page), page,
                                                 MHD_RESPMEM_MUST_FREE);

      ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    
      MHD_destroy_response(response);

      return ret;
    
    }

    return MHD_NO;
    /* TODO: return 404 response, empty responses makes resource load time
     * 200-300 ms for favico.ico and style.css each*/
  }

  return MHD_NO;
}
