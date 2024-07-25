///////////////////////////////////////////////////////////////////////////////
// MotionControlDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "MotionControlDriver.h"
#include "MotionControlClassFactory.h"

DECLARE_GENERIC_DEVICE(MOTIONCONTROLDRV)

IOSTATUS CMotionControlDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CMotionControlClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CMotionControlDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CMotionControlDriver::MOTIONCONTROLDRV_GetVersion( )
{
	return( (MOTIONCONTROLDRV_Major << 8) | MOTIONCONTROLDRV_Minor );
}

