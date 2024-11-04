#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupManualMoving : public AxisGroupStateMachine
{
protected:
    bool m_IsAxisGroupInCSP = false;

public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupManualMoving);
        s_pController->AxisGroupSwitchOpMode(OpMode::CSP);
        s_pController->AxisGroupResetInterpolator(OpMode::CSP);
        m_IsAxisGroupInCSP = s_pController->IsAxisGroupOpModeSwitched();
    }

    void react(EventCycleUpdate const&) override
    {
        SafetyCheck();

        if(m_IsAxisGroupInCSP)
        {
            s_pController->AxisGroupHandwheel();
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

    void react(EventAxisGroupDeselectAxis const&) override
    {
        transit<AxisGroupStandby>();
    }

};