#pragma once

#include "Filter.h"
#include<array>

namespace Axis
{
	
	class CDifferentiator
	{
	public:
		CDifferentiator();
		~CDifferentiator();

		void DiffProcess(double _input, double* _first_derivative, double* _second_derivative);

	private:
		bool m_bInit = false;

		std::array<double, 3> m_PreInput = { 0.0 };

		CFilter firstDerivativeFilter;
		CFilter secondDerivativeFilter;
	};
}