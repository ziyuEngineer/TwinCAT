#pragma once
#include "SpindleStateMachine.h"

class SpindleFault : public SpindleStateMachine
{
public:
	void entry() override
	{
		report_current_state(SystemState::eFault);
	}

	void react(Cycle_Update const&) override
	{
		
	}

	void exit() override {};
};