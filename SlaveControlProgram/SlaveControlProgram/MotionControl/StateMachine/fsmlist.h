#pragma once
#include "AxisGroupStateMachine.h"
#include "SpindleStateMachine.h"

using m_FsmHandle = tinyfsm::FsmList<AxisGroupStateMachine, SpindleStateMachine>;
