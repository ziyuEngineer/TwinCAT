#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupTest : public AxisGroupStateMachine
{
    void entry() override
    {
        report_current_state(SystemState::eTest);
    }

    void react(Cycle_Update const&) override
    {
        SafetyCheck();

        if (!s_pController->IsServoButtonOn())
        {
            if (s_pController->AxisGroupDisable())
            {
                transit<AxisGroupDisabled>();
            }
        }
        if (!s_pController->IsTestStateSelected())
        {
            transit<AxisGroupStandby>();
        }
        else
        {
            s_pController->AxisGroupTest();
        }
    }

    void exit() override {};
};