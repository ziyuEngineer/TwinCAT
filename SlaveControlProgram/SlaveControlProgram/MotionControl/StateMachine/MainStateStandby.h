#pragma once
#include "MainStateMachine.h"
#include "MainStateBuffering.h"

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

        if (s_pController->IsManualModeSelected())
        {
            transit<MainStateManual>();
        }

        int spindle_moving_dir = s_pController->IsSpindleMovingPressed();

        if (spindle_moving_dir != 0 && m_LastSpindleDir != spindle_moving_dir) // react when spindle CW or CCW button is pressed
        {
            m_LastSpindleDir = spindle_moving_dir;
            s_pController->RequestSpindleRotate(m_SpindleRotatingVel * spindle_moving_dir);
        }
        else if(spindle_moving_dir == 0 && m_LastSpindleDir != 0)
        {
            m_LastSpindleDir = spindle_moving_dir;
            s_pController->RequestSpindleStop();
        }
    }

    void react(EventContinuousExecution const& event) override
    {
        state<MainStateBuffering>().SetMinDataLengthToStart(event.minDataToStart);
        state<MainStateBuffering>().SetAxisGroupOpMode(event.axisgroupOpMode);
        transit<MainStateBuffering>();
    }

    void exit() override
    {
        if (s_pController->IsSpindleMovingPressed() != 0)
        {
            s_pController->RequestSpindleStop();
        }
    }

protected:
    double m_SpindleRotatingVel = 20.0; // Default speed, unit rps
    double m_LastSpindleDir = 0;
};