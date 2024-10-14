#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupHandwheel : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eHandwheel);
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
        else if (!s_pController->IsHandwheelStateSelected())
        {
            transit<AxisGroupStandby>();
        }
        else
        {
            s_pController->AxisGroupHandwheel();
        }
    }

    void exit() override 
    {
        s_pController->HandwheelCommandReset();
    };
};