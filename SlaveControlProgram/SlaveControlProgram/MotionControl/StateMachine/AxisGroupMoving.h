#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupMoving : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eMoving);
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
        else if (s_pController->m_bMovingFinish)
        {
            transit<AxisGroupStandby>();
        }
        else
        {
            s_pController->AxisGroupMoving();
        }
    }

    void exit() override {};
};