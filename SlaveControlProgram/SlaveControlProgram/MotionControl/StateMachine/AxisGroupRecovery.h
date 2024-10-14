#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupRecovery : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eRecovery);
    }

    void react(Cycle_Update const&) override
    {
        if (!s_pController->m_bEnterRecoveryState)
        {
            transit<AxisGroupStandby>();
        }
        else
        {
            s_pController->AxisGroupRecovery();
        }
    }

    void exit() override 
    {
        s_pController->HandwheelCommandReset();
    };
};