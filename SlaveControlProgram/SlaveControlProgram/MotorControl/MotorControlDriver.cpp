///////////////////////////////////////////////////////////////////////////////
// MotorControlDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "MotorControlDriver.h"
#include "MotorControlClassFactory.h"

DECLARE_GENERIC_DEVICE(MOTORCONTROLDRV)

IOSTATUS CMotorControlDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CMotorControlClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CMotorControlDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CMotorControlDriver::MOTORCONTROLDRV_GetVersion( )
{
	return( (MOTORCONTROLDRV_Major << 8) | MOTORCONTROLDRV_Minor );
}

