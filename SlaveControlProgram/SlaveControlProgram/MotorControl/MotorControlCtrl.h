///////////////////////////////////////////////////////////////////////////////
// MotorControlCtrl.h

#ifndef __MOTORCONTROLCTRL_H__
#define __MOTORCONTROLCTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "MotorControlW32.h"
#include "TcBase.h"
#include "MotorControlClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CMotorControlCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CMotorControlCtrl, &CLSID_MotorControlCtrl>
	, public IMotorControlCtrl
	, public ITcOCFCtrlImpl<CMotorControlCtrl, CMotorControlClassFactory>
{
public:
	CMotorControlCtrl();
	virtual ~CMotorControlCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_MOTORCONTROLCTRL)
DECLARE_NOT_AGGREGATABLE(CMotorControlCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMotorControlCtrl)
	COM_INTERFACE_ENTRY(IMotorControlCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __MOTORCONTROLCTRL_H__
