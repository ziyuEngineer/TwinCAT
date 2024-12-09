#pragma once
#include "CommonDefine.h"
#include <unordered_map>

class CSafetyController
{
public:
	CSafetyController();
	~CSafetyController();

// Initialize
public:
	void MapParameters(ModuleSafetyInputs* inputs, ModuleSafetyOutputs* outputs, const ModuleSafetyParameter* parameters);
	static IMotionControlInterface* m_pMainModuleInterface;

	SafetyState m_SafetyStatus;
	void Input();
	void Output();
	ULONG m_ErrorCode = 0;

private:
	ModuleSafetyInputs* m_pInputs = nullptr;
	ModuleSafetyOutputs* m_pOutputs = nullptr;
	const ModuleSafetyParameter* m_pParameters = nullptr;

	int m_ActualAxisNum = 0;
	int m_ActualDriverNum = 0;
	SHORT m_DriverNumPerAxis[5] = { 0 }; // INT type in tmc
	std::unordered_map<int, AxisOrder> m_DriverAxisMap;

	// Check functions
	// Warning check list
	bool IsAxisGroupPosLimitCheckPassed();
	bool IsAxisGroupVelLimitCheckPassed();
	bool IsSpindlePosLimitCheckPassed();
	bool IsSpindleVelLimitCheckPassed();
	bool IsSubSystemLimitsCheckPassed();
	double m_ErrorPos[6] = { 0.0 };
	int m_ErrorDriver;

	// Emergency check list
	bool IsHardLimitCheckPassed();
	bool IsGantryDeviationCheckPassed();
	double m_ErrorDeviation = 0.0;

	// Fault check list
	bool IsAxisGroupStatusCheckPassed();
	bool IsSpindleStatusCheckPassed();
	bool IsSubSystemStatusCheckPassed();

	// Recovery check list
	bool IsMovingOppositely();
	bool IsDeviationDecreasing();

	bool IsAxisGroupExitFault();
	bool IsSpindleExitFault();

	int m_HardLimitDigitalMask = 15;// 14(1110):y+; 13(1101):y-; 11(1011):z+; 7(0111):z-

	unsigned long GenerateErrorCode(ProjectSeries project, ModuleName module_name, ModuleError module_err, AxisOrder axis_num, ErrorClass errorClass);
	unsigned long GenerateTinyErrorCode(ModuleName module_name, ModuleError module_err, AxisOrder axis_num);

public:
	bool IsSubSystemStateCorrect();
	
	bool IsFaultCheckPassed();
	bool IsEmergencyCheckPassed();
	bool IsWarningCheckPassed();
	bool IsRecoveryCheckPassed();

	void ClearErrorCode();

	// Invoke interfaces provided by main module
	void ReportFault();
	void RequestClearFault();

	void ReportEmergency();

	void ReportWarning();

	void RequestEnterRecovery();
	void RequestExitRecovery();
};