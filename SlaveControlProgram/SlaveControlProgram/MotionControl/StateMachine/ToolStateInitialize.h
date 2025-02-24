#pragma once
#include "ToolStateMachine.h"

class ToolStateInitialize : public ToolStateMachine
{
public:
    void entry() override
    {
        report_current_state(ToolState::eToolStateInitialize);
    }

    void react(EventCycleUpdate const&) override
    {
        // Test state machine switch
        transit<ToolStateStandby>();
    }

    void exit() override
    {
    }
};