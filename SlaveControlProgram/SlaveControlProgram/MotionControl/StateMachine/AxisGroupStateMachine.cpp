#include "../TcPch.h"
#pragma hdrstop
#include "AxisGroupStateMachine.h"
#include "AxisGroupIdle.h"
#include "AxisGroupInitialize.h"
#include "AxisGroupDisabled.h"
#include "AxisGroupPreStandby.h"
#include "AxisGroupStandby.h"
#include "AxisGroupManualMoving.h"
#include "AxisGroupPreContinuousMoving.h"
#include "AxisGroupContinuousMoving.h"
#include "AxisGroupFault.h"
#include "AxisGroupEmergency.h"
#include "AxisGroupRecovery.h"
#include "AxisGroupLimitViolation.h"

CAxisGroupController* AxisGroupStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(AxisGroupStateMachine, AxisGroupIdle)

// Do not check under the following states: Idle, Initialize, Recovery, Fault, Emergency
void AxisGroupStateMachine::SafetyCheck()
{
    AxisGroupState requestState;
    requestState = s_pController->AxisGroupSafetyCheck();
    switch (requestState)
    {
    case AxisGroupState::eAxisGroupFault:
        transit<AxisGroupFault>();
        break;
    case AxisGroupState::eAxisGroupEmergency:
        transit<AxisGroupEmergency>();
        break;
    case AxisGroupState::eAxisGroupLimitViolation:
        transit<AxisGroupLimitViolation>();
        break;
    }

    // TODO: need further consideration, AxisGroup should be affected when Spindle was in Fault state.
    //if (s_pController->m_SpindleState == SystemState::eFault)
    //{
    //    transit<AxisGroupStandby>();
    //    // Or transit<AxisGroupEmergency>();
    //}
}
