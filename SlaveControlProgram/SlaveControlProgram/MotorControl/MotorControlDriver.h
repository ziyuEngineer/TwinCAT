///////////////////////////////////////////////////////////////////////////////
// MotorControlDriver.h

#ifndef __MOTORCONTROLDRIVER_H__
#define __MOTORCONTROLDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define MOTORCONTROLDRV_NAME        "MOTORCONTROL"
#define MOTORCONTROLDRV_Major       1
#define MOTORCONTROLDRV_Minor       0

#define DEVICE_CLASS CMotorControlDriver

#include "ObjDriver.h"

class CMotorControlDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl MOTORCONTROLDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(MOTORCONTROLDRV)
	VxD_Service( MOTORCONTROLDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __MOTORCONTROLDRIVER_H__