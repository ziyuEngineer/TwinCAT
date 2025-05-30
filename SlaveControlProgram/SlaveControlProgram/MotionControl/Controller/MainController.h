#pragma once
#include "CommandManager.h"
#include "Matrix.h"

class CMainController
{
public:
	CMainController();
	~CMainController();

// Initialize
public:
	void MapParameters(MotionControlInputs* inputs, MotionControlOutputs* outputs);
	static IPanelProcess* m_pPanelProcess;
	static ISpindleInterface* m_pSpindleInterface;
	static IAxisGroupInterface* m_pAxisGroupInterface;

	SystemState m_MainState;
	static CTcTrace* m_Trace;

	// Ecent logger
	static ITcEventLogger* m_pEventLogger;
	static ITcMessage* m_pMessage;

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
	void EmptyRingBuffer();
	void QuitMachiningNormally();
	void QuitMachiningWithWarning();
	void ClearLocalCommand();

	// Panel Information
	bool IsServoOnPressed();
	bool IsManualModeSelected();
	bool IsExecutionPressed();
	int IsSpindleMovingPressed();
	void UpdateHandwheelInfo();
	void ClearHandwheelInfo();
	void UpdateManualCommandInRecoveryState(ModuleError dir, AxisOrder axis);
	bool IsResetPressed();
	bool PanelReset();
	bool DeselectAxisAutomatically();
	bool DeselectServoButtonAutomatically();
	double GetSpindleRatio();
	double GetMachiningAxisRatio();

	void RingBufferInput();
	void RingBufferOutput();
	ULONG GetCurrentRingBufferSize();
	bool IsLatestCommandStop();

	// RPC invoked by host
	// Ring Buffer
	ULONG GetMaxBufferSize();
	bool IsReadyToMove();
	bool IsReadyToReceiveCmd();
	bool IsReadyToSwitchTool();

	// Invoke interfaces provided by Spindle module
	void RequestSpindleRotate(double vel);
	void RequestSpindlePositioning(double pos);
	void RequestSpindleStop();
	void RequestSpindleEnable();
	void RequestSpindleDisable();
	void RequestSpindleFault();
	void RequestSpindleResetError();

	// Invoke interfaces provided by AxisGroup module
	void RequestAxisGroupServoOn();
	void RequestAxisGroupServoOff();
	void RequestAxisGroupChangeOpMode(OpMode mode);
	void RequestAxisGroupMove();
	void RequestAxisGroupStop();
	void RequestAxisGroupEnterHandwheel();
	void RequestAxisGroupQuitHandwheel();
	void RequestAxisGroupFault();
	void RequestAxisGroupResetError();
	bool IsAxisGroupOpModeChanged();
	bool IsAxisGroupDisabled();
	bool IsAxisGroupReadyForPositioning();
	void RequestAxisGroupPositioning(bool moving_axis[5], double target[5]);

	// Invoke interfaces provided by AxisGroup module
	void RequestToolSwitch(int num);

	// Safety module
	ULONG GetErrorCode();
	bool IsSystemNormal();

	// Event Logger
	void DispatchEventMessage(ULONG event_id);

// Test
private:
	Matrix<50, 50> m_MatrixA;
	Matrix<100, 100> m_MatrixB;
	Vector<50> m_VectorB;
	bool MatrixAssign();
	void MatrixTest();

	// The following flags are used to prevent send too many duplicate event logs
	bool m_IsRecoveryEventSent = false;
};