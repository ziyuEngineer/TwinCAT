#pragma once
#include "FsmCommon.h"
#include "CommandController.h"

class MainStateDisabled;
class MainStatePreStandby;
class MainStateStandby;
class MainStateManual;
class MainStateBuffering;
class MainStateContinuousExecution;
class MainStateFault;
class MainStateRecovery;

class MainStateMachine : public tinyfsm::Fsm<MainStateMachine>
{
public:
    virtual ~MainStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(EventCycleUpdate const&) = 0;

    virtual void react(EventContinuousExecution const&) {};

    virtual void react(EventStopContinuousMoving const&) {};

    static CCommandController* s_pController;

    static void start()
    {
        if (s_pController == nullptr)
        {
            //throw std::runtime_error("Controller system is not set");
        }
        else
        {
            Fsm<MainStateMachine>::start();
        }
    }

    static void report_current_state(SystemState _sysState)
    {
        s_pController->m_MainState = _sysState;
    }
};
