#include "../TcPch.h"
#pragma hdrstop
#include "SpindleStateMachine.h"
#include "SpindleIdle.h"
#include "SpindleInitialize.h"
#include "SpindleDisabled.h"
#include "SpindleEnable.h"
#include "SpindlePreMoving.h"
#include "SpindleRotate.h"
#include "SpindleLocate.h"
#include "SpindlePostMoving.h"
#include "SpindleFault.h"
#include "SpindleEmergency.h"

CSpindleController* SpindleStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(SpindleStateMachine, SpindleIdle)
