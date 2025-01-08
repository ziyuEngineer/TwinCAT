#pragma once
#include "CommonDefine.h"

namespace Driver
{
	class CDriver
	{
	public:
		CDriver();
		~CDriver();

	protected:
		bool m_Flip = false;
		int m_OpModeMask = 7; // 2#00000111
		int m_DriverStateGeneralMask = 0x6F; // 2#01101111
		int m_DriverStateBeckhoffMask = 0xF000; // 2#1111 0000 0000 0000
		int m_MultiTurnNum = 0; // For revolute axis

		const MotionControlInfo* m_DriverParam = nullptr;
		DriverInput* m_DriverInput = nullptr;
		DriverOutput* m_DriverOutput = nullptr;
		
	// Parameters related to driver
	public:
		// Input from driver
		unsigned short m_StatusWord = 0;
		long m_FdbPosVal = 0;		// Abs Pos feedback, inc
		short m_FdbTorVal = 0;		// Torque feedback
		long m_FdbVelVal = 0;		// Velocity feedback, inc/s
		short m_ErrorCode = 0;
		short m_WarningCode = 0;
		char m_ActualOpMode;
		long m_EffectivePosCmd = 0;		// Effective pos command
		short m_EffectiveTorCmd = 0;		// Effective torque command
		long m_EffectiveVelCmd = 0;		// Effective velocity command

		// Output to driver
		unsigned short m_ControlWord = 0;
		short m_OperationMode = 0;
		short m_TargetTor = 0;
		short m_AdditiveTor = 0;
		long m_TargetPos = 0;
		long m_TargetVel = 0;
		long m_AdditivePos = 0;
		long m_AdditiveVel = 0;

	public:
		void MapParameters(DriverInput* driver_input, DriverOutput* driver_output, const MotionControlInfo* driver_param);
		void Scan();
		void Actuate();

		double  GetFeedbackPosition();
		double  GetFeedbackVelocity();
		void	ComputeAcceleration(double input, double* first_derivative, double* second_derivative);
		double  GetFeedbackTorque();
		OpMode	GetActualOperationMode();
		DriverStatus GetActualDriverStatus();
		double  GetEffectivePositionCommand();
		double  GetEffectiveVelocityCommand();
		double  GetEffectiveTorqueCommand();

		void    SetTargetTorque(double cmd_tor);
		void    SetAdditiveTorque(double additive_tor);
		void    SetTargetPosition(double cmd_pos);
		void    SetAdditivePosition(double additive_pos);
		void    ReturnToZeroPosition();
		void	SetTargetVelocity(double cmd_vel);
		void    SetAdditiveVelocity(double additive_vel);
		bool    SetDriverParam();
		void    SetOperationMode(OpMode mode);
		bool    IsOpModeSwitched();

		void    ControlUnitSync();
		bool    Enable();
		bool    Disable();
		bool    IsEnableState();
		bool	IsDisableState();

		void    ClearError();
		bool    IsFaultState();
		bool    IsEmergencyState();
		
		short   CheckErrorCode();
		short   CheckWarnCode();

		void	QuickStop();
	
	private:
		DriverState	GetDriverState_Beckhoff();
		DriverStateGeneral GetDriverState_General();
		DriverStatus GetDriverStatus_Beckhoff();
		DriverStatus GetDriverStatus_General();
		bool    IsDriverReady();
		bool    IsDriverShielded();
		void    ControlUnitSync_General();
		bool	Enable_Beckhoff();
		bool    Disable_Beckhoff();
		bool    Enable_General();
		bool    Disable_General();
		void    SetOperationMode_Beckhoff(OpMode mode);
		void    SetOperationMode_General(OpMode mode);
		void    ClearError_Beckhoff();
		void    ClearError_General();
	};
}

