#pragma once
#include "FsmCommon.h"
#include "SafetyController.h"

class SafetyStateNormal;
class SafetyStateWarning;
class SafetyStateEmergency;
class SafetyStateFault;
class SafetyStateRecovery;

class SafetyStateMachine : public tinyfsm::Fsm<SafetyStateMachine>
{
public:
    virtual ~SafetyStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(EventCycleUpdate const&) = 0;

    virtual void react(EventRequestSafetyEnterRecoveryState const&) {};

    virtual void react(EventRequestSafetyExitRecoveryState const&) {};

    static CSafetyController* s_pController;

    virtual void FaultCheck();

    virtual void EmergencyCheck();

    virtual void WarningCheck();

    virtual void RecoveryCheck();

    static void start()
    {
        if (s_pController == nullptr)
        {
            //throw std::runtime_error("Controller system is not set");
        }
        else
        {
            Fsm<SafetyStateMachine>::start();
        }
    }

    static void report_current_state(SafetyState sysState)
    {
        s_pController->m_SafetyStatus = sysState;
    }
};
