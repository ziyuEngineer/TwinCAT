#pragma once

#include "ModuleSimulationCommonDefine.h"

class CSimSERCOS
{
public:
	CSimSERCOS() = default;
	~CSimSERCOS() = default;

protected:
	const MotionControlInfo* m_DriverParam = nullptr;
	DriverOutput* m_SimInput = nullptr;
	DriverInput* m_SimOutput = nullptr;
	double m_CycleTime = 0.0;

public:
	void MapParameters(DriverInput* sim_output, DriverOutput* sim_input, const MotionControlInfo* driver_param, const double cycle_time);
	void Initialize();

	void Receive();
	void Transmit();

	void ReactionsToReceivedCommands();

private:
	void ReactionToControlWord();
	void HandlePowerTransitions();

	void ReactionToTargetCommand();
	void ReactionToPosCommand();
	void ReactionToVelCommand();
	void ReactionToTorCommand();

	void CommandSafetyCheck();

	// Simulation output
	unsigned short m_StatusWord = 0;
	long m_FdbPosVal = 0;		
	short m_FdbTorVal = 0;		
	long m_FdbVelVal = 0;		
	long m_EffectivePosCmd = 0;	
	short m_EffectiveTorCmd = 0;
	long m_EffectiveVelCmd = 0;	

	// Simulation input
	unsigned short m_ControlWord = 0;
	short m_TargetTor = 0;
	short m_AdditiveTor = 0;
	long m_TargetPos = 0;
	long m_TargetVel = 0;
	long m_AdditivePos = 0;
	long m_AdditiveVel = 0;

};