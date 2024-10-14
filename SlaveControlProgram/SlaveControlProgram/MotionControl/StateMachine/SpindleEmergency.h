#pragma once
#include "SpindleStateMachine.h"

class SpindleEmergency : public SpindleStateMachine
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
            if (s_pController->SpindleDisable())
            {
                transit<SpindleDisabled>();
            }
        }
    }

    void exit() override {};
};