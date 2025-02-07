#pragma once

#include "ModuleSimulationCommonDefine.h"

#include "SimAxisGroup.h"
#include "SimSpindle.h"

class CSimulationController
{
public:
	CSimulationController();
	~CSimulationController();

	void MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters);

	void Run();

	void Initialize();

private:
	ModuleSimulationInputs* m_pInputs = nullptr;
	ModuleSimulationOutputs* m_pOutputs = nullptr;
	CSimAxisGroup m_SimAxisGroup;
	CSimSpindle m_SimSpindle;

	int m_ActualAxisGroupDriverNum = 0;

	void MockSoEDeviveInfo();
};