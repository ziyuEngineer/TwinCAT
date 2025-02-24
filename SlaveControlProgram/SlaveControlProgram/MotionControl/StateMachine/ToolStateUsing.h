#pragma once
#include "ToolStateMachine.h"

class ToolStateUsing : public ToolStateMachine
{
public:
    void entry() override
    {
        report_current_state(ToolState::eToolStateUsing);
    }

    void react(EventCycleUpdate const&) override
    {
    }

    void exit() override
    {
    }
};