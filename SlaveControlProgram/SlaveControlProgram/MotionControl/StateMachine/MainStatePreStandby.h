#pragma once
#include "MainStateMachine.h"

class MainStatePreStandby : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::ePreStandby);
        s_pController->RequestAxisGroupServoOn();
        s_pController->RequestSpindleEnable();
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsReadyToStandby())
        {
            transit<MainStateStandby>();
        }
        else
        {
            s_pController->RequestAxisGroupServoOn();
            s_pController->RequestSpindleEnable();
        }
        // TODO: report error if unsuccessful transitions last for several cycles
    }

    void exit() override {};
};