#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupEmergency : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupEmergency);
    }

    void react(EventCycleUpdate const&) override
    {
        
    }

    void exit() override
    {}

    void react(EventAxisGroupResetError const&) override
    {
        if (s_pController->AxisGroupDisable())
        {
            transit<AxisGroupDisabled>();
        }
    }

    void react(EventAxisGroupEnterFault const&) override
    {
        transit<AxisGroupFault>();
    }
};