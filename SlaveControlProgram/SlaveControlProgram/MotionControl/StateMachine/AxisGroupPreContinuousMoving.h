#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupPreContinuousMoving : public AxisGroupStateMachine
{
protected:
    OpMode m_MovingOpMode;
    bool m_IsAxisGroupOpModeSwitched = false;

public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupPreContinuousMoving);
        s_pController->AxisGroupSwitchOpMode(m_MovingOpMode);
        m_IsAxisGroupOpModeSwitched = s_pController->IsAxisGroupOpModeSwitched();
    }

    void react(EventCycleUpdate const&) override
    {
        if (!m_IsAxisGroupOpModeSwitched)
        {
            s_pController->AxisGroupSwitchOpMode(m_MovingOpMode);
            m_IsAxisGroupOpModeSwitched = s_pController->IsAxisGroupOpModeSwitched();
        }
    }

    void exit() override
    {
        m_IsAxisGroupOpModeSwitched = false;
    }

    void react(EventAxisGroupContinuouslyMove const&) override
    {
        if (m_IsAxisGroupOpModeSwitched)
        {
            transit<AxisGroupContinuousMoving>();
        }
    }

    void SetMovingOpMode(OpMode mode)
    {
        m_MovingOpMode = mode;
    }

    void react(EventAxisGroupEnterFault const&) override
    {
        transit<AxisGroupFault>();
    }
};
