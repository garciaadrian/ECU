/**
*******************************************************************************
*                                                                             *
* ECU: iRacing Extensions Collection Project                                  *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#include "atlas/DLLDefines.h"

class CTDataSourceBase;

enum AEXAPI_ExtApiVersion {
	AEXAPI_Version1 = 0,	//	CTDataSourceBase
	AEXAPI_Version2 = 1,	//	CTDataSource2
	AEXAPI_Version3	= 2,	//	CTDataSource3
	AEXAPI_Version4	= 3,	//	CTDataSource4
	AEXAPI_Version5	= 4,	//	CTDataSource5
	AEXAPI_Version6	= 5,	//	CTDataSource6
	AEXAPI_Version7	= 6		//	CTDataSource7
};
#define AEXAPI_CURRENT_EXT_API_VERSION (AEXAPI_Version7)

AEXAPI_ExtApiVersion AEXAPI_GetVersion() {
  return AEXAPI_CURRENT_EXT_API_VERSION;
}

bool AEXAPI_GetInterfaceInstance(AEXAPI_ExtApiVersion version_requested,
                                                       CTDataSourceBase** pp_instance) {
  return true;
}

void AEXAPI_ReleaseInterfaceInstance(CTDataSourceBase* pInstance) {
  
}
