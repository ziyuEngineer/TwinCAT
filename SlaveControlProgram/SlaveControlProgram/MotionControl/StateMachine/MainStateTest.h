#pragma once
#include "MainStateMachine.h"

class MainStateTest : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eTest);
    }


    void react(EventCycleUpdate const&) override
    {

    }

    void exit() override {};
};