// MotorControlCtrl.cpp : Implementation of CTcMotorControlCtrl
#include "TcPch.h"
#pragma hdrstop

#include "MotorControlW32.h"
#include "MotorControlCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMotorControlCtrl

CMotorControlCtrl::CMotorControlCtrl() 
	: ITcOCFCtrlImpl<CMotorControlCtrl, CMotorControlClassFactory>() 
{
}

CMotorControlCtrl::~CMotorControlCtrl()
{
}

