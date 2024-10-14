#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupLimitViolation : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eLimitViolation);
    }

    void react(Cycle_Update const&) override
    {
        if (s_pController->m_bEnterRecoveryState)
        {
            transit<AxisGroupRecovery>();
        }
        else
        {
            s_pController->AxisGroupLimitViolation();
        }
    }

    void exit() override {};
};
