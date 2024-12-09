#pragma once
#include "MainStateMachine.h"

class MainStateRecovery : public MainStateMachine
{
protected:
    ModuleError m_ErrorType;
    AxisOrder m_ErrorAxis;

public:
    void entry() override
    {
        report_current_state(SystemState::eRecovery);
        s_pController->RequestAxisGroupEnterHandwheel();
        ParseErrorCode(s_pController->GetErrorCode(), m_ErrorType, m_ErrorAxis);
    }

    void react(EventCycleUpdate const&) override
    {
        s_pController->UpdateManualCommandInRecoveryState(m_ErrorType, m_ErrorAxis);
    }

    void react(EventRequestExitRecoveryState const&) override
    {
        s_pController->DeselectAxisAutomatically();
        s_pController->RequestAxisGroupQuitHandwheel();
        transit<MainStateStandby>();
    }

    void exit() override
    {
    }
};