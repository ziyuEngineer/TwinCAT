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

// Quit Recovery state and Enter Standby state 
struct EventAxisGroupExitRecoveryState : tinyfsm::Event
{};

// Quit LimitViolation state and Enter Recovery state 
struct EventAxisGroupEnterRecoveryState : tinyfsm::Event
{};

struct EventAxisGroupResetError : tinyfsm::Event
{};

struct EventAxisGroupPreMovingChangeOpMode : tinyfsm::Event
{
	OpMode requestMode;
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