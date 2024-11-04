#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupLimitViolation : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupLimitViolation);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->AxisGroupLimitViolation();
    }

    void exit() override {};

    void react(EventAxisGroupEnterRecoveryState const&) override
    {
        transit<AxisGroupRecovery>();
    };
};
