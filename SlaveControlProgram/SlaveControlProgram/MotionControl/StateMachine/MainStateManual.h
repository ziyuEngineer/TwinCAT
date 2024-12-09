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
        s_pController->DeselectAxisAutomatically();
        state<MainStateBuffering>().SetMinDataLengthToStart(event.minDataToStart);
        state<MainStateBuffering>().SetAxisGroupOpMode(event.axisgroupOpMode);
        transit<MainStateBuffering>();
    }

    void react(EventRequestEnterFaultState const&) override // Invoked by Safety module
    {
        s_pController->DeselectAxisAutomatically();
        transit<MainStateFault>();
    }

    void react(EventRequestEnterStandbyState const&) override // Invoked by Safety module
    {
        s_pController->DeselectAxisAutomatically();
        transit<MainStateStandby>();
    }

    void react(EventRequestEnterDisabledState const&) override // Invoked by Safety module
    {
        s_pController->DeselectAxisAutomatically();
        s_pController->DeselectServoButtonAutomatically();
        s_pController->RequestAxisGroupServoOff();
        s_pController->RequestSpindleDisable();
        transit<MainStateDisabled>();
    }
};