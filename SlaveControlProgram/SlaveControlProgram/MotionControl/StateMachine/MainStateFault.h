#pragma once
#include "MainStateMachine.h"

class MainStateFault : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eFault);
    }


    void react(EventCycleUpdate const&) override
    {

    }

    void exit() override {};
};