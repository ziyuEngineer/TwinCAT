#pragma once
#include "FsmCommon.h"
#include "ToolController.h"

class ToolStateInitialize;
class ToolStateStandby;
class ToolStateSwitching;
class ToolStateUsing;
class ToolStateFault;

class ToolStateMachine : public tinyfsm::Fsm<ToolStateMachine>
{
public:
    virtual ~ToolStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(EventCycleUpdate const&) = 0;

    static CToolController* s_pController;

    static void start()
    {
        Fsm<ToolStateMachine>::start();
    }

    static void report_current_state(ToolState toolState)
    {
        s_pController->m_ToolState = toolState;
    }
};
