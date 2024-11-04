#include "../TcPch.h"
#pragma hdrstop
#include "SpindleStateMachine.h"
#include "SpindleIdle.h"
#include "SpindleInitialize.h"
#include "SpindleDisabled.h"
#include "SpindleEnable.h"
#include "SpindlePreMoving.h"
#include "SpindleRotate.h"
#include "SpindleLocate.h"
#include "SpindlePostMoving.h"
#include "SpindleFault.h"
#include "SpindleEmergency.h"

CSpindleController* SpindleStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(SpindleStateMachine, SpindleIdle)

void SpindleStateMachine::SafetyCheck()
{
    // TODO: need further consideration, Spindle should be affected when AxisGroup was in Fault state.
    //if (s_pController->m_AxisGroupState == SystemState::eFault)
    //{
    //    transit<SpindleStandby>();
    //    // Or transit<SpindleEmergency>();
    //}
}