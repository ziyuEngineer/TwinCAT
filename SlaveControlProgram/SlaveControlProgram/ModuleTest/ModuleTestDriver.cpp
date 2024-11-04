///////////////////////////////////////////////////////////////////////////////
// ModuleTestDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "ModuleTestDriver.h"
#include "ModuleTestClassFactory.h"

DECLARE_GENERIC_DEVICE(MODULETESTDRV)

IOSTATUS CModuleTestDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CModuleTestClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CModuleTestDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CModuleTestDriver::MODULETESTDRV_GetVersion( )
{
	return( (MODULETESTDRV_Major << 8) | MODULETESTDRV_Minor );
}

