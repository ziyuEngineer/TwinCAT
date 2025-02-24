#pragma once
#include "ToolStateMachine.h"

class ToolStateFault : public ToolStateMachine
{
public:
    void entry() override
    {
        report_current_state(ToolState::eToolStateFault);
    }

    void react(EventCycleUpdate const&) override
    {
    }

    void exit() override
    {
    }
};