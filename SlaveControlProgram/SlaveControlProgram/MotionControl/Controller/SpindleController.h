#pragma once
#include "Spindle.h"

class CSpindleController
{
public:
	CSpindleController();
	~CSpindleController();

	// Initialize
public:
	void MapParameters(ModuleSpindleInputs* inputs, ModuleSpindleOutputs* outputs, ModuleSpindleParameter* parameters);
	bool PostConstruction();

private:
	ModuleSpindleInputs* m_pInputs = nullptr;
	ModuleSpindleOutputs* m_pOutputs = nullptr;

	CSpindle m_Spindle;

public:
	// Cyclic Run
	void Input(); // Update Inputs from hardware or host machine
	void Output(); // Update Outputs to hardware or host machine

	// Spindle state machine
	SpindleState m_SpindleState;
	SpindleState GetCurrentState();
	
	bool SpindleInitialize();
	void SpindleStandby();
	void SpindleMoving(SpindlePosition spindle_cmd);
	void SpindleMoving(SpindleRot spindle_cmd);
	void SpindleBrake();
	void SpindleStandStill();
	void SpindleFault();
	bool SpindleEnable();
	bool SpindleDisable();
	bool IsSpindleEnabled();
	bool IsSpindleMotionless();
	void SpindleSwitchOpMode(OpMode mode);
	bool IsSpindleOpModeSwitched();
	void SpindleResetInterpolator(OpMode mode);
	void SpindleClearError();

	// RPC
	SpindleVelLimit m_SpindleVelLimit;
	void SetSpindleVelLimit(SpindleVelLimit vel_limit);
};