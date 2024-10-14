#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupEmergency : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eEmergency);
    }

    void react(Cycle_Update const&) override
    {
        if (s_pController->IsResetSelected())
        {
            if (s_pController->AxisGroupDisable())
            {
                transit<AxisGroupDisabled>();
            }
        }
    }

    void exit() override {};
};