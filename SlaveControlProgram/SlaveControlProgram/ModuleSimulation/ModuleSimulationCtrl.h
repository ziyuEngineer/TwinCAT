///////////////////////////////////////////////////////////////////////////////
// ModuleSimulationCtrl.h

#ifndef __MODULESIMULATIONCTRL_H__
#define __MODULESIMULATIONCTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "ModuleSimulationW32.h"
#include "TcBase.h"
#include "ModuleSimulationClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CModuleSimulationCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CModuleSimulationCtrl, &CLSID_ModuleSimulationCtrl>
	, public IModuleSimulationCtrl
	, public ITcOCFCtrlImpl<CModuleSimulationCtrl, CModuleSimulationClassFactory>
{
public:
	CModuleSimulationCtrl();
	virtual ~CModuleSimulationCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_MODULESIMULATIONCTRL)
DECLARE_NOT_AGGREGATABLE(CModuleSimulationCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CModuleSimulationCtrl)
	COM_INTERFACE_ENTRY(IModuleSimulationCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __MODULESIMULATIONCTRL_H__
