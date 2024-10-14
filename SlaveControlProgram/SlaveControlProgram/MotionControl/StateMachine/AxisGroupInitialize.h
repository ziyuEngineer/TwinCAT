#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupInitialize : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eInitialization);
    }

    void react(Cycle_Update const&) override
    {
        if (s_pController->AxisGroupInitialize())
        {
            transit<AxisGroupDisabled>();
        }
    }

    void exit() override {};
};