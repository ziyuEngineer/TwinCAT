#pragma once
#include "SimCANopen.h"

class CSimSpindle
{
public:
	CSimSpindle();
	~CSimSpindle();

public:
	CSimCANopen m_Axis;
	void MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters);

	void Initialize();
	void Run();

private:
	double m_CycleTime = 0.0;
};