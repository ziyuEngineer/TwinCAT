#pragma once
#include "SpindleStateMachine.h"

class SpindleIdle : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eIdle);
    }


    void react(Cycle_Update const&) override
    {
        transit<SpindleInitialize>();
    }

    void exit() override {};
};