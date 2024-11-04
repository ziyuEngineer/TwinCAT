#pragma once
#include "SpindleStateMachine.h"

class SpindleInitialize : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleInitialization);
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->SpindleInitialize())
        {
            transit<SpindleDisabled>();
        }
    }

    void exit() override {};
};