#include "../TcPch.h"
#pragma hdrstop
#include "ToolStateMachine.h"

#include "ToolStateInitialize.h"
#include "ToolStateStandby.h"
#include "ToolStateSwitching.h"
#include "ToolStateUsing.h"
#include "ToolStateFault.h"

CToolController* ToolStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(ToolStateMachine, ToolStateInitialize)