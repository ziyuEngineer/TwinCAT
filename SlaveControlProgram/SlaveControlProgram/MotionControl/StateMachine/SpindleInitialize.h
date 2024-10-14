#pragma once
#include "SpindleStateMachine.h"

class SpindleInitialize : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eInitialization);
    }

    void react(Cycle_Update const&) override
    {
        if (s_pController->SpindleInitialize())
        {
            transit<SpindleDisabled>();
        }
    }

    void exit() override {};
};