#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupFault : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(AxisGroupState::eAxisGroupFault);
    }

    void react(EventCycleUpdate const&) override
    {
       s_pController->AxisGroupFault();
    }

    void exit() override
	{
	}

	void react(EventAxisGroupResetError const&) override
	{
		// Notify plc module to Reset SoE
		s_pController->NotifyPlcResetSoE(true);

		s_pController->AxisGroupClearError(); // Send enable controlword to reset fault.

		if (s_pController->AxisGroupDisable()) 
		{ 
			transit<AxisGroupDisabled>();
		}
		
		/*s_pController->AxisGroupClearError();
		transit<AxisGroupDisabled>();*/
	};
};