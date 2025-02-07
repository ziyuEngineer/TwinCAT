// ModuleSimulationCtrl.cpp : Implementation of CTcModuleSimulationCtrl
#include "TcPch.h"
#pragma hdrstop

#include "ModuleSimulationW32.h"
#include "ModuleSimulationCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CModuleSimulationCtrl

CModuleSimulationCtrl::CModuleSimulationCtrl() 
	: ITcOCFCtrlImpl<CModuleSimulationCtrl, CModuleSimulationClassFactory>() 
{
}

CModuleSimulationCtrl::~CModuleSimulationCtrl()
{
}

