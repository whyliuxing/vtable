//////////////////////////////////////////////////////////////////////////
//	Very Simple Jmp and Swap Detour Class for MSVC++					//
//																		//
//	Copyright (c) 2010-2012 Harry Pidcock								//
//																		//
//	Permission is hereby granted, free of charge, to any person			//
//	obtaining a copy of this software and associated documentation		//
//	files (the "Software"), to deal in the Software without				//
//	restriction, including without limitation the rights to use,		//
//	copy, modify, merge, publish, distribute, sublicense, and/or sell	//
//	copies of the Software, and to permit persons to whom the			//
//	Software is furnished to do so, subject to the following			//
//	conditions:															//
//																		//
//	The above copyright notice and this permission notice shall be		//
//	included in all copies or substantial portions of the Software.		//
//																		//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,		//
//	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES		//
//	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND			//
//	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT			//
//	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,		//
//	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING		//
//	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR		//
//	OTHER DEALINGS IN THE SOFTWARE.										//
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
	m_Detour(detour)
{
	m_FunctionSize = GetFunctionSize(m_Target);

	m_OriginalBytes = new unsigned char[m_FunctionSize];
	memcpy_s(m_OriginalBytes, m_FunctionSize, m_Target, m_FunctionSize);

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
	VirtualProtect(m_Target, m_FunctionSize, PAGE_READWRITE, &protect);

	if(m_bDetoured)
	{
		memcpy_s(m_Target, m_FunctionSize, m_OriginalBytes, m_FunctionSize);
	}
	else
	{
		EmitJump(m_Target, m_Detour);
	}

	VirtualProtect(m_Target, m_FunctionSize, protect, &protect);

	m_bDetoured = !m_bDetoured;
}

// This function will emit an absolute jump.
void CJmpSwapDetour::EmitJump(void *dest, void *target) const
{
	unsigned char *mem = (unsigned char *)dest;
	// mov eax imm32;
	mem[0] = 0xB8;
	*((void **)&mem[1]) = target;

	// jump eax;
	mem[sizeof(void *) + 1] = 0xFF;
	mem[sizeof(void *) + 2] = 0xE0;
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