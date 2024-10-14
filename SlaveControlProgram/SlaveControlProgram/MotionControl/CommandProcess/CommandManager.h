#pragma once
#include "CommonDefine.h"

class CCommandManager
{
public:
	CCommandManager();
	~CCommandManager();

protected:
	ULONG* m_CommandWriteIndex = nullptr;
	ULONG* m_CommandReadIndex = nullptr;
	FullCommand* m_CommandInput = nullptr;

public:
	void Input();
	void Output();

	void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs);
	
	bool Initialize();
	void GetCommand();
	void Reset();

	int GetBufferSize();
	void SetMinLengthToStart(int _min);

	FullCommand m_Command;

	ULONG m_LocalReadIndex;
	ULONG m_LocalWriteIndex;

	int m_MinStartLength = 0;
};