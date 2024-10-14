#pragma once
#include "FsmCommon.h"

class AxisGroupIdle;
class AxisGroupInitialize;
class AxisGroupDisabled;
class AxisGroupStandby;
class AxisGroupHandwheel;
class AxisGroupMoving;
class AxisGroupFault;
class AxisGroupTest;
class AxisGroupEmergency;
class AxisGroupRecovery;

class AxisGroupStateMachine : public tinyfsm::Fsm<AxisGroupStateMachine>
{
public:
    virtual ~AxisGroupStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    struct AutoModeRequested : tinyfsm::Event {};

    struct SystemFaultClearRequested : tinyfsm::Event {};

    virtual void react(Cycle_Update const&) = 0;

    //virtual void react(SystemSafetyCheck const&) = 0;

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
            Fsm<AxisGroupStateMachine>::start();
        }
    }

    static void report_current_state(SystemState _sysState)
    {
        s_pController->m_AxisGroupState = _sysState;
    }
};
