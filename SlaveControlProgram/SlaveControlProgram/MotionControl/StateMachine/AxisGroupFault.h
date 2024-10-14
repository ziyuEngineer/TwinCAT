#pragma once
#include "AxisGroupStateMachine.h"

class AxisGroupFault : public AxisGroupStateMachine
{
public:
    void entry() override
    {
        report_current_state(SystemState::eFault);
    }

    void react(Cycle_Update const&) override
    {
        if (!s_pController->IsResetSelected()) // reset button  from panel
		{
			s_pController->AxisGroupFault();
		}
		else
		{
			// Notify plc module to Reset SoE
			s_pController->NotifyPlcResetSoE(true);

			if (s_pController->AxisGroupEnable()) { // Send enable controlword to reset fault.
				transit<AxisGroupDisabled>();
				s_pController->NotifyPlcResetSoE(false);
			}
		}
    }

    void exit() override {};
};