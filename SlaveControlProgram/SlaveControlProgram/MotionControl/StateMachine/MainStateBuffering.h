#pragma once
#include "MainStateMachine.h"

class MainStateBuffering : public MainStateMachine
{
protected:
    ULONG m_MinDataLengthToStart;
    OpMode m_RequestAxisGroupOpMode;
    bool m_IsAxisGroupOpModeSwitched = false;

public:
    void entry() override
    {
        report_current_state(SystemState::eBuffering);
        s_pController->DispatchEventMessage(TcEvents::MainEvent::MainModuleBuffering.nEventId);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->RingBufferInput();
        s_pController->RingBufferOutput();

        if (s_pController->IsExecutionPressed() && IsCommandEnoughToStartMoving())
        {
            if (!m_IsAxisGroupOpModeSwitched)
            {
                s_pController->RequestAxisGroupChangeOpMode(m_RequestAxisGroupOpMode);
                m_IsAxisGroupOpModeSwitched = s_pController->IsAxisGroupOpModeChanged();
            }
            else
            {
                transit<MainStateContinuousExecution>();
            }
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

    void exit() override
    {
        m_IsAxisGroupOpModeSwitched = false;
    }

    void SetMinDataLengthToStart(ULONG min_length)
    {
        m_MinDataLengthToStart = min_length;
    }

    bool IsCommandEnoughToStartMoving()
    {
        return ((s_pController->GetCurrentRingBufferSize() >= m_MinDataLengthToStart) || s_pController->IsLatestCommandStop());
    }

    void SetAxisGroupOpMode(OpMode op_mode)
    {
        m_RequestAxisGroupOpMode = op_mode;
    }

};