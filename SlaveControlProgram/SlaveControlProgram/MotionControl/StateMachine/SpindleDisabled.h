#pragma once
#include "SpindleStateMachine.h"

class SpindleDisabled : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleDisabled);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->SpindleDisable();
    }

    void exit() override {};

    void react(EventSpindleSetLimit const& spindle_limit) override
    {
        s_pController->SetSpindleVelLimit(spindle_limit.spindleVelLimit);
    }

    void react(EventSpindleEnable const&) override
    {
        transit<SpindleEnable>();
    }
};