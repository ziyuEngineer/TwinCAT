#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupIdle : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eIdle);
    }

    void react(Cycle_Update const&) override
    {
        transit<AxisGroupInitialize>();
    }

    void exit() override {};
};