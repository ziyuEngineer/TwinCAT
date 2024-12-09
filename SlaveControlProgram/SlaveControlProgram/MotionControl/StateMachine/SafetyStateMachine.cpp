#include "../TcPch.h"
#pragma hdrstop
#include "SafetyStateMachine.h"

#include "SafetyStateNormal.h"
#include "SafetyStateWarning.h"
#include "SafetyStateEmergency.h"
#include "SafetyStateFault.h"
#include "SafetyStateRecovery.h"

CSafetyController* SafetyStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(SafetyStateMachine, SafetyStateNormal)

void SafetyStateMachine::FaultCheck()
{
    if (!s_pController->IsFaultCheckPassed())
    {
        s_pController->ReportFault();
        transit<SafetyStateFault>();
    }
}

void SafetyStateMachine::EmergencyCheck()
{
    if (!s_pController->IsEmergencyCheckPassed())
    {
        s_pController->ReportEmergency();
        transit<SafetyStateEmergency>();
    }
}

void SafetyStateMachine::WarningCheck()
{
    if (!s_pController->IsWarningCheckPassed())
    {
        s_pController->ReportWarning();
        transit<SafetyStateWarning>();
    }
}

void SafetyStateMachine::RecoveryCheck()
{
    if (!s_pController->IsRecoveryCheckPassed())
    {
        s_pController->ReportWarning();
        transit<SafetyStateWarning>();
    }
}
