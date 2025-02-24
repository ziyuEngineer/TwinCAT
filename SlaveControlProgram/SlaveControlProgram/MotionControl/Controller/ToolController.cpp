#pragma once
#pragma hdrstop

#include "ToolController.h"

CToolController::CToolController()
{
	m_ToolState = ToolState::eToolStateInitialize;
}

CToolController::~CToolController()
{
}

void CToolController::MapParameters(ModuleToolInputs* inputs, ModuleToolOutputs* outputs)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
}

void CToolController::Input()
{

}

void CToolController::Output()
{
	m_pOutputs->StateTool = m_ToolState;
}