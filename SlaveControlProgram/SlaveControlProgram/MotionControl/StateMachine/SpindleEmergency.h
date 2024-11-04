#pragma once
#include "SpindleStateMachine.h"

class SpindleEmergency : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindleEmergency);
    }

    void react(EventCycleUpdate const&) override
    {
        
    }

    void exit() override {};
};