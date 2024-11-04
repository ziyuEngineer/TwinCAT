#pragma once
#include "MainStateMachine.h"

class MainStateRecovery : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eRecovery);
    }


    void react(EventCycleUpdate const&) override
    {

    }

    void exit() override {};
};