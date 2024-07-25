///////////////////////////////////////////////////////////////////////////////
// MotionControlDriver.h

#ifndef __MOTIONCONTROLDRIVER_H__
#define __MOTIONCONTROLDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define MOTIONCONTROLDRV_NAME        "MOTIONCONTROL"
#define MOTIONCONTROLDRV_Major       1
#define MOTIONCONTROLDRV_Minor       0

#define DEVICE_CLASS CMotionControlDriver

#include "ObjDriver.h"

class CMotionControlDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl MOTIONCONTROLDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(MOTIONCONTROLDRV)
	VxD_Service( MOTIONCONTROLDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __MOTIONCONTROLDRIVER_H__