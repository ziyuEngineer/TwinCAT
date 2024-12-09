#pragma once

#include "../ModuleTestServices.h"
#include "../ModuleTestInterfaces.h"

enum class AxisGroupCase
{
	AxisGroupNull = 0,
	AxisGroupServoOn = 1,
	AxisGroupServoOff = 2,
	AxisGroupEnterHandwheel = 3,
	AxisGroupQuitHandwheel = 4,
	AxisGroupContinuouslyMoving = 5,
	AxisGroupStopContinuouslyMoving = 6
};

enum class SpindleCase
{
	SpindleNull = 0,
	SpindleEnable = 1,
	SpindleRotating = 2,
	SpindleLocating = 3,
	SpindleStopMoving = 4
};

enum class SafetyModuleCase
{
	SafetyNull = 0,
	SafetyEnterRecovery = 1,
	SafetyExitRecovery = 2
};