#include "../TcPch.h"
#pragma hdrstop
#include "AxisGroupStateMachine.h"
#include "AxisGroupIdle.h"
#include "AxisGroupInitialize.h"
#include "AxisGroupDisabled.h"
#include "AxisGroupPreStandby.h"
#include "AxisGroupStandby.h"
#include "AxisGroupManualMoving.h"
#include "AxisGroupPositioning.h"
#include "AxisGroupPreContinuousMoving.h"
#include "AxisGroupContinuousMoving.h"
#include "AxisGroupFault.h"
#include "AxisGroupEmergency.h"

CAxisGroupController* AxisGroupStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(AxisGroupStateMachine, AxisGroupIdle)
