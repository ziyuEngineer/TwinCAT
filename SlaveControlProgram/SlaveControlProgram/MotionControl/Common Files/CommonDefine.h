#pragma once

#include "../MotionControlInterfaces.h"
#include "../MotionControlServices.h"

constexpr double kRadius2Degree = 57.2957795;	// This constant is used to convert an angle measured in radians into degrees.
constexpr double kDegree2Radius = 0.01745329;	// This constant is used to convert an angle measured in degrees into radians.
constexpr double kEPSILON = 1e-6;				// Define a very small value for floating-point operations.
constexpr double kINF = 4.2950e+09;				// Define a very large value that can be used as an approximation for infinity.

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

enum class ModuleNum
{
	System = 0,
	MotorControl = 1
};

enum class ModuleSub
{
	PosOver = 0,
	VelOver = 1,
	TorOver = 2
};

enum class AxisNum
{
	PrimaryAxis = 0,
	X_Axis = 1,
	Y_Axis = 2,
	Z_Axis = 3,
	A_Axis = 4,
	B_Axis = 5,
	C_Axis = 6
};

enum class ErrorClass
{
	classA = 1,
	classB = 2,
	classC = 3,
	classD = 4,
	classE = 5
};
