#include "../TcPch.h"
#pragma hdrstop
#include "SafetyController.h"

IMotionControlInterface* CSafetyController::m_pMainModuleInterface = nullptr;

CSafetyController::CSafetyController()
{
	m_SafetyStatus = SafetyState::eSafetyStateNormal;
}

CSafetyController::~CSafetyController()
{
}

void CSafetyController::MapParameters(ModuleSafetyInputs* inputs, ModuleSafetyOutputs* outputs, const ModuleSafetyParameter* parameters)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_pParameters = parameters;

	m_ActualAxisNum = parameters->ActualAxisNum;
	m_ActualDriverNum = parameters->ActualDriverNum;
	memcpy(m_DriverNumPerAxis, parameters->DriverNumPerAxis, sizeof(m_DriverNumPerAxis));

	int driver_num = 0;
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_DriverAxisMap[driver_num] = static_cast<AxisOrder>(i+1);
			driver_num++;
		}
	}

	int rows = kMaxAxisNum * kMaxMotorNumPerAxis;
	m_TorFollowingErrorWindow.resize(rows);
	m_TorCmdDeviationWindow.resize(rows);
}

void CSafetyController::Input()
{
	SlidingWindowComputeTorqueDiagnosticInfo();
}

void CSafetyController::Output()
{
	m_pOutputs->SafetyStatus = m_SafetyStatus;
	m_pOutputs->ErrorCode = m_ErrorCode;
}

bool CSafetyController::IsAxisGroupPosLimitCheckPassed() 
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		if(m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos < m_pParameters->SafetyParameterAxisGroup[i].PosLowerLimit)
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::PosLowerOver, m_DriverAxisMap.at(i));
			m_ErrorPos[i] = m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos;
			m_ErrorDriver = i;
			return false;
		}
		else if (m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos > m_pParameters->SafetyParameterAxisGroup[i].PosUpperLimit)
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::PosUpperOver, m_DriverAxisMap.at(i));
			m_ErrorPos[i] = m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos;
			m_ErrorDriver = i;
			return false;
		}
	}
	return true;
}

bool CSafetyController::IsAxisGroupVelLimitCheckPassed()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		if (fabs_(m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentVel) > m_pParameters->SafetyParameterAxisGroup[i].MaxVelocity)
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::VelOver, m_DriverAxisMap.at(i));
			return false;
		}
	}
	return true;
}

bool CSafetyController::IsSpindlePosLimitCheckPassed() 
{
	if (m_pInputs->DiagnosticInfo.SpindleInfo.CurrentPos < m_pParameters->SafetyParameterSpindle.PosLowerLimit)
	{
		m_ErrorCode = GenerateTinyErrorCode(ModuleName::Spindle, ModuleError::PosLowerOver, AxisOrder::Spindle);
		return false;
	}
	else if (m_pInputs->DiagnosticInfo.SpindleInfo.CurrentPos > m_pParameters->SafetyParameterSpindle.PosUpperLimit)
	{
		m_ErrorCode = GenerateTinyErrorCode(ModuleName::Spindle, ModuleError::PosUpperOver, AxisOrder::Spindle);
		return false;
	}
	return true;
}

bool CSafetyController::IsSpindleVelLimitCheckPassed()
{
	if (fabs_(m_pInputs->DiagnosticInfo.SpindleInfo.CurrentVel) > m_pParameters->SafetyParameterSpindle.MaxVelocity)
	{
		m_ErrorCode = GenerateTinyErrorCode(ModuleName::Spindle, ModuleError::VelOver, AxisOrder::Spindle);
		return false;
	}
	return true;
}

/**
 * @brief Check soft limits and velocity limit;
 *
 * @return true
 * @return false
 */
bool CSafetyController::IsSubSystemLimitsCheckPassed()
{
	return IsAxisGroupPosLimitCheckPassed() && IsAxisGroupVelLimitCheckPassed() && IsSpindlePosLimitCheckPassed() && IsSpindleVelLimitCheckPassed();
}

bool CSafetyController::IsWarningCheckPassed()
{
	return IsSubSystemLimitsCheckPassed();
}

/**
* @brief Check if the position exceeds the hard limits.
* Normally, m_DigitalInput = 15(2#1111),
* if optoelectronic switch connected to Channel 0 is blocked, m_DigitalInput = 14(2#1110);
* if optoelectronic switch connected to Channel 1 is blocked, m_DigitalInput = 13(2#1101);
* and so on.
*
* @return true
* @return false
*/
bool CSafetyController::IsHardLimitCheckPassed() 
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		if (m_pInputs->DiagnosticInfo.AxisGroupDigitalInput[i] == m_HardLimitDigitalMask - (1 << m_pParameters->SafetyParameterAxisGroup[i].PositiveHardBit))
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::PosUpperOver, m_DriverAxisMap.at(i));
			m_ErrorPos[i] = m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos;
			m_ErrorDriver = i;
			return false;
		}
		else if (m_pInputs->DiagnosticInfo.AxisGroupDigitalInput[i] == m_HardLimitDigitalMask - (1 << m_pParameters->SafetyParameterAxisGroup[i].NegativeHardBit))
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::PosLowerOver, m_DriverAxisMap.at(i));
			m_ErrorPos[i] = m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentPos;
			m_ErrorDriver = i;
			return false;
		}
	}
	return true;
}

bool CSafetyController::IsGantryDeviationCheckPassed()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		if (m_pParameters->SafetyParameterAxisGroup[i].IsGantry)
		{
			if (fabs_(m_pInputs->DiagnosticInfo.AxisGroupInfo.GantryDeviation) > m_pParameters->SafetyParameterAxisGroup[i].GantryDeviationTolerance)
			{
				m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::GantryDeviationOver, m_DriverAxisMap.at(i));
				m_ErrorDeviation = m_pInputs->DiagnosticInfo.AxisGroupInfo.GantryDeviation;
				return false;
			}
		}
	}
	return true;
}

bool CSafetyController::IsTorqueCommandCheckPassed()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		// test
		m_pOutputs->TestTorqueCmd[i] = m_TorCmdDeviationSum[i] / kCheckWindowSize;
		if (m_TorCmdDeviationSum[i] / kCheckWindowSize > kTorCmdDeviationTolerance)
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::TorCmdOscillation, m_DriverAxisMap.at(i));
			
			return false;
		}
	}
	return true;
}

bool CSafetyController::IsTorqueFollowingErrorCheckPassed()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		// test
		m_pOutputs->TestTorqueError[i] = m_TorFollowingErrorSum[i] / kCheckWindowSize;
		if (m_TorFollowingErrorSum[i] / kCheckWindowSize > kTorFollowingErrorTolerance)
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::TorFollowingOscillation, m_DriverAxisMap.at(i));

			return false;
		}
	}
	return true;
}

bool CSafetyController::IsEmergencyCheckPassed()
{
	return IsHardLimitCheckPassed() && IsGantryDeviationCheckPassed();//&& IsTorqueFollowingErrorCheckPassed() && IsTorqueCommandCheckPassed();
}

bool CSafetyController::IsAxisGroupStatusCheckPassed()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		if (m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[i].CurrentStatus == static_cast<int>(DriverStatus::DriverFault))
		{
			m_ErrorCode = GenerateTinyErrorCode(ModuleName::AxisGroup, ModuleError::StatusErr, m_DriverAxisMap.at(i));
			return false;
		}
	}
	return true;
}

bool CSafetyController::IsSpindleStatusCheckPassed()
{
	if (m_pInputs->DiagnosticInfo.SpindleInfo.CurrentStatus == static_cast<int>(DriverStatus::DriverFault))
	{
		m_ErrorCode = GenerateTinyErrorCode(ModuleName::Spindle, ModuleError::StatusErr, AxisOrder::Spindle);
		return false;
	}
	return true;
}

/**
 * @brief Check status word of drivers
 * 
 * @return true 
 * @return false 
 */
bool CSafetyController::IsSubSystemStatusCheckPassed()
{
	return IsAxisGroupStatusCheckPassed() && IsSpindleStatusCheckPassed();
}

bool CSafetyController::IsAxisGroupExitFault()
{
	return m_pInputs->DiagnosticInfo.StateAxisGroup != AxisGroupState::eAxisGroupFault;
}

bool CSafetyController::IsSpindleExitFault()
{
	return m_pInputs->DiagnosticInfo.StateSpindle != SpindleState::eSpindleFault;
}

bool CSafetyController::IsSubSystemStateCorrect()
{
	return IsSpindleExitFault() && IsAxisGroupExitFault();
}

bool CSafetyController::IsFaultCheckPassed()
{
	return IsSubSystemStatusCheckPassed();
}

bool CSafetyController::IsMovingOppositely()
{
	if (static_cast<ModuleError>(m_ErrorCode & 0xF00) == ModuleError::PosLowerOver)
	{
		return m_ErrorPos[m_ErrorDriver] < m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[m_ErrorDriver].CurrentPos;
	}
	else if (static_cast<ModuleError>(m_ErrorCode & 0xF00) == ModuleError::PosUpperOver)
	{
		return m_ErrorPos[m_ErrorDriver] > m_pInputs->DiagnosticInfo.AxisGroupInfo.SingleAxisInformation[m_ErrorDriver].CurrentPos;
	}
	return true;
}

bool CSafetyController::IsDeviationDecreasing()
{
	if (static_cast<ModuleError>(m_ErrorCode & 0xF00) == ModuleError::GantryDeviationOver)
	{
		return fabs_(m_ErrorDeviation) > fabs_(m_pInputs->DiagnosticInfo.AxisGroupInfo.GantryDeviation);
	}
	return true;
}

bool CSafetyController::IsRecoveryCheckPassed()
{
	return IsMovingOppositely() && IsDeviationDecreasing();
}

void CSafetyController::ClearErrorCode()
{
	m_ErrorCode = 0;
}

void CSafetyController::ReportFault()
{
	m_pMainModuleInterface->EnterFaultState();
}

void CSafetyController::RequestClearFault()
{
	m_pMainModuleInterface->ExitFaultState();
}
	 
void CSafetyController::ReportEmergency()
{
	m_pMainModuleInterface->EnterDisabledState();
}

void CSafetyController::ReportWarning()
{
	m_pMainModuleInterface->EnterStandbyState();
}
	 
void CSafetyController::RequestEnterRecovery()
{
	m_pMainModuleInterface->EnterRecoveryState(true);
}

void CSafetyController::RequestExitRecovery()
{
	m_pMainModuleInterface->EnterRecoveryState(false);
}

unsigned long CSafetyController::GenerateErrorCode(ProjectSeries project, ModuleName module_name, ModuleError module_err, AxisOrder axis_num, ErrorClass errorClass)
{
	unsigned long nRet;

	char errorCode[9];

	errorCode[0] = static_cast<int>(project) + '0';

	errorCode[1] = static_cast<int>(module_name) + '0';

	errorCode[2] = static_cast<int>(module_err) + '0';

	errorCode[3] = static_cast<int>(axis_num) + '0';

	errorCode[4] = 'F'; // reserved
	errorCode[5] = 'F'; // reserved
	errorCode[6] = 'F'; // reserved

	errorCode[7] = 'A' + static_cast<int>(errorClass);

	errorCode[8] = '\0';

	nRet = strtol(errorCode, NULL, 16);

	return nRet;
}

unsigned long CSafetyController::GenerateTinyErrorCode(ModuleName module_name, ModuleError module_err, AxisOrder axis_num)
{
	unsigned long nRet;

	char errorCode[5];

	errorCode[0] = static_cast<int>(module_name) + '0';

	errorCode[1] = static_cast<int>(module_err) + '0';

	errorCode[2] = static_cast<int>(axis_num) + '0';

	errorCode[3] = 'F'; // reserved

	errorCode[4] = '\0';

	nRet = strtol(errorCode, NULL, 16);

	return nRet;
}

void CSafetyController::SlidingWindowComputeTorqueDiagnosticInfo()
{
	if (m_WindowCounter < kCheckWindowSize)
	{
		for (int i = 0; i < m_ActualDriverNum; i++)
		{
			m_TorCmdDeviationSum[i] += m_pInputs->TorqueDiagnosticInfo.CommandDeviation[i];
			m_TorFollowingErrorSum[i] += m_pInputs->TorqueDiagnosticInfo.FollowingError[i];

			m_TorCmdDeviationWindow[i].push_back(m_pInputs->TorqueDiagnosticInfo.CommandDeviation[i]);
			m_TorFollowingErrorWindow[i].push_back(m_pInputs->TorqueDiagnosticInfo.FollowingError[i]);
		}
		m_WindowCounter++;
	}
	else
	{
		for (int i = 0; i < m_ActualDriverNum; i++)
		{
			m_TorCmdDeviationSum[i] += (m_pInputs->TorqueDiagnosticInfo.CommandDeviation[i] - m_TorCmdDeviationWindow[i][0]);
			m_TorFollowingErrorSum[i] += (m_pInputs->TorqueDiagnosticInfo.FollowingError[i] - m_TorFollowingErrorWindow[i][0]);

			m_TorCmdDeviationWindow[i].erase(m_TorCmdDeviationWindow[i].begin());
			m_TorFollowingErrorWindow[i].erase(m_TorFollowingErrorWindow[i].begin());

			m_TorCmdDeviationWindow[i].push_back(m_pInputs->TorqueDiagnosticInfo.CommandDeviation[i]);
			m_TorFollowingErrorWindow[i].push_back(m_pInputs->TorqueDiagnosticInfo.FollowingError[i]);
		}
	}
}