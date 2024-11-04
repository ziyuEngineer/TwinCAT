#pragma once
#include "MainStateMachine.h"

class MainStateDisabled : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eDisabled);
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsServoOnPressed())
        {
            transit<MainStatePreStandby>();
        }
    }

    void exit() override {};
};