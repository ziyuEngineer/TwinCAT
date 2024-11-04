#pragma once
#include "SpindleStateMachine.h"

class SpindleIdle : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleIdle);
    }


    void react(EventCycleUpdate const&) override
    {
        transit<SpindleInitialize>();
    }

    void exit() override {};
};