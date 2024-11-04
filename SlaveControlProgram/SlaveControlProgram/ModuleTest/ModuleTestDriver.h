///////////////////////////////////////////////////////////////////////////////
// ModuleTestDriver.h

#ifndef __MODULETESTDRIVER_H__
#define __MODULETESTDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define MODULETESTDRV_NAME        "MODULETEST"
#define MODULETESTDRV_Major       1
#define MODULETESTDRV_Minor       0

#define DEVICE_CLASS CModuleTestDriver

#include "ObjDriver.h"

class CModuleTestDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl MODULETESTDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(MODULETESTDRV)
	VxD_Service( MODULETESTDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __MODULETESTDRIVER_H__