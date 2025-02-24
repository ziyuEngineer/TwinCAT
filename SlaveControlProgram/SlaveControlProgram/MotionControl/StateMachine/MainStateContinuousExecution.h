#pragma once
#include "MainStateMachine.h"

class MainStateContinuousExecution : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eContinuousExecution);
        s_pController->DispatchEventMessage(TcEvents::MainEvent::MainModuleMoving.nEventId);
        s_pController->RequestAxisGroupMove();
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->RingBufferInput();
        s_pController->RingBufferOutput();

        bool is_command_valid = s_pController->RingBufferDispatchCommand(); 
        
        if (!is_command_valid)
        {
            s_pController->QuitMachiningWithWarning();
            transit<MainStateStandby>();
        }
        
        if (s_pController->IsMovingFinished())
        {
            s_pController->QuitMachiningNormally();
            transit<MainStateStandby>();
        }
    }

    void react(EventStopContinuousMoving const&) override // Invoked by Host
    {
        s_pController->QuitMachiningNormally();
        transit<MainStateStandby>();
    }

    void react(EventRequestEnterFaultState const&) override // Invoked by Safety module 
    {
        s_pController->EmptyRingBuffer();
        transit<MainStateFault>();
    }

    void react(EventRequestEnterStandbyState const&) override // Invoked by Safety module
    {
        s_pController->EmptyRingBuffer();
        transit<MainStateStandby>();
    }

    void react(EventRequestEnterDisabledState const&) override // Invoked by Safety module
    {
        s_pController->EmptyRingBuffer();
        s_pController->DeselectServoButtonAutomatically();
        s_pController->RequestAxisGroupServoOff();
        s_pController->RequestSpindleDisable();
        transit<MainStateDisabled>();
    }

    void exit() override
    {
    }
};