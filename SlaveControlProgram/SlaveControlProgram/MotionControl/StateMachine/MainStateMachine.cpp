#include "../TcPch.h"
#pragma hdrstop
#include "MainStateMachine.h"

#include "MainStateDisabled.h"
#include "MainStatePreStandby.h"
#include "MainStateStandby.h"
#include "MainStateManual.h"
#include "MainStateBuffering.h"
#include "MainStateContinuousExecution.h"
#include "MainStateFault.h"
#include "MainStateRecovery.h"

CCommandController* MainStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(MainStateMachine, MainStateDisabled)