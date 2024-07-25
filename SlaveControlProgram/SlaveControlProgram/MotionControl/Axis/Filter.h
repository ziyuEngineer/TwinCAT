#pragma once

#include "CommonDefine.h"

namespace Axis
{
	struct LowPassFilterParam
	{
		double cutoff_frequency = 0.0;  // cut-off frequency
		double cycle_time = 0.001;		// sampling period
		double alpha = 0.0;				// filter coefficient
		double output = 0.0;			// filtered result
	};

	// TODO: Realize a variety of filters for choice

	class CFilter
	{
	public:
		CFilter();
		~CFilter();

	public:
		LowPassFilterParam FilterParam;
		void SetLowPassFilterParam(LowPassFilterParam _filter_param);
		double LowPassFilter(double input);
	};
}