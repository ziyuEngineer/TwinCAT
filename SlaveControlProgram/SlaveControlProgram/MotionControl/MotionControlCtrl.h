///////////////////////////////////////////////////////////////////////////////
// MotionControlCtrl.h

#ifndef __MOTIONCONTROLCTRL_H__
#define __MOTIONCONTROLCTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "MotionControlW32.h"
#include "TcBase.h"
#include "MotionControlClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CMotionControlCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CMotionControlCtrl, &CLSID_MotionControlCtrl>
	, public IMotionControlCtrl
	, public ITcOCFCtrlImpl<CMotionControlCtrl, CMotionControlClassFactory>
{
public:
	CMotionControlCtrl();
	virtual ~CMotionControlCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_MOTIONCONTROLCTRL)
DECLARE_NOT_AGGREGATABLE(CMotionControlCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMotionControlCtrl)
	COM_INTERFACE_ENTRY(IMotionControlCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __MOTIONCONTROLCTRL_H__
