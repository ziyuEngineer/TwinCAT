#pragma once
#include "ToolStateMachine.h"

class ToolStateSwitching : public ToolStateMachine
{
public:
    void entry() override
    {
        report_current_state(ToolState::eToolStateSwitching);
    }

    void react(EventCycleUpdate const&) override
    {
    }

    void exit() override
    {
    }
};