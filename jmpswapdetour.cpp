//////////////////////////////////////////////////////////////////////////
//  Very Simple Jmp and Swap Detour Class for MSVC++
//  
//  Copyright (c) 2010-2012 Harry Pidcock
//  
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following
//  conditions:
//  
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////

#include "jmpswapdetour.h"
#include <Windows.h>

//************************************
// Method:    CJmpSwapDetour
// FullName:  CJmpSwapDetour::CJmpSwapDetour
// Access:    public 
// Parameter: void * function
// Parameter: void * detour
//************************************
CJmpSwapDetour::CJmpSwapDetour(void *function, void *detour) : 
	m_bDetoured(false), 
	m_Target(function), 
	m_Detour(detour),
	m_Jmp(NULL),
	m_JmpSize(0)
{
	m_Target = FollowJmp(m_Target);
	m_Detour = FollowJmp(m_Detour);

	m_FunctionSize = GetFunctionSize(m_Target);

	m_OriginalBytes = new unsigned char[m_FunctionSize];
	memcpy_s(m_OriginalBytes, m_FunctionSize, m_Target, m_FunctionSize);

	BuildJump(m_Detour);

	Swap();
}

CJmpSwapDetour::~CJmpSwapDetour(void)
{
	if(m_bDetoured)
		Swap();

	delete [] m_OriginalBytes;
}

//************************************
// Method:    Swap
// FullName:  CJmpSwapDetour::Swap
// Access:    public 
//************************************
void CJmpSwapDetour::Swap(void)
{
	DWORD protect;
	VirtualProtect(m_Target, m_FunctionSize, PAGE_EXECUTE_READWRITE, &protect);

	if(m_bDetoured)
	{
		memcpy_s(m_Target, m_FunctionSize, m_OriginalBytes, m_FunctionSize);
	}
	else
	{
		memcpy_s(m_Target, m_FunctionSize, m_Jmp, m_JmpSize);
	}

	VirtualProtect(m_Target, m_FunctionSize, protect, &protect);

	m_bDetoured = !m_bDetoured;
}

#ifdef _WIN64
extern "C" void JumpInstruction(void);
#else
extern "C" void __stdcall JumpInstruction(void);
#endif

// This function will emit an absolute jump.
void CJmpSwapDetour::BuildJump(void *target)
{
	unsigned char *jmp = (unsigned char *)FollowJmp(JumpInstruction);

	m_JmpSize = 0;
	for(size_t i = 0; i < 32; i++)
	{
		if(*((unsigned int *)&jmp[i]) == 0xCCCCCCCC)
		{
			m_JmpSize = i;
			break;
		}
	}

	m_Jmp = malloc(m_JmpSize);
	memcpy(m_Jmp, jmp, m_JmpSize);

	for(size_t i = 0; i < m_JmpSize; i++)
	{
		size_t *tPtr = (size_t *)((size_t)m_Jmp + i);

#ifdef _WIN64
		size_t ptr = 0xAABBCCDDEEFF0011;
#else
		size_t ptr = 0xAABBCCDD;
#endif

		if(*tPtr == ptr)
		{
			*tPtr = (size_t)target;
			break;
		}
	}
}


// Sig scan for the last ret retn retf etc.
// In Windows the PE's memory is going to be 0xCC as padding (between functions, data segments etc),
//   so we can determine the end of a function by this.
const void *c_ReturnSigs[4] = {"\xC2\xFF\xFF\xCC", "\xC3\xCC\xCC", "\xCA\xFF\xFF\xCC", "\xCB\xCC\xCC"};
const char *c_ReturnMasks[4] = {"x??x", "xxx", "x??x", "xxx"};

bool CJmpSwapDetour::MatchSigMask(void *memory, const void *sig, size_t sigLength, const char *mask) const
{
	for(size_t i = 0; i < sigLength; i++)
	{
		if(((unsigned char *)memory)[i] != ((unsigned char *)sig)[i] &&
			mask[i] == 'x')
		{
			return false;
		}
	}

	return true;
}

size_t CJmpSwapDetour::GetFunctionSize(void *function) const
{
	// Scan the next 4kb (that's a big function :/) for our function's end.
	for(size_t i = 0; i < 4096; i++)
	{
		for(int m = 0; m < 4; m++) // We are pretty much guessing here, but it could be one of 4 return styles.
		{
			if(MatchSigMask(
				(void *)((size_t)function + i), 
				c_ReturnSigs[m], 
				strlen((const char *)(c_ReturnSigs[m])), 
				c_ReturnMasks[m]))
			{
				return i + 1;
			}
		}
	}

	return 0;
}

void *CJmpSwapDetour::FollowJmp(void *ptr) const
{
	if(*(unsigned char *)ptr != 0xE9)
		return ptr;

	void *offsetPtr = (void *)((size_t)ptr + 1); // Skip the jmp instruction opcode.

	int offset = *(int *)offsetPtr; // Will be an 32bit int even in x64

	return (void *)((size_t)offsetPtr + offset + sizeof(int)); // Offset is from eip
}