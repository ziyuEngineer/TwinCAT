#pragma once
#include "SafetyStateMachine.h"

class SafetyStateFault : public SafetyStateMachine
{
public:
    void entry() override
    {
        report_current_state(SafetyState::eSafetyStateFault);
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsFaultCheckPassed() && s_pController->IsSubSystemStateCorrect())
        {
            s_pController->RequestClearFault();
            transit<SafetyStateNormal>();
        }
    }

    void exit() override
    {
        s_pController->ClearErrorCode();
    }
};