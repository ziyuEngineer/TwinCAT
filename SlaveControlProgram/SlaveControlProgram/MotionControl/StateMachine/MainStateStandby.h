#pragma once
#include "MainStateMachine.h"
#include "MainStateBuffering.h"
#include "MainStateFault.h"
#include "MainStateToolSwitching.h"

class MainStateStandby : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eStandby);
    }

    void react(EventCycleUpdate const&) override
    {
        if (!s_pController->IsServoOnPressed())
        {
            s_pController->RequestAxisGroupServoOff();
            if (s_pController->IsAxisGroupDisabled())
            {
                transit<MainStateDisabled>();
            }
        }

        if (s_pController->IsManualModeSelected() && IsSafeToTransitState())
        {
            transit<MainStateManual>();
        }

        int spindle_moving_dir = s_pController->IsSpindleMovingPressed();

        double spindle_rotating_vel = m_SpindleRotatingMaxVel * s_pController->GetSpindleRatio();

        if (spindle_moving_dir != 0 && (m_LastSpindleDir != spindle_moving_dir || m_LastSpindleVel != spindle_rotating_vel)) // react when spindle CW or CCW button is pressed
        {
            m_LastSpindleDir = spindle_moving_dir;
            m_LastSpindleVel = spindle_rotating_vel;
            s_pController->RequestSpindleRotate(spindle_rotating_vel * spindle_moving_dir);
        }
        else if(spindle_moving_dir == 0 && m_LastSpindleDir != 0)
        {
            m_LastSpindleDir = spindle_moving_dir;
            s_pController->RequestSpindleStop();
        }
    }

    void react(EventContinuousExecution const& event) override
    {
        if (IsSafeToTransitState())
        {
            state<MainStateBuffering>().SetMinDataLengthToStart(event.minDataToStart);
            state<MainStateBuffering>().SetAxisGroupOpMode(event.axisgroupOpMode);
            transit<MainStateBuffering>();
        }
    }

    void react(EventRequestEnterFaultState const&) override
    {
        transit<MainStateFault>();
    }

    void react(EventRequestEnterRecoveryState const&) override
    {
        transit<MainStateRecovery>();
    }

    void react(EventRequestEnterDisabledState const&) override // Invoked by Safety module
    {
        s_pController->DeselectServoButtonAutomatically();
        s_pController->RequestAxisGroupServoOff();
        s_pController->RequestSpindleDisable();
        transit<MainStateDisabled>();
    }

    void react(EventRequestAxisGroupPositioning const& event) override
    {
        bool enabled_axis[5]{};

        double pos[5]{};

        for (int i = 0; i < kMaxAxisNum; i++)
        {
            enabled_axis[i] = event.enabled_axis[i];
            pos[i] = event.target[i];
        }
        s_pController->RequestAxisGroupPositioning(enabled_axis, pos);
    }

    void react(EventRequestToolSwitch const& event) override
    {
        if (state<MainStateToolSwitching>().GetCurrentToolNumber() != event.tool_number)
        {
            state<MainStateToolSwitching>().SetToolNumber(event.tool_number);
            transit<MainStateToolSwitching>();
        }
    }

    void exit() override
    {
        if (s_pController->IsSpindleMovingPressed() != 0)
        {
            s_pController->RequestSpindleStop();
        }
    }

protected:
    double m_SpindleRotatingMaxVel = 50.0; // Max speed, unit rps
    double m_LastSpindleDir = 0;
    double m_LastSpindleVel = 0.0;
};