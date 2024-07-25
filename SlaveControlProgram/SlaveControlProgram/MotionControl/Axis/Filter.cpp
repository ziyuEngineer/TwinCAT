#pragma once
#pragma hdrstop

#include "Filter.h"

namespace Axis
{
	CFilter::CFilter()
	{

	}

	CFilter::~CFilter()
	{

	}

	/**
	 * @brief Set low pass filter parameters
	 *
	 * @param _filter_param
	 */
	void CFilter::SetLowPassFilterParam(LowPassFilterParam _filter_param)
	{
		FilterParam.cutoff_frequency = _filter_param.cutoff_frequency;
		FilterParam.cycle_time = _filter_param.cycle_time;

		FilterParam.alpha = (2 * PI * FilterParam.cutoff_frequency * FilterParam.cycle_time) 
						/ (1 + 2 * PI * FilterParam.cutoff_frequency * FilterParam.cycle_time);
	}

	/**
	 * @brief Low pass filter process
	 *
	 * @return double
	 */
	double CFilter::LowPassFilter(double input)
	{
		FilterParam.output = FilterParam.alpha * input + (1 - FilterParam.alpha) * FilterParam.output;

		return FilterParam.output;
	}
}