#pragma once
#include "SpindleStateMachine.h"

class SpindleFault : public SpindleStateMachine
{
public:
	void entry() override
	{
		report_current_state(SpindleState::eSpindleFault);
	}

	void react(EventCycleUpdate const&) override
	{
		
	}

	void exit() override {};
};