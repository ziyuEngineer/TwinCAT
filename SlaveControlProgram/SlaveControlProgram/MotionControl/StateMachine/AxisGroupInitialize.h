#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupInitialize : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupInitialization);
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->AxisGroupInitialize())
        {
            transit<AxisGroupDisabled>();
        }
    }

    void exit() override {};
};