#include "../TcPch.h"
#pragma hdrstop
#include "CommandManager.h"

CCommandManager::CCommandManager() {
    m_CommandBuffer = new Ringbuffer<FullCommand, 32768>();
    m_LocalReadIndex = 0;
}

CCommandManager::~CCommandManager() 
{ 
    delete m_CommandBuffer; 
}

void CCommandManager::Input() 
{
    m_LocalWriteIndex = *m_CommandWriteIndex;
    if (!(m_LocalWriteIndex <= m_LocalReadIndex)) 
    {
        int commandsToWrite = m_LocalWriteIndex - m_LocalReadIndex;

        if (m_CommandBuffer->writeAvailable() > commandsToWrite && commandsToWrite > 0) 
        {
            m_CommandBuffer->writeBuff(m_CommandInput, commandsToWrite);
            m_LocalReadIndex = m_LocalWriteIndex;
        }
    }
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

bool CCommandManager::GetCommandFromBuffer()
{
    if (!m_CommandBuffer->isEmpty())
    {
        return m_CommandBuffer->remove(&m_Command);
    }
    return false;
}

void CCommandManager::Reset() 
{
    m_CommandBuffer->producerClear();
    m_LocalReadIndex = 0;
    memset(&m_Command, 0, sizeof(FullCommand));
}

int CCommandManager::GetMaxBufferSize() 
{ 
    return m_CommandBuffer->getBufferSize(); 
}

int CCommandManager::GetCurrentBufferSize()
{
    int current_ringbuffer_size = m_CommandBuffer->getHead() - m_CommandBuffer->getTail();
    return current_ringbuffer_size;
}