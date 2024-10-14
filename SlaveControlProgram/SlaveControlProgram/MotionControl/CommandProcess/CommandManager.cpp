#include "../TcPch.h"
#pragma hdrstop
#include "CommandManager.h"

CCommandManager::CCommandManager()
{

}

CCommandManager::~CCommandManager()
{

}

void CCommandManager::Input()
{
	m_LocalWriteIndex = *m_CommandWriteIndex;
}

void CCommandManager::Output()
{
	*m_CommandReadIndex = m_LocalReadIndex;
}

void CCommandManager::MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs)
{
	m_CommandWriteIndex = &_pInputs->CommandWriteIndex;
	m_CommandReadIndex = &_pOutputs->CommandReadIndex;
	m_CommandInput = _pInputs->CommandInput;
}

bool CCommandManager::Initialize()
{
	m_LocalReadIndex = 0;
	return (static_cast<int>(m_LocalWriteIndex) >= m_MinStartLength) && (m_MinStartLength > 0);
}

void CCommandManager::GetCommand()
{
	m_MinStartLength = 0; // Reset for next RPC
	if (m_LocalReadIndex < m_LocalWriteIndex)
	{
		// Use hybrid commands
		int ReadArrayIndex = m_LocalReadIndex & kCommandBufferMask;
		m_Command = *(m_CommandInput + ReadArrayIndex);
		m_LocalReadIndex++;
	}
}

void CCommandManager::Reset()
{
	m_LocalReadIndex = 0;
	memset(&m_Command, 0, sizeof(FullCommand));
}

int CCommandManager::GetBufferSize()
{
	return kCommandMaxLength;
}

void CCommandManager::SetMinLengthToStart(int _min)
{
	m_MinStartLength = _min;
}