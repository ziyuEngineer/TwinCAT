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

    void exit() override {};

	void react(EventAxisGroupResetError const&) override
	{
		// Notify plc module to Reset SoE
		s_pController->NotifyPlcResetSoE(true);

		if (s_pController->AxisGroupEnable()) { // Send enable controlword to reset fault.
			transit<AxisGroupDisabled>();
			s_pController->NotifyPlcResetSoE(false);
		}
	};
};