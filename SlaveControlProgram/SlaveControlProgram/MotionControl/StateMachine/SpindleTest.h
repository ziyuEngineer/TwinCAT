#pragma once
#include "SpindleStateMachine.h"

class SpindleTest : public SpindleStateMachine
{
    void entry() override
    {
        report_current_state(SystemState::eTest);
    }

    void react(Cycle_Update const&) override
    {
        if (!s_pController->IsServoButtonOn())
        {
            if (s_pController->SpindleDisable())
            {
                transit<SpindleDisabled>();
            }
        }
        if (!s_pController->IsSpindleTestSelected())
        {
            transit<SpindleStandby>();
        }
        else
        {
            s_pController->SpindleTest();
        }
    }

    void exit() override {};
};