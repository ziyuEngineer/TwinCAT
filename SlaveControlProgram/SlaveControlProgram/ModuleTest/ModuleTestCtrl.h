///////////////////////////////////////////////////////////////////////////////
// ModuleTestCtrl.h

#ifndef __MODULETESTCTRL_H__
#define __MODULETESTCTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "ModuleTestW32.h"
#include "TcBase.h"
#include "ModuleTestClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CModuleTestCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CModuleTestCtrl, &CLSID_ModuleTestCtrl>
	, public IModuleTestCtrl
	, public ITcOCFCtrlImpl<CModuleTestCtrl, CModuleTestClassFactory>
{
public:
	CModuleTestCtrl();
	virtual ~CModuleTestCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_MODULETESTCTRL)
DECLARE_NOT_AGGREGATABLE(CModuleTestCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CModuleTestCtrl)
	COM_INTERFACE_ENTRY(IModuleTestCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __MODULETESTCTRL_H__
