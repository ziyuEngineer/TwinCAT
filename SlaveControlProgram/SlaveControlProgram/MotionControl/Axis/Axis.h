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
	InterpolationParameter* m_InterpolationParam;

	// The following variable will only be used in position or velocity interpolation,
	// so initialzie it as CST mode;
	OpMode m_LastOpMode = OpMode::CST;

	void InterpolationReset(OpMode _mode);

	bool m_bStandbyPosSet = false;
	

public:	
	double m_CmdPos;
	double m_CmdVel;
	double m_CmdAcc;
	double m_CmdTor;

	double m_CmdPosBeforeInterpolated; // observe only
	double m_CmdVelBeforeInterpolated;
	bool m_bPosInterpolationFinished = false;
	bool m_bVelInterpolationFinished = false;

	double m_FdbPos;
	double m_FdbPosFiltered;
	double m_FdbVel;
	double m_FdbVelFiltered;
	double m_FdbAcc;
	double m_FdbTor;
	OpMode m_ActualOpMode;

	double m_StandbyPos;
	
public:
	void MapParameters(DriverInput* _DriverInput, DriverOutput* _DriverOutput, MotionControlInfo* _DriverParam, InterpolationParameter* _InterpolationParameter);
	bool PostConstruction();

	void Input();
	void Output();

	bool Initialize();
	bool Enable();
	bool Disable();

	void HoldPosition();
	void StandStill();
	void Move(double _cmd, OpMode _mode, bool _bInterpolated, bool _bUpdateFeedback);
	void ReturnToZeroPoint();

	bool IsExceedingLimit();
	bool IsEmergency();
	bool IsFault();

	PositionInterpolator1D m_PositionInterpolator;
	VelocityInterpolator1D m_VelocityInterpolator;
	PositionInterpolator1D::State m_CurrentStatePos;
	VelocityInterpolator1D::State m_CurrentStateVel;
};