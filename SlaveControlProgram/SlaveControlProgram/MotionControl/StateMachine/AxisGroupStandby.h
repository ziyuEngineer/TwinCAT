#pragma once
#include "AxisGroupStateMachine.h"
#include "AxisGroupPreContinuousMoving.h"

class AxisGroupStandby : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupStandby);
    }

    void react(EventCycleUpdate const&) override
    {
        SafetyCheck();

        s_pController->AxisGroupStandStill();
    }

    void exit() override
    {}

    void react(EventAxisGroupDisable const&) override
    {
        transit<AxisGroupDisabled>();
    }

    void react(EventAxisGroupSelectAxis const& ) override
    {
        transit<AxisGroupManualMoving>();
    }

    void react(EventAxisGroupPreMovingChangeOpMode const& event) override
    {
        state<AxisGroupPreContinuousMoving>().SetMovingOpMode(event.requestMode);
        transit<AxisGroupPreContinuousMoving>();
    }
};