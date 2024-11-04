#pragma once
#include "MainStateMachine.h"

class MainStateManual : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eManual);
        s_pController->RequestAxisGroupEnterHandwheel();
    }

    void react(EventCycleUpdate const&) override
    {
        if (!s_pController->IsManualModeSelected())
        {
            transit<MainStateStandby>();
        }
        else
        {
            // update Handwheelmoving pos
            s_pController->UpdateManualMovingCommand();
        }
    }

    void exit() override
    {
        s_pController->RequestAxisGroupQuitHandwheel();
    }

    void react(EventContinuousExecution const& event) override
    {
        state<MainStateBuffering>().SetMinDataLengthToStart(event.minDataToStart);
        state<MainStateBuffering>().SetAxisGroupOpMode(event.axisgroupOpMode);
        transit<MainStateBuffering>();
    }
};