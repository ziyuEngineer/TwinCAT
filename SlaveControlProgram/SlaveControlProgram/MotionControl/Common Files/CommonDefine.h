#pragma once
#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

#include "../MotionControlInterfaces.h"
#include "../MotionControlServices.h"
#include "TcRouterInterfaces.h"
#include "TcEventLoggerInterfaces.h"

// Common define
constexpr double kRadius2Degree = 57.2957795;	// This constant is used to convert an angle measured in radians into degrees.
constexpr double kDegree2Radius = 0.01745329;	// This constant is used to convert an angle measured in degrees into radians.
constexpr double kEpsilon = 1e-6;				// Define a very small value for floating-point operations.
constexpr double kInf = 4.2950e+09;				// Define a very large value that can be used as an approximation for infinity.

// MotionControl define
constexpr int kMaxMotorNumPerAxis = 2;
constexpr int kMaxAxisNum = 5;
constexpr int kSpindleDriverNum = 1;

// Xml define
constexpr int kMaxRead = 8192;

// Interpolation define
constexpr bool kInterpolated = true;
constexpr bool kNotInterpolated = false;

// Command buffer define
constexpr ULONG kMaxBufferSize = 32768;

// Torque check const
constexpr int kCheckWindowSize = 100;
constexpr double kTorCmdDeviationTolerance = 40.0;
constexpr double kTorFollowingErrorTolerance = 40.0;

/**
 * @brief Restrict a given value 'x' to be within the range specified by 'minimum' and 'maximum'.
 * 
 */
#define CLAMP(x,minimum,maximum)	((x) < (minimum) ? (minimum) : ((x) > (maximum) ? (maximum) : (x)))

/**
 * @brief Return the greater of two values 'a' and 'b'.
 * 
 */
#define MAX(a,b)					(((a) < (b)) ? (b) : (a))

/**
 * @brief Return the lesser of two values 'a' and 'b'.
 * 
 */
#define MIN(a,b)					(((a) > (b)) ? (b) : (a))

/**
 * @brief Return the sign of a given value 'a'.
 * 
 */
#define SIGN(a)						(((float)(a)) > 0 ? 1 : (((float)(a)) < 0 ? -1 : 0))

/**
 * @brief Convert an angle from degrees to radians.
 * 
 */
#define DEG2RAD(a)					((a) * kDegree2Radius)

/**
 * @brief Convert an angle from radians to degrees.
 * 
 */
#define RAD2DEG(a)					((a) * kRadius2Degree)

// For generating error codes in the future
enum class ProjectSeries
{
	ThreeAxis = 3,
	FourAxis = 4,
	FiveAxis = 5
};

enum class ModuleName
{
	MainModule = 0,
	AxisGroup = 1,
	Spindle = 2
};

enum class ModuleError
{
	SysErr = 0,
	StatusErr = 1,
	PosUpperOver = 2,
	PosLowerOver = 3,
	VelOver = 4,
	TorOver = 5,
	GantryDeviationOver = 6,
	TorFollowingOscillation = 7,
	TorCmdOscillation = 8
};

enum class ErrorClass
{
	classA = 0,
	classB = 1,
	classC = 2,
	classD = 3,
	classE = 4
};

enum class DriverStatus
{
	DriverWorking = 0,
	DriverDisabled = 1,
	DriverWarning = 2,
	DriverFault = 3
};

#endif
