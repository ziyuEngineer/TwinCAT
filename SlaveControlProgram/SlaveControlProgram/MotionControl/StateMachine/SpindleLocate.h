#pragma once
#include "SpindleStateMachine.h"

class SpindleLocate : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindlePositioning);
        s_pController->SpindleResetInterpolator(OpMode::CSP);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->SpindleMoving(m_SpindlePositionCommand);
    }

    void react(EventSpindleStop const&) override
    {
        transit<SpindlePostMoving>();
    }

    void exit() override
    {
        s_pController->SpindleStandStill();
    }

    void react(EventSpindlePositioning const& spindle_cmd) override
    {
        SetCommand(spindle_cmd.spindlePosition);
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
    SpindlePosition m_SpindlePositionCommand;

public:
    void SetCommand(SpindlePosition spindle_pos)
    {
        m_SpindlePositionCommand = spindle_pos;
    }
};