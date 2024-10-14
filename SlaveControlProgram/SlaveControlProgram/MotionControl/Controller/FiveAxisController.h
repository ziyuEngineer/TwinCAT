#pragma once
#include "AxisGroup.h"
#include "Spindle.h"
#include "CommandManager.h"
#include "Matrix.h"

class CFiveAxisController
{
public:
	CFiveAxisController();
	~CFiveAxisController();
	
// Initialize
public:
	void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters);
	bool PostConstruction();
	static ISoEProcess* m_pSoEProcess;
	static IPanelProcess* m_pPanelProcess;

private:
	MotionControlInputs* m_pInputs = nullptr;
	MotionControlOutputs* m_pOutputs = nullptr;

	CAxisGroup m_AxisGroup;
	CSpindle m_Spindle;

	CCommandManager m_CommandManager;
	void PanelProcess();

public:
	// Cyclic Run
	void Input(); // Update Inputs from hardware or host machine
	void Output(); // Update Outputs to hardware or host machine
	void CommonModuleProcess(); // Process common modules which will be used by AxisGroup and Spindle

	// AxisGroup State Machine
	SystemState m_AxisGroupState;
	bool AxisGroupInitialize();
	void AxisGroupStandby();
	void AxisGroupMoving();
	void AxisGroupHandwheel();
	void AxisGroupLimitViolation();
	void AxisGroupFault();
	void AxisGroupTest();
	void AxisGroupRecovery();
	bool AxisGroupEnable();
	bool AxisGroupDisable();
	// Safety Module
	SystemState AxisGroupSafetyCheck();
	
	// Spindle state machine
	SystemState m_SpindleState;
	bool SpindleInitialize();
	void SpindleStandby();
	void SpindleMoving();
	void SpindleFault();
	void SpindleTest();
	bool SpindleEnable();
	bool SpindleDisable();
	// Safety Module
	SystemState SpindleSafetyCheck();

	// Local state switch trigger
	// Common
	bool IsServoButtonOn();
	bool IsResetSelected();
	bool IsMovingStarted();
	bool IsMovingFinished();
	bool m_bMovingStart = false;
	bool m_bMovingFinish = false;
	// AxisGroup
	bool IsTestStateSelected();
	bool IsHandwheelStateSelected();
	void NotifyPlcResetSoE(bool _bSignal);
	// Spindle
	bool IsSpindleTestSelected();

	// Command process
	void GetCommand();
	void ResetCommand();
	void HandwheelCommandReset();

	// RPC related
	// Ring Buffer
	int GetBufferSize();
	bool SetBufferStartLength(int _min);
	bool IsReadyToMove();
	bool IsReadyToReceiveCmd();
	// State switch trigger from host machine
	bool m_bEnterRecoveryState = false;
	void SetRecoveryFlag(bool _bEnter);

private:
	// Test variables
	double m_SimTime = 0.0;
	double m_TestInitPos[kMaxAxisNum];
	double m_SimTimeSpindle = 0.0;

	void MatrixTest();
};