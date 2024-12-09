#pragma once
#include "SpindleStateMachine.h"

class SpindleRotate : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleRotating);
        s_pController->SpindleResetInterpolator(OpMode::CSV);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->SpindleMoving(m_SpindleRotateCommand);
    }

    void react(EventSpindleStop const&) override
    {
        transit<SpindlePostMoving>();
    }

    void exit() override 
    {
        s_pController->SpindleStandStill();
    }

    void react(EventSpindleRotating const& spindle_cmd) override
    {
        SetCommand(spindle_cmd.spindleRot);
    }

    void react(EventSpindleEnterFault const&) override
    {
        transit<SpindleFault>();
    }

    void react(EventSpindleDisable const&) override
    {
        transit<SpindlePostMoving>();
    }

protected:
    SpindleRot m_SpindleRotateCommand;

public:
    void SetCommand(SpindleRot spindle_vel)
    {
        m_SpindleRotateCommand = spindle_vel;
    }
};