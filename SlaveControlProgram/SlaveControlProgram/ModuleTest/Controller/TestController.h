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

	void Run();

private:
	ModuleTestInputs* m_pInputs = nullptr;
	ModuleTestOutputs* m_pOutputs = nullptr;
	
	void AxisGroupTest();
	void SpindleTest();

	void ReactionToMockPanel();
	void ReactionToServoButton();
	void ReactionToSpindleButton();
	void ReactionToHandwheelButton();

	AxisGroupCase m_AxisGroupTestCase;
	SpindleCase m_SpindleTestCase;

	bool m_IsOperationAllowed = false;
	bool m_IsAxisGroupManualMoving = false;
	bool m_IsSpindleMoving = false;

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

	bool IsMovingOperationAllowed();
	void UpdateManualMovingCommand();
};