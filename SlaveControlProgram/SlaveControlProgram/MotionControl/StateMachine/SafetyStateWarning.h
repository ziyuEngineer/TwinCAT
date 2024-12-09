#pragma once
#include "SafetyStateMachine.h"

class SafetyStateWarning : public SafetyStateMachine
{
public:
    void entry() override
    {
        report_current_state(SafetyState::eSafetyStateWarning);
    }

    void react(EventCycleUpdate const&) override
    {
        FaultCheck();

        EmergencyCheck();
    }

    void exit() override {};

    void react(EventRequestSafetyEnterRecoveryState const&) override
    {
        transit<SafetyStateRecovery>();
    }
};