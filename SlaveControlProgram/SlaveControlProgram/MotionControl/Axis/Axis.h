#pragma once
#include "CommonDefine.h"
#include "Differentiator.h"

namespace Axis
{
	struct AxisParam
	{
		short  abs_dir = 1;				// Absolute encoder (positive) direction
		long   abs_encoder_res = 1;		// Absolute encoder (positive) resolution

		double rated_curr = 1.0;		// Rated current
		double rated_tor = 1.0;			// Rated torque
		short  tor_dir = 1;				// Torque (positive) direction

		long   abs_zero_pos = 0;		// Zero position of absolute encoder
		double abs_pos_ub = 0.0;		// Position upperbound of absolute encoder
		double abs_pos_lb = 0.0;		// Position lowerbound of absolute encoder
										
		double tor_cons = 0.0;			// Torque constant, = rated_tor / rated_curr
		
		double reduction_ratio = 1.0;	// Reduction ratio

		bool   abs_encoder_type = false;	// Reserved for distinguishing encoder type between single-turn and multi-turns

		short  tor_pdo_max = 50;		    // Limit of data transferred to torque pdo
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

	// Parameters related to driver
	public:
		// Input from driver
		unsigned short StatusWord = 0;
		long FdbPosVal = 0;	 // Abs Pos feedback
		short FdbTorVal = 0; // Torque feedback
		long FdbVelVal = 0;  // Velocity feedback
		unsigned int ErrorCode = 0;
		unsigned int WarningCode = 0;

		// Output to driver
		unsigned short ControlWord = 0;
		short OperationMode = 0;
		long TargetTor = 0;
		long TargetPos = 0;
		long TargetVel = 0;

	public:
		double  GetFeedbackPosition();
		double  GetFeedbackVelocity();
		void	ComputeAcceleration(double _input, double* _first_derivative, double* _second_derivative);
		double  GetFeedbackTorque();

		short   SendTargetTorque(double _jnt_tor);
		long    SendTargetPosition(double _ref_pos);
		long    ReturnToZeroPosition();
		long	SendTargetVelocity(double _ref_vel);
		bool    SetAxisParam(AxisParam _mc_param);
		void    SetOperationMode(OpMode mode);

		bool    QuickStop();
		bool    FaultReset();
		bool    ServoOn();
		bool    ServoOff();

		bool    IsDriverReady();
		bool    IsEnableState();
		bool    IsFaultState();
		unsigned int    CheckErrorCode();
		unsigned int    CheckWarnCode();
	
	private:
		DriverState	GetDriverState();
	};
}

