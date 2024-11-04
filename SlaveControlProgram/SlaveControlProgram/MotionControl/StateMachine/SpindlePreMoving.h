#pragma once
#include "SpindleStateMachine.h"
#include "SpindleLocate.h"
#include "SpindleRotate.h"

enum class SpindleMovingState
{
    eSpindleNull = 0,
    eSpindleRotate = 1,
    eSpindleLocate = 2
};

class SpindlePreMoving : public SpindleStateMachine
{
protected:
    SpindleMovingState m_NextMovingState = SpindleMovingState::eSpindleNull;
    SpindlePosition m_SpindleLocateCommand;
    SpindleRot m_SpindleRotateCommand;
    OpMode m_NextOpMode;

public:
    void entry() override
    {
        report_current_state(SpindleState::eSpindlePreMoving);
        s_pController->SpindleEnable();
        s_pController->SpindleSwitchOpMode(m_NextOpMode);
    }

    void react(EventCycleUpdate const&) override
    {
        if (!s_pController->IsSpindleEnabled())
        {
            s_pController->SpindleEnable();
        }
        else if (!s_pController->IsSpindleOpModeSwitched())
        {
            s_pController->SpindleSwitchOpMode(m_NextOpMode);
        }
        else
        {
            switch (m_NextMovingState)
            {
            case SpindleMovingState::eSpindleNull:
                break;

            case SpindleMovingState::eSpindleRotate:
                state<SpindleRotate>().SetCommand(m_SpindleRotateCommand);
                transit<SpindleRotate>();
                break;

            case SpindleMovingState::eSpindleLocate:
                state<SpindleLocate>().SetCommand(m_SpindleLocateCommand);
                transit<SpindleLocate>();
                break;
            }
        }
    }

    void exit() override
    {
        ResetNextMovingState();
    }

    void SetNextMovingState(SpindleMovingState next_state)
    {
        m_NextMovingState = next_state;
    }

    void ResetNextMovingState()
    {
        m_NextMovingState = SpindleMovingState::eSpindleNull;
    }

    void SetNextMovingCmd(SpindlePosition pos_cmd)
    {
        m_SpindleLocateCommand = pos_cmd;
        m_NextOpMode = OpMode::CSP;
    }

    void SetNextMovingCmd(SpindleRot rot_cmd)
    {
        m_SpindleRotateCommand = rot_cmd;
        m_NextOpMode = OpMode::CSV;
    }
};
