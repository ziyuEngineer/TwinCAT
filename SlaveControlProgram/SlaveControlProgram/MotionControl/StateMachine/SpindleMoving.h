#pragma once
#include "SpindleStateMachine.h"

class SpindleMoving : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eMoving);
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
        else if (s_pController->m_bMovingFinish)
        {
            transit<SpindleStandby>();
        }
        else
        {
            s_pController->SpindleMoving();
        }
    }

    void exit() override {};
};