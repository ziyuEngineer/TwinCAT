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
		int m_DigitalMask = 15;// 14(1110):y+; 13(1101):y-; 11(1011):z+; 7(0111):z-
		int m_OpModeMask = 7; // 2#00000111
		int m_DriverStateGeneralMask = 0x6F; // 2#01101111

		MotionControlInfo* m_DriverParam = nullptr;
		DriverInput* m_DriverInput = nullptr;
		DriverOutput* m_DriverOutput = nullptr;
		
	// Parameters related to driver
	public:
		// Input from driver
		unsigned short m_StatusWord = 0;
		long m_FdbPosVal = 0;	 // Abs Pos feedback
		short m_FdbTorVal = 0; // Torque feedback
		long m_FdbVelVal = 0;  // Velocity feedback
		short m_ErrorCode = 0;
		short m_WarningCode = 0;
		short m_DigitalInput = 0;

		// Output to driver
		unsigned short m_ControlWord = 0;
		short m_OperationMode = 0;
		short m_TargetTor = 0;
		long m_TargetPos = 0;
		long m_TargetVel = 0;

	public:
		void MapParameters(DriverInput* _DriverInput, DriverOutput* _DriverOutput, MotionControlInfo* _DriverParam);
		void Scan();
		void Actuate();

		double  GetFeedbackPosition();
		double  GetFeedbackVelocity();
		void	ComputeAcceleration(double _input, double* _first_derivative, double* _second_derivative);
		double  GetFeedbackTorque();

		void    SetTargetTorque(double _cmd_tor);
		void    SetTargetPosition(double _cmd_pos);
		void    ReturnToZeroPosition();
		void	SetTargetVelocity(double _cmd_vel);
		bool    SetDriverParam();
		void    SetOperationMode(OpMode _mode);
		OpMode	GetActualOperationMode();

		void    ControlUnitSync();
		bool    Enable();
		bool    Disable();

		bool    IsEnableState();
		bool    IsFaultState();
		bool    IsEmergencyState();
		bool	IsLimitExceeded();
		short   CheckErrorCode();
		short   CheckWarnCode();
	
	private:
		DriverState	GetDriverState();
		DriverState_General GetDriverState_General();
		bool    IsDriverReady();
		bool    IsDriverShielded();
		bool    IsSoftLimitExceeded();
		bool    IsHardLimitExceeded();
		void    ControlUnitSync_General();
		bool	Enable_Beckhoff();
		bool    Disable_Beckhoff();
		bool    Enable_General();
		bool    Disable_General();
		void    SetOperationMode_Beckhoff(OpMode _mode);
		void    SetOperationMode_General(OpMode _mode);
	};
}

