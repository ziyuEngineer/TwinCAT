#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupPositioning : public AxisGroupStateMachine
{
protected:
    bool m_IsAxisGroupInCSP = false;
    bool m_EnabledMovingAxis[5];
    double m_TargetPos[5];
    double m_PositioningTolerance = 0.01;

public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupPositioning);
        s_pController->AxisGroupSwitchOpMode(OpMode::CSP);
        s_pController->AxisGroupResetInterpolator(OpMode::CSP);
        m_IsAxisGroupInCSP = s_pController->IsAxisGroupOpModeSwitched();
    }

    void react(EventCycleUpdate const&) override
    {
        if (m_IsAxisGroupInCSP)
        {
            if (IsPositioningFinished())
            {
                transit<AxisGroupStandby>();
            }
            else
            {
                s_pController->AxisGroupPositioning(m_EnabledMovingAxis, m_TargetPos);
            }
        }
        else
        {
            s_pController->AxisGroupSwitchOpMode(OpMode::CSP);
            m_IsAxisGroupInCSP = s_pController->IsAxisGroupOpModeSwitched();
        }
    }

    void exit() override
    {
        m_IsAxisGroupInCSP = false;
    }

    void react(EventAxisGroupPositioning const& event) override
    {
        SetMovingTarget(event.enabled_axis, event.target);
    }

    void react(EventAxisGroupEnterFault const&) override
    {
        transit<AxisGroupFault>();
    }

    void react(EventAxisGroupDisable const&) override
    {
        transit<AxisGroupDisabled>();
    }

    void SetMovingTarget(const bool moving_axis[5], const double target[5])
    {
        for (int i = 0; i < kMaxAxisNum; i++)
        {
            m_EnabledMovingAxis[i] = moving_axis[i];
            m_TargetPos[i] = target[i];
        }
    }

    bool IsPositioningFinished()
    {
        for (int i = 0; i < kMaxAxisNum; i++)
        {
            if (m_EnabledMovingAxis[i])
            {
                if (fabs_(m_TargetPos[i] - s_pController->GetSingleAxisPosition(i)) > m_PositioningTolerance)
                {
                    return false;
                }
            }
        }
        return true;
    }
};