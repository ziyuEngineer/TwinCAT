#pragma once
#include "SpindleStateMachine.h"

class SpindlePostMoving : public SpindleStateMachine
{
public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindlePostMoving);
        s_pController->SpindleBrake();
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsSpindleMotionless())
        {
            if (s_pController->SpindleDisable())
            {
                transit<SpindleEnable>();
            }
        }
        else
        {
            s_pController->SpindleBrake();
        }
    }

    void exit() override {};
};