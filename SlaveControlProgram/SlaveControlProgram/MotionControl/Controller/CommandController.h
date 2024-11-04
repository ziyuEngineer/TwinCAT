#pragma once
#include "CommandManager.h"

class CCommandController
{
public:
	CCommandController();
	~CCommandController();

// Initialize
public:
	void MapParameters(MotionControlInputs* inputs, MotionControlOutputs* outputs);
	static IPanelProcess* m_pPanelProcess;
	static ISpindleInterface* m_pSpindleInterface;
	static IAxisGroupInterface* m_pAxisGroupInterface;

	SystemState m_MainState;

private:
	MotionControlInputs* m_pInputs = nullptr;
	MotionControlOutputs* m_pOutputs = nullptr;

	CCommandManager m_CommandManager;
	void PanelProcess();

public:
	// Cyclic Run
	void Input(); // Update Inputs from hardware or host machine
	void Output(); // Update Outputs to hardware or host machine

	bool IsReadyToStandby();
	bool IsMovingFinished();

	// Command process
	bool RingBufferDispatchCommand();
	void QuitMachining();

	// Panel Information
	bool IsServoOnPressed();
	bool IsManualModeSelected();
	bool IsExecutionPressed();
	int IsSpindleMovingPressed();
	void UpdateManualMovingCommand();

	void RingBufferInput();
	void RingBufferOutput();
	int GetCurrentRingBufferSize();

	// RPC invoked by host
	// Ring Buffer
	int GetMaxBufferSize();
	bool IsReadyToMove();
	bool IsReadyToReceiveCmd();

	// Invoke interfaces provided by Spindle module
	void RequestSpindleRotate(double vel);
	void RequestSpindleStop();
	void RequestSpindleEnable();
	void RequestSpindleDisable();

	// Invoke interfaces provided by AxisGroup module
	void RequestAxisGroupServoOn();
	void RequestAxisGroupServoOff();
	void RequestAxisGroupChangeOpMode(OpMode mode);
	void RequestAxisGroupMove();
	void RequestAxisGroupStop();
	void RequestAxisGroupEnterHandwheel();
	void RequestAxisGroupQuitHandwheel();
	void RequestAxisGroupResetError();
	void RequestAxisGroupRecoveryState(bool is_entered);
	bool IsAxisGroupOpModeChanged();
	bool IsAxisGroupDisabled();
};