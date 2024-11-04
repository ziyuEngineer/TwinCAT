#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupDisabled : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupDisabled);
    }

    void react(EventCycleUpdate const&) override
    {
        SafetyCheck();

        s_pController->AxisGroupDisable();
    }

    void exit() override {};

    void react(EventAxisGroupServoOn const&) override
    {
        transit<AxisGroupPreStandby>();
    }
};