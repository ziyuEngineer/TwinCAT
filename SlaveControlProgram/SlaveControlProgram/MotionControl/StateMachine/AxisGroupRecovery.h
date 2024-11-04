#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupRecovery : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupRecovery);
    }

    void react(EventCycleUpdate const&) override
    {
       s_pController->AxisGroupRecovery();
    }

    void exit() override 
    {
        
    };

    void react(EventAxisGroupExitRecoveryState const&) override
    {
        transit<AxisGroupStandby>();
    };
};