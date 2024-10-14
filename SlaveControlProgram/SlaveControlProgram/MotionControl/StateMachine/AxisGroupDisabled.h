#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupDisabled : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eDisabled);
    }

    void react(Cycle_Update const&) override
    {
        SafetyCheck();

        if (s_pController->IsServoButtonOn())
        {
            if (s_pController->AxisGroupEnable())
            {
                transit<AxisGroupStandby>();
            }
        }
        else
        {
            s_pController->AxisGroupDisable();
        }
    }

    void exit() override {};
};