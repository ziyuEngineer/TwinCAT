#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupPreStandby : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupPreStandby);
        s_pController->AxisGroupEnable();
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsAxisGroupEnabled())
        {
            transit<AxisGroupStandby>();
        }
        else
        {
            s_pController->AxisGroupEnable();
        }
    }

    void exit() override
    {}

};