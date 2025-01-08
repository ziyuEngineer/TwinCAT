#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupContinuousMoving : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupContinuousMoving);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->AxisGroupMoving();
    }

    void exit() override 
    {
        s_pController->StopComputingTuningError();
        s_pController->AxisGroupStandStill();
    };

    void react(EventAxisGroupStop const&) override
    {
        transit<AxisGroupStandby>();
    }

    void react(EventAxisGroupEnterFault const&) override
    {
        transit<AxisGroupFault>();
    }

    void react(EventAxisGroupDisable const&) override
    {
        transit<AxisGroupDisabled>();
    }
};