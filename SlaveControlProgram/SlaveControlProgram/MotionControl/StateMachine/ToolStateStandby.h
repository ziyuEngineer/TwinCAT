#pragma once
#include "ToolStateMachine.h"

class ToolStateStandby : public ToolStateMachine
{
public:
    void entry() override
    {   
        report_current_state(ToolState::eToolStateStandby);
    }

    void react(EventCycleUpdate const&) override
    {   
    }

    void exit() override
    {
    }
};