#pragma once
#include "FsmCommon.h"
#include "AxisGroupController.h"

class AxisGroupIdle;
class AxisGroupInitialize;
class AxisGroupDisabled;
class AxisGroupPreStandby;
class AxisGroupStandby;
class AxisGroupManualMoving;
class AxisGroupPreContinuousMoving;
class AxisGroupContinuousMoving;
class AxisGroupFault;
class AxisGroupEmergency;
class AxisGroupRecovery;
class AxisGroupLimitViolation;

class AxisGroupStateMachine : public tinyfsm::Fsm<AxisGroupStateMachine>
{
public:
    virtual ~AxisGroupStateMachine() = default;

    virtual void entry() {};

    virtual void exit() {};

    virtual void react(EventCycleUpdate const&) = 0;

    virtual void react(EventAxisGroupDeselectAxis const&) {};

    virtual void react(EventAxisGroupSelectAxis const&) {};

    virtual void react(EventAxisGroupServoOn const&) {};

    virtual void react(EventAxisGroupDisable const&) {};

    virtual void react(EventAxisGroupContinuouslyMove const&) {};

    virtual void react(EventAxisGroupStop const&) {};

    virtual void react(EventAxisGroupEnterRecoveryState const&) {};

    virtual void react(EventAxisGroupExitRecoveryState const&) {};

    virtual void react(EventAxisGroupResetError const&) {};

    virtual void react(EventAxisGroupPreMovingChangeOpMode const&) {};

    virtual void SafetyCheck();
    
    static CAxisGroupController* s_pController;

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

    static void report_current_state(AxisGroupState _sysState)
    {
        s_pController->m_AxisGroupState = _sysState;
    }
};
