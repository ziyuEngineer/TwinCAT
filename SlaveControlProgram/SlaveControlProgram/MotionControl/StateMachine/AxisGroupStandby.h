#pragma once
#include "AxisGroupStateMachine.h"
#include "AxisGroupPreContinuousMoving.h"
#include "AxisGroupPositioning.h"

class AxisGroupStandby : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupStandby);
    }

    void react(EventCycleUpdate const&) override
    {
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

    void react(EventAxisGroupEnterFault const&) override
    {
        transit<AxisGroupFault>();
    }

    void react(EventAxisGroupPositioning const& event) override
    {
        state<AxisGroupPositioning>().SetMovingTarget(event.enabled_axis, event.target);
        transit<AxisGroupPositioning>();
    }
};