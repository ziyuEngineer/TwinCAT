#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupStandby : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eStandby);
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
        else if (s_pController->IsHandwheelStateSelected())
        {
            transit<AxisGroupHandwheel>();
        }
        else if (s_pController->m_bMovingStart)
        {
            transit<AxisGroupMoving>();
        }
        else if (s_pController->IsTestStateSelected())
        {
            transit<AxisGroupTest>();
        }
        else
        {
            s_pController->AxisGroupStandby();
        }
    }
};