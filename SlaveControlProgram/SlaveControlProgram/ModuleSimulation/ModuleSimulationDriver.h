///////////////////////////////////////////////////////////////////////////////
// ModuleSimulationDriver.h

#ifndef __MODULESIMULATIONDRIVER_H__
#define __MODULESIMULATIONDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define MODULESIMULATIONDRV_NAME        "MODULESIMULATION"
#define MODULESIMULATIONDRV_Major       1
#define MODULESIMULATIONDRV_Minor       0

#define DEVICE_CLASS CModuleSimulationDriver

#include "ObjDriver.h"

class CModuleSimulationDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl MODULESIMULATIONDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(MODULESIMULATIONDRV)
	VxD_Service( MODULESIMULATIONDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __MODULESIMULATIONDRIVER_H__