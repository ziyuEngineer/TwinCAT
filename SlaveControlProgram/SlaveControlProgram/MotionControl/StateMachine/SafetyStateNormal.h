#pragma once
#include "SafetyStateMachine.h"

class SafetyStateNormal : public SafetyStateMachine
{
public:
    void entry() override
    {
        report_current_state(SafetyState::eSafetyStateNormal);
    }

    void react(EventCycleUpdate const&) override
    {
        FaultCheck();

        EmergencyCheck();

        WarningCheck();
    }

    void exit() override {};
};