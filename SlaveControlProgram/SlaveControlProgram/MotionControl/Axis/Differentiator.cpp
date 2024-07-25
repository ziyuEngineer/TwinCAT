#pragma once
#pragma hdrstop

#include "Differentiator.h"

namespace Axis
{
	CDifferentiator::CDifferentiator()
	{

	}

	CDifferentiator::~CDifferentiator()
	{

	}

	/**
	 * @brief Differentiation
	 * 
	 * @param _input: position or velocity
	 * @param _first_derivative:	if _input is position, return velocity;			if_input is velocity, return acceleration;
	 * @param _second_derivative:	if _input is position, return acceleration;		if_input is velocity, return jerk;
	 */
	void CDifferentiator::DiffProcess(double _input, double* _first_derivative, double* _second_derivative)
	{
		if (!m_bInit)
		{
			m_PreInput[0] = _input;
			m_PreInput[1] = _input;
			m_PreInput[2] = _input;

			LowPassFilterParam tempFilterParam;
			tempFilterParam.cutoff_frequency = 10;
			tempFilterParam.cycle_time = 0.001;

			firstDerivativeFilter.SetLowPassFilterParam(tempFilterParam);
			secondDerivativeFilter.SetLowPassFilterParam(tempFilterParam);

			m_bInit = true;
		}

		// Update
		m_PreInput[2] = m_PreInput[1];
		m_PreInput[1] = m_PreInput[0];
		m_PreInput[0] = _input;

		// Calculate derivatives
		double first_derivative = (m_PreInput[0] - m_PreInput[2]) / (firstDerivativeFilter.FilterParam.cycle_time);
		double second_derivative = (m_PreInput[0] - 2 * m_PreInput[1] + m_PreInput[2]) / (secondDerivativeFilter.FilterParam.cycle_time * secondDerivativeFilter.FilterParam.cycle_time);
		
		*_first_derivative = firstDerivativeFilter.LowPassFilter(first_derivative);
		*_second_derivative = secondDerivativeFilter.LowPassFilter(second_derivative);
	}
}
