#include "../TcPch.h"
#pragma hdrstop
#include "AxisGroupStateMachine.h"
#include "AxisGroupIdle.h"
#include "AxisGroupInitialize.h"
#include "AxisGroupDisabled.h"
#include "AxisGroupStandby.h"
#include "AxisGroupHandwheel.h"
#include "AxisGroupMoving.h"
#include "AxisGroupTest.h"
#include "AxisGroupFault.h"
#include "AxisGroupEmergency.h"
#include "AxisGroupRecovery.h"
#include "AxisGroupLimitViolation.h"

CFiveAxisController* AxisGroupStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(AxisGroupStateMachine, AxisGroupIdle)

// Do not check under the following states: Idle, Initialize, Recovery, Fault, Emergency
void AxisGroupStateMachine::SafetyCheck()
{
    SystemState requestState;
    requestState = s_pController->AxisGroupSafetyCheck();
    switch (requestState)
    {
    case SystemState::eFault:
        transit<AxisGroupFault>();
        break;
    case SystemState::eEmergency:
        transit<AxisGroupEmergency>();
        break;
    case SystemState::eLimitViolation:
        transit<AxisGroupLimitViolation>();
        break;
    }

    // TODO: need further consideration, AxisGroup should be affected when Spindle was in Fault state.
    if (s_pController->m_SpindleState == SystemState::eFault)
    {
        transit<AxisGroupStandby>();
        // Or transit<AxisGroupEmergency>();
    }
}
