#pragma once
#include "SpindleStateMachine.h"

class SpindleStandby : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eStandby);
    }

    void react(Cycle_Update const&) override
    {
        if (!s_pController->IsServoButtonOn())
        {
            if (s_pController->SpindleDisable())
            {
                transit<SpindleDisabled>();
            }
        }
        else if (s_pController->m_bMovingStart)
        {
            transit<SpindleMoving>();
        }
        else if (s_pController->IsSpindleTestSelected())
        {
            transit<SpindleTest>();
        }
        else
        {
            s_pController->SpindleStandby();
        }
    }
};