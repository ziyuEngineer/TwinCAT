// ModuleTestCtrl.cpp : Implementation of CTcModuleTestCtrl
#include "TcPch.h"
#pragma hdrstop

#include "ModuleTestW32.h"
#include "ModuleTestCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CModuleTestCtrl

CModuleTestCtrl::CModuleTestCtrl() 
	: ITcOCFCtrlImpl<CModuleTestCtrl, CModuleTestClassFactory>() 
{
}

CModuleTestCtrl::~CModuleTestCtrl()
{
}

