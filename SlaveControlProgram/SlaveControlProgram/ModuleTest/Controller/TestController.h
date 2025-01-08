#pragma once
#include "CommonDefine.h"
#include "ModuleTestCommonDefine.h"

class CTestController
{
public:
	CTestController();
	~CTestController();

	void MapParameters(ModuleTestInputs* inputs, ModuleTestOutputs* outputs);
	static ISpindleInterface* m_pSpindleInterface;
	static IAxisGroupInterface* m_pAxisGroupInterface;
	static ISafetyInterface* m_pSafetyInterface;

	void Run();

private:
	ModuleTestInputs* m_pInputs = nullptr;
	ModuleTestOutputs* m_pOutputs = nullptr;
	
	void AxisGroupTest();
	void SpindleTest();
	void SafetyTest();

	void ReactionToMockPanel();
	void ReactionToServoButton();
	void ReactionToSpindleButton();
	void ReactionToHandwheelButton();
	void ReactionToRecoveryButton();
	void ReactionToContinuousButton();
	void ReactionToGetParamValueButton();
	void ReactionToSetParamValueButton();
	void ReactionToPositioning();

	AxisGroupCase m_AxisGroupTestCase;
	SpindleCase m_SpindleTestCase;
	SafetyModuleCase m_SafetyTestCase;

	bool m_IsOperationAllowed = false;
	bool m_IsAxisGroupManualMoving = false;
	bool m_IsSpindleMoving = false;
	bool m_IsContinuousSelected = false;
	bool m_IsPreContinuousPassed = false;
	bool m_IsGetKpButtonSelected = false;
	bool m_IsGetKvButtonSelected = false;
	bool m_IsGetTnButtonSelected = false;
	bool m_IsSetKpButtonSelected = false;
	bool m_IsSetKvButtonSelected = false;
	bool m_IsSetTnButtonSelected = false;
	bool m_IsPositioning = false;

	// Invoke interfaces provided by Spindle module
	void TestSpindleRotate(double vel);
	void TestSpindleStop();
	void TestSpindleEnable();
	bool IsSpindleMoving();

	// Invoke interfaces provided by AxisGroup module
	void TestAxisGroupServoOn();
	void TestAxisGroupServoOff();
	void TestAxisGroupChangeOpMode(OpMode mode);
	void TestAxisGroupMove();
	void TestAxisGroupStop();
	void TestAxisGroupEnterHandwheel();
	void TestAxisGroupQuitHandwheel();
	void TestAxisGroupResetError();
	bool IsAxisGroupOpModeChanged();
	bool IsAxisGroupDisabled();
	bool IsAxisGroupManualMoving();
	void TestAxisGroupPositioning();

	bool TestGetSingleAxisKp();
	bool TestGetSingleAxisKv();
	bool TestGetSingleAxisTn();

	bool TestSetSingleAxisKp();
	bool TestSetSingleAxisTn();
	bool TestSetSingleAxisKv();

	bool IsMovingOperationAllowed();
	void UpdateManualMovingCommand();
	void UpdateContinuousMovingCommand();

	// Invoke interfaces provided by Safety module
	bool m_IsRecoverSelected = false;
	void TestSafetyEnterRecovery();
	void TestSafetyExitRecovery();
};