#pragma once
#include "SafetyStateMachine.h"

class SafetyStateEmergency : public SafetyStateMachine
{
public:
    void entry() override
    {
        report_current_state(SafetyState::eSafetyStateEmergency);
    }

    void react(EventCycleUpdate const&) override
    {
        FaultCheck();
    }

    void exit() override
    {}

    void react(EventRequestSafetyEnterRecoveryState const&) override
    {
        transit<SafetyStateRecovery>();
    }
};