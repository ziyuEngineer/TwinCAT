#pragma once
#include "FsmCommon.h"

class SpindleIdle;
class SpindleInitialize;
class SpindleDisabled;
class SpindleStandby;
class SpindleMoving;
class SpindleFault;
class SpindleTest;
class SpindleEmergency;

class SpindleStateMachine : public tinyfsm::Fsm<SpindleStateMachine>
{
public:
    virtual ~SpindleStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(Cycle_Update const&) = 0;

    virtual void SafetyCheck();

    static CFiveAxisController* s_pController;

    static void start()
    {
        if (s_pController == nullptr)
        {
            //throw std::runtime_error("Controller system is not set");
        }
        else
        {
            Fsm<SpindleStateMachine>::start();
        }
    }

    static void report_current_state(SystemState _sysState)
    {
        s_pController->m_SpindleState = _sysState;
    }
};
