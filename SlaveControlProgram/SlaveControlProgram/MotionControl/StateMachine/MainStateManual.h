#pragma once
#include "MainStateMachine.h"

class MainStateManual : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eManual);
        s_pController->DispatchEventMessage(TcEvents::MainEvent::MainModuleManual.nEventId);
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
            // update Handwheel info -- selected axis and handwheel pos
            s_pController->UpdateHandwheelInfo();
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

    void react(EventRequestEnterDisabledState const&) override // Invoked by Safety module
    {
        s_pController->DeselectAxisAutomatically();
        s_pController->DeselectServoButtonAutomatically();
        s_pController->RequestAxisGroupServoOff();
        s_pController->RequestSpindleDisable();
        transit<MainStateDisabled>();
    }
};