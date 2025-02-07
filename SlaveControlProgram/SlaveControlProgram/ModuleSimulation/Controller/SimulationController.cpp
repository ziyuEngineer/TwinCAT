#include "../TcPch.h"
#pragma hdrstop
#include "SimulationController.h"

CSimulationController::CSimulationController()
{
}

CSimulationController::~CSimulationController()
{
}

void CSimulationController::MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_ActualAxisGroupDriverNum = parameters->ActualAxisGroupDriverNum;
	m_SimAxisGroup.MapParameters(outputs, inputs, parameters);
	m_SimSpindle.MapParameters(outputs, inputs, parameters);
}

void CSimulationController::Run()
{
	m_SimAxisGroup.Run();
	m_SimSpindle.Run();
}

void CSimulationController::Initialize()
{
	m_SimAxisGroup.Initialize();
	m_SimSpindle.Initialize();
	MockSoEDeviveInfo();
}

void CSimulationController::MockSoEDeviveInfo()
{
	for (int i = 0; i < m_ActualAxisGroupDriverNum; i++)
	{
		m_pOutputs->SimAxisGroupDriverInfo[i].aNetId = "169.254.41.58.4.1";
		m_pOutputs->SimAxisGroupDriverInfo[i].nSlaveAddr = 1002;
		m_pOutputs->SimAxisGroupDriverInfo[i].nDriveNo = 0x00;
	}
}
