#pragma once
#include "SafetyStateMachine.h"

class SafetyStateRecovery : public SafetyStateMachine
{
public:
    void entry() override
    {
        report_current_state(SafetyState::eSafetyStateRecovery);
        s_pController->RequestEnterRecovery();
    }

    void react(EventCycleUpdate const&) override
    {
        FaultCheck();

        RecoveryCheck();
    }

    void exit() override
    {
        s_pController->ClearErrorCode();
    }

    void react(EventRequestSafetyExitRecoveryState const&) override
    {
        s_pController->RequestExitRecovery();
        transit<SafetyStateNormal>();
    }
};