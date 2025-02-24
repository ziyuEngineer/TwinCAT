#pragma once
#include "MainStateMachine.h"

class MainStateToolSwitching : public MainStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eToolSwitching);
        s_pController->DispatchEventMessage(TcEvents::MainEvent::MainModuleTool.nEventId);
    }

    void react(EventCycleUpdate const&) override
    {
        //s_pController->RequestAxisGroupPositioning();
        //s_pController->RequestSpindlePositioning(50.0);
    }

    void SetToolNumber(int tool_num)
    {
        m_SetToolNumber = tool_num;
    }

    int GetCurrentToolNumber()
    {
        return m_CurrentToolNumber;
    }

    void exit() override
    {
        m_CurrentToolNumber = m_SetToolNumber;
    }

protected:
    int m_CurrentToolNumber;
    int m_SetToolNumber;
};