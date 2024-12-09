#pragma once

#include "Driver.h"
#include "Biquad.h"
#include "interpolator1d/interpolator.hh"

using namespace Driver;
class CAxis
{
public:	
	CAxis() = default;
	~CAxis() = default;

private:
	CDriver m_Driver;
	Biquad m_FdbPosFilter;
	Biquad m_FdbVelFilter;
	const InterpolationParameter* m_InterpolationParam;

	PositionInterpolator1D m_PositionInterpolator;
	VelocityInterpolator1D m_VelocityInterpolator;
	PositionInterpolator1D::State m_CurrentStatePos;
	VelocityInterpolator1D::State m_CurrentStateVel;
	bool m_IsPosInterpolationFinished = false;
	bool m_IsVelInterpolationFinished = false;

	bool m_IsStandbyPosSet = false;
	
public:	
	double m_CmdPos;
	double m_CmdVel;
	double m_CmdAcc;
	double m_CmdTor;

	double m_CmdPosBeforeInterpolated; // observe only
	double m_CmdVelBeforeInterpolated;
	
	double m_FdbPos;
	double m_FdbPosFiltered;
	double m_FdbVel;
	double m_FdbVelFiltered;
	double m_FdbAcc;
	double m_FdbTor;
	OpMode m_ActualOpMode;
	DriverStatus m_CurrentDriverStatus;

	double m_StandbyPos;
	
public:
	void MapParameters(DriverInput* driver_input, DriverOutput* driver_output, const MotionControlInfo* driver_param, const InterpolationParameter* interpolation_parameter);
	bool PostConstruction();

	void Input();
	void Output();

	bool Initialize();
	bool Enable();
	bool Disable();
	void SwitchOperationMode(OpMode mode);
	bool IsOpModeSwitched();
	bool IsEnabled();
	bool IsDisabled();

	void HoldPosition();
	void StandStill();
	void Move(double cmd, OpMode mode, bool is_interpolated, bool update_feedback);
	void ReturnToZeroPoint();

	bool IsEmergency();
	bool IsFault();
	void ClearError();
	void QuickStop();

	void InterpolationReset(OpMode _mode);
	void UpdatePositionCommand();
	void CompensateAdditiveTor(double add_tor);
};