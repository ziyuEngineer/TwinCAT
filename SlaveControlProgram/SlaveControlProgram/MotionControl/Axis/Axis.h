#pragma once
#include "CommonDefine.h"
#include "Differentiator.h"

namespace Axis
{
	struct AxisParam
	{
		short abs_dir = 1;				// Absolute encoder (positive) direction
		LONG abs_encoder_res = 1;		// Absolute encoder resolution

		double rated_curr = 1.0;		// Rated current
		double rated_tor = 1.0;			// Rated torque
		short tor_dir = 1;				// Torque (positive) direction

		LONG abs_zero_pos = 0;		// Zero position of absolute encoder
		LONG abs_pos_ub = 0;		// Position upperbound of absolute encoder
		LONG abs_pos_lb = 0;		// Position lowerbound of absolute encoder
										
		double tor_cons = 0.0;			// Torque constant, = rated_tor / rated_curr
		
		double transmission_ratio = 1.0;	// Transmisssion ratio

		bool abs_encoder_type = false;	// Reserved for distinguishing encoder type between single-turn and multi-turns

		short tor_pdo_max = 50;		    // Limit of data transferred to torque pdo
		short additive_tor = 0;		// Compensating torque

		short positive_hard_bit = 15;
		short negative_hard_bit = 15;
	};
}

namespace Axis
{
	class CAxis : protected CDifferentiator
	{
	public:
		CAxis();
		~CAxis();

	protected:
		AxisParam m_AxisParam;

		bool m_Flip = false;
		int m_DigitalMask = 15;// 14(1110):y+; 13(1101):y-; 11(1011):z+; 7(0111):z-
		int m_OpModeMask = 7; // 2#00000111

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
		double  GetFeedbackPosition();
		double  GetFeedbackVelocity();
		void	ComputeAcceleration(double _input, double* _first_derivative, double* _second_derivative);
		double  GetFeedbackTorque();

		short   SendTargetTorque(double _cmd_tor);
		long    SendTargetPosition(double _cmd_pos);
		long    ReturnToZeroPosition();
		long	SendTargetVelocity(double _cmd_vel);
		bool    SetAxisParam(AxisParam _mc_param);
		void    SetOperationMode(OpMode mode);
		OpMode	GetActualOperationMode();

		void    ControlUnitSync();
		bool    Enable();
		bool    Disable();

		bool    IsDriverReady();
		bool    IsEnableState();
		bool    IsFaultState();
		bool    IsEmergencyState();
		bool	IsLimitExceeded();
		short   CheckErrorCode();
		short   CheckWarnCode();
	
	private:
		DriverState	GetDriverState();
		bool    IsSoftLimitExceeded();
		bool    IsHardLimitExceeded();
	};
}

