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

// Note:
//		This class is only to be used when you know that only 1 thread will
//		be calling the detoured function at any time.
//		If you want to have thread safe detours. Use Microsoft Detour Library.

#ifndef __JMPSWAPDETOUR_H__
#define __JMPSWAPDETOUR_H__

class CJmpSwapDetour
{
public:
	CJmpSwapDetour(void *function, void *detour);
	virtual ~CJmpSwapDetour(void);

	// Swap between our detour and original function.
	// Because of this, we are not thread-safe.
	void Swap(void);

	// Get the detoured function.
	template<typename T>
	T GetTarget(void) const
	{
		return (T)m_Target;
	};

	// Get the detour function.
	template<typename T>
	T GetDetour(void) const
	{
		return (T)m_Detour;
	};

private:
	bool m_bDetoured;

	void *m_OriginalBytes;
	void *m_Target;
	void *m_Detour;
	void *m_Jmp;
	size_t m_JmpSize;

	size_t m_FunctionSize;

	void BuildJump(void *target);

	bool MatchSigMask(void *memory, const void *sig, size_t sigLength, const char *mask) const;
	size_t GetFunctionSize(void *function) const;

	void *FollowJmp(void *ptr) const;
};

#endif // __JMPSWAPDETOUR_H__