#pragma once
#include "MainStateMachine.h"

class MainStateFault : public MainStateMachine
{
protected:
    bool m_IsPanelReset;

public:
    void entry() override
    {
        report_current_state(SystemState::eFault);
        s_pController->DispatchEventMessage(TcEvents::MainEvent::MainModuleFault.nEventId);
        ResetPanel();
        s_pController->RequestAxisGroupFault();
        s_pController->RequestSpindleFault();
    }

    void react(EventCycleUpdate const&) override
    {
        if (s_pController->IsResetPressed())
        {
            s_pController->RequestAxisGroupResetError();
            s_pController->RequestSpindleResetError();
        }
    }

    void react(EventRequestExitFaultState const& event) override
    {
        transit<MainStateDisabled>();
    }

    void exit() override
    {
        m_IsPanelReset = false;
    }

    void ResetPanel()
    {
        if (!m_IsPanelReset)
        {
            m_IsPanelReset = s_pController->PanelReset();
        }
    }
};