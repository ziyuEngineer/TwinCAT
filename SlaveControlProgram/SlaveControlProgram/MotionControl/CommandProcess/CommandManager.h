#pragma once
#include "CommonDefine.h"
#include "ring_buffer.hh"

class CCommandManager {
public:
    CCommandManager();
    ~CCommandManager();

protected:
    ULONG* m_CommandWriteIndex = nullptr;
    ULONG* m_CommandReadIndex = nullptr;
    FullCommand* m_CommandInput = nullptr;
    Ringbuffer<FullCommand, kMaxBufferSize>* m_CommandBuffer;

public:
    void Input();
    void Output();

    void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs);

    bool GetCommandFromBuffer();
    const FullCommand& GetCurrentCommand() const { return m_Command; };
   
    void Reset();

    ULONG GetMaxBufferSize();
    ULONG GetCurrentBufferSize();
    void GetLatestCommand(FullCommand& cmd);

    FullCommand m_Command;

    ULONG m_LocalReadIndex;
    ULONG m_LocalWriteIndex;
};