#pragma once
#include "FsmCommon.h"
#include "SpindleController.h"

class SpindleIdle;
class SpindleInitialize;
class SpindleDisabled;
class SpindleEnable;
class SpindlePreMoving;
class SpindleRotate;
class SpindleLocate;
class SpindlePostMoving;
class SpindleFault;
class SpindleEmergency;

class SpindleStateMachine : public tinyfsm::Fsm<SpindleStateMachine>
{
public:
    virtual ~SpindleStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(EventCycleUpdate const&) = 0;

    virtual void react(EventSpindleRotating const&) {};

    virtual void react(EventSpindlePositioning const&) {};

    virtual void react(EventSpindleStop const&) {};

    virtual void react(EventSpindleSetLimit const&) {};

    virtual void react(EventSpindleEnable const&) {};

    virtual void react(EventSpindleDisable const&) {};

    virtual void react(EventSpindleEnterFault const&) {};

    virtual void react(EventSpindleResetError const&) {};

    static CSpindleController* s_pController;

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

    static void report_current_state(SpindleState sysState)
    {
        s_pController->m_SpindleState = sysState;
    }
};
