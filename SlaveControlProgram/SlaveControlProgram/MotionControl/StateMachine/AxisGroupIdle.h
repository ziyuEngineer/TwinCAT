#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupIdle : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupIdle);
    }

    void react(EventCycleUpdate const&) override
    {
        transit<AxisGroupInitialize>();
    }

    void exit() override {};
};