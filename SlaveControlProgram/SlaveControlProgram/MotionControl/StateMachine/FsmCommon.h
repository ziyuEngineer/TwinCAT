#pragma once
#include <tinyfsm.hpp>
#include "MotionControlServices.h"

// Common events
struct EventCycleUpdate : tinyfsm::Event 
{};

// Spindle events
// 
// Quit Disabled State and Enter Enable State
struct EventSpindleEnable : tinyfsm::Event
{};

// Quit Enable State and Enter Disabled State
struct EventSpindleDisable : tinyfsm::Event
{};

// Quit PreMoving state and Enter Rotate state
struct EventSpindleRotating : tinyfsm::Event
{
	SpindleRot spindleRot;
};

// Quit PreMoving state and Enter Locate state
struct EventSpindlePositioning : tinyfsm::Event
{
	SpindlePosition spindlePosition;
};

// Quit rotate/locate state and Enter PostMoving state
struct EventSpindleStop : tinyfsm::Event
{};

struct EventSpindleSetLimit : tinyfsm::Event
{
	SpindleVelLimit spindleVelLimit;
};

struct EventSpindleEnterFault : tinyfsm::Event
{};

struct EventSpindleResetError : tinyfsm::Event
{};

// AxisGroup
//
// Quit Disabled state and Enter PreStandby state
struct EventAxisGroupServoOn : tinyfsm::Event
{};

// Quit Standby state and Enter Disabled state
struct EventAxisGroupDisable : tinyfsm::Event
{};

// Quit Standby state and Enter ManualMoving state
struct EventAxisGroupSelectAxis : tinyfsm::Event
{};

// Quit ManualMoving state and enter Standby state
struct EventAxisGroupDeselectAxis : tinyfsm::Event
{};

// Quit Standby state and Enter Moving state
struct EventAxisGroupContinuouslyMove : tinyfsm::Event
{};

// Quit Moving state and Enter Standby state
struct EventAxisGroupStop : tinyfsm::Event
{};

struct EventAxisGroupEnterFault : tinyfsm::Event
{};

struct EventAxisGroupResetError : tinyfsm::Event
{};

struct EventAxisGroupPreMovingChangeOpMode : tinyfsm::Event
{
	OpMode requestMode;
};

struct EventAxisGroupPositioning : tinyfsm::Event
{
	bool enabled_axis[5];
	double target[5];
};

// Main State
//
// Quit Standby/Manual state and Enter Buffering state
struct EventContinuousExecution : tinyfsm::Event
{
	int minDataToStart;
	OpMode axisgroupOpMode;
};

struct EventStopContinuousMoving : tinyfsm::Event
{};

struct EventRequestEnterRecoveryState : tinyfsm::Event
{};

struct EventRequestExitRecoveryState : tinyfsm::Event
{};

struct EventRequestEnterFaultState : tinyfsm::Event
{};

struct EventRequestExitFaultState : tinyfsm::Event
{};

struct EventRequestEnterStandbyState : tinyfsm::Event
{};

struct EventRequestEnterDisabledState : tinyfsm::Event
{};

struct EventRequestAxisGroupPositioning : tinyfsm::Event
{
	bool enabled_axis[5];
	double target[5];
};

// Safety Module
struct EventRequestSafetyEnterRecoveryState : tinyfsm::Event
{};

struct EventRequestSafetyExitRecoveryState : tinyfsm::Event
{};
