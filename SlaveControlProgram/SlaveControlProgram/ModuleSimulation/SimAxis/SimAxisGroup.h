#pragma once
#include "SimSERCOS.h"

class CSimAxisGroup
{
public:
	CSimAxisGroup();
	~CSimAxisGroup();

public:
	CSimSERCOS m_Axes[kMaxAxisNum * kMaxMotorNumPerAxis];
	void MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters);

	void Initialize();
	void Run();
	
private:
	int m_ActualDriverNum = 0;
	double m_CycleTime = 0.0;
};