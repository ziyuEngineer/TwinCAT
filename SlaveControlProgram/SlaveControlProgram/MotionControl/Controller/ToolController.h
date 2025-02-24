#pragma once
#include "CommonDefine.h"

class CToolController
{
public:
	CToolController();
	~CToolController();

public:
	void MapParameters(ModuleToolInputs* inputs, ModuleToolOutputs* outputs);
	ToolState m_ToolState;

	void Input();
	void Output();

private:
	ModuleToolInputs* m_pInputs = nullptr;
	ModuleToolOutputs* m_pOutputs = nullptr;
};