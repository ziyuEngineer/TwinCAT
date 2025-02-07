///////////////////////////////////////////////////////////////////////////////
// ModuleSimulationDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "ModuleSimulationDriver.h"
#include "ModuleSimulationClassFactory.h"

DECLARE_GENERIC_DEVICE(MODULESIMULATIONDRV)

IOSTATUS CModuleSimulationDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CModuleSimulationClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CModuleSimulationDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CModuleSimulationDriver::MODULESIMULATIONDRV_GetVersion( )
{
	return( (MODULESIMULATIONDRV_Major << 8) | MODULESIMULATIONDRV_Minor );
}

