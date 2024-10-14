#pragma once
#include "SpindleStateMachine.h"

class SpindleDisabled : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eDisabled);
    }

    void react(Cycle_Update const&) override
    {
        if (s_pController->IsServoButtonOn())
        {
            if (s_pController->SpindleEnable())
            {
                transit<SpindleStandby>();
            }
        }
        else
        {
            s_pController->SpindleDisable();
        }
    }

    void exit() override {};
};