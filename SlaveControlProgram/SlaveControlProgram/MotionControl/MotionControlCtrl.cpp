// MotionControlCtrl.cpp : Implementation of CTcMotionControlCtrl
#include "TcPch.h"
#pragma hdrstop

#include "MotionControlW32.h"
#include "MotionControlCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMotionControlCtrl

CMotionControlCtrl::CMotionControlCtrl() 
	: ITcOCFCtrlImpl<CMotionControlCtrl, CMotionControlClassFactory>() 
{
}

CMotionControlCtrl::~CMotionControlCtrl()
{
}

