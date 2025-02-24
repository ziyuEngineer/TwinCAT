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
#include "MainStateToolSwitching.h"

CMainController* MainStateMachine::s_pController = nullptr;
FSM_INITIAL_STATE(MainStateMachine, MainStateDisabled)

void MainStateMachine::ParseErrorCode(ULONG error_code, ModuleError& err_type, AxisOrder& axis)
{
	int type_mask = 3840; // 2#0000 1111 0000 0000
	int axis_mask = 240; // 2#0000 0000 1111 0000

	err_type = static_cast<ModuleError>((error_code & type_mask) >> 8); // index of error type

	axis = static_cast<AxisOrder>((error_code & axis_mask) >> 4); // index of error type
}

bool MainStateMachine::IsSafeToTransitState()
{
	return s_pController->IsSystemNormal();
}