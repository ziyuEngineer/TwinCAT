#pragma once
#include "SpindleStateMachine.h"
#include "SpindlePreMoving.h"

class SpindleEnable : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleEnable);
    }

    void react(EventCycleUpdate const&) override
    {
        //if (s_pController->SpindleDisable()) // double check to ensure that driver has been disabled
        //{
        //    transit<SpindleDisabled>();
        //}
    }

    void react(EventSpindleRotating const & spindle_cmd) override
    {
        state<SpindlePreMoving>().SetNextMovingState(SpindleMovingState::eSpindleRotate);
        state<SpindlePreMoving>().SetNextMovingCmd(spindle_cmd.spindleRot);
        transit<SpindlePreMoving>();
    }

    void react(EventSpindlePositioning const& spindle_cmd) override
    {
        state<SpindlePreMoving>().SetNextMovingState(SpindleMovingState::eSpindleLocate);
        state<SpindlePreMoving>().SetNextMovingCmd(spindle_cmd.spindlePosition);
        transit<SpindlePreMoving>();
    }

    void exit() override {};
};