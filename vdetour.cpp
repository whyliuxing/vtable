//////////////////////////////////////////////////////////////////////////
//	Virtual Table Detour and Hooks Class for MSVC++						//
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

#include "vdetour.h"
#include <windows.h>

//************************************
// Method:    Hint
// FullName:  CVTable::Hint
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: size_t args
// Parameter: const char * name
//************************************
void CVTable::Hint(size_t vindex, size_t args, const char *name)
{
	// Create our CVEntry for the virtual table entry.
	CVEntry *entry = new CVEntry();

	entry->m_pVEntry = this->m_pVTable + vindex;
	entry->m_iArgCount = args;
	entry->m_pName = name;
	entry->m_pDetour = NULL;
	entry->m_pOriginalEntry = *(entry->m_pVEntry);
	entry->m_CallHooks.clear();
	entry->m_ReturnHooks.clear();

	this->m_Entries[vindex] = entry;

	// Create our trampoline function.
	this->CreateTrampoline(entry);

	// Modify the virtual table to call our trampoline function.
	DWORD oldProtect;
	VirtualProtect((void *)entry->m_pVEntry, sizeof(size_t), PAGE_READWRITE, &oldProtect);
	*entry->m_pVEntry = entry->m_pTrampoline;
	VirtualProtect((void *)entry->m_pVEntry, sizeof(size_t), oldProtect, &oldProtect);
}

//************************************
// Method:    CVTable
// FullName:  CVTable::CVTable
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: void * * vtable
//************************************
CVTable::CVTable(void **vtable)
{
	this->m_pVTable = vtable;
}

//************************************
// Method:    ~CVTable
// FullName:  CVTable::~CVTable
// Access:    public 
// Returns:   
// Qualifier:
//************************************
CVTable::~CVTable()
{
	this->RevertAll();
}

//************************************
// Method:    GetEntry
// FullName:  CVTable::GetEntry
// Access:    private 
// Returns:   std::map<size_t, CVEntry *>::iterator
// Qualifier:
// Parameter: size_t vindex
//************************************
std::map<size_t, CVEntry *>::iterator CVTable::GetEntry(size_t vindex)
{
	return this->m_Entries.find(vindex);
}

//************************************
// Method:    Detour
// FullName:  CVTable::Detour
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: void * func
//************************************
void CVTable::Detour(size_t vindex, void *func)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		itor->second->m_pDetour = func;
	}
}

//************************************
// Method:    RemoveDetour
// FullName:  CVTable::RemoveDetour
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
//************************************
void CVTable::RemoveDetour(size_t vindex)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		this->m_Entries.erase(itor);
	}
}

//************************************
// Method:    CallHook
// FullName:  CVTable::CallHook
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: void * func
//************************************
void CVTable::CallHook(size_t vindex, void *func)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		itor->second->m_CallHooks.push_back(func);
	}
}

//************************************
// Method:    RemoveCallHook
// FullName:  CVTable::RemoveCallHook
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: void * func
//************************************
void CVTable::RemoveCallHook(size_t vindex, void *func)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		CVEntry *entry = itor->second;

		std::vector<void *>::iterator itorx = entry->m_CallHooks.begin();

		while(itorx != entry->m_CallHooks.end())
		{
			if(*itorx == func)
			{
				entry->m_CallHooks.erase(itorx);
				return;
			}
			itorx++;
		}
	}
}

//************************************
// Method:    ReturnHook
// FullName:  CVTable::ReturnHook
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: void * func
//************************************
void CVTable::ReturnHook(size_t vindex, void *func)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		itor->second->m_ReturnHooks.push_back(func);
	}
}

//************************************
// Method:    RemoveReturnHook
// FullName:  CVTable::RemoveReturnHook
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
// Parameter: void * func
//************************************
void CVTable::RemoveReturnHook(size_t vindex, void *func)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		CVEntry *entry = itor->second;

		std::vector<void *>::iterator itorx = entry->m_ReturnHooks.begin();

		while(itorx != entry->m_ReturnHooks.end())
		{
			if(*itorx == func)
			{
				entry->m_ReturnHooks.erase(itorx);
				return;
			}
			itorx++;
		}
	}
}

//************************************
// Method:    Revert
// FullName:  CVTable::Revert
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: size_t vindex
//************************************
void CVTable::Revert(size_t vindex)
{
	std::map<size_t, CVEntry *>::iterator itor = this->GetEntry(vindex);

	if(itor != this->m_Entries.end())
	{
		CVEntry *entry = itor->second;

		// Restore the virtual table entry.
		DWORD oldProtect;
		VirtualProtect((void *)entry->m_pVEntry, sizeof(size_t), PAGE_READWRITE, &oldProtect);
		*(entry->m_pVEntry) = entry->m_pOriginalEntry;
		VirtualProtect((void *)entry->m_pVEntry, sizeof(size_t), oldProtect, &oldProtect);

		// Delete our trampoline function.
		VirtualProtect((void *)entry->m_pTrampolineMem, sizeof(entry->m_pTrampolineMemSz), PAGE_READWRITE, &oldProtect);
		free((void *)entry->m_pTrampolineMem);

		delete entry;

		this->m_Entries.erase(itor);
	}
}

//************************************
// Method:    RevertAll
// FullName:  CVTable::RevertAll
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void CVTable::RevertAll()
{
	std::map<size_t, CVEntry *>::reverse_iterator itor = this->m_Entries.rbegin();

	while(itor != this->m_Entries.rend())
	{
		this->Revert(itor->first);
		itor++;
	}
}

//************************************
// Method:    BaseVHook
// FullName:  CVTable::BaseVHook
// Access:    private 
// Returns:   void *
// Qualifier:
//************************************
void *CVTable::BaseVHook()
{
	// Warning: Cannot make direct calls in this thread since
	//		they are all relative, we need calls to absolute
	//		addresses since we copy the original function.

	__asm emms;
	
	// Find the start of this function.

	// Get the CVEntry pointer.
	CVEntry *entry;

	__asm
	{
		mov entry, 0xAABBCCDD; // This will get replaced with the pointer to the CVEntry for this function.
	}

	size_t argCount = entry->m_iArgCount;
	size_t hookCount = entry->m_CallHooks._Mylast - entry->m_CallHooks._Myfirst;

	// Call all the hooks.
	for(size_t i = 0; i < hookCount; i++)
	{
		void *hook = ((void **)entry->m_CallHooks._Myfirst)[i]; // Might have some problems with other std libraries.
		__asm
		{
			mov ebx, esp;
			mov ecx, argCount;
			jcxz endLoop;
argLoop:
			// Push all the arguments.
			mov eax, [ebp + 8 + ecx * 4];
			push eax;
			jcxz endLoop;
			sub ecx, 1;
			jmp argLoop;
endLoop:
			mov ecx, this;
			mov eax, hook;
			call eax;
			mov esp, ebx;
		}
	}

	void *retFunc = entry->m_pDetour;

	if(retFunc == NULL)
		retFunc = entry->m_pOriginalEntry;

	size_t popCount = argCount * sizeof(size_t);

	// Call either the original function or the detour.
	__asm
	{
		mov ebx, esp;
		mov ecx, argCount;
		jcxz endLoopb;
argLoopb:
		// Push all the arguments.
		mov eax, [ebp + 8 + ecx * 4];
		push eax;
		jcxz endLoopb;
		sub ecx, 1;
		jmp argLoopb;
endLoopb:
		mov ecx, this;
		mov eax, retFunc;
		call eax;
		mov esp, ebx;
	}

	__asm
	{
		sub esp, 8;
		fstp [esp];

		push eax;
		push edx;
	}

	hookCount = entry->m_ReturnHooks._Mylast - entry->m_ReturnHooks._Myfirst;

	// Call all the hooks.
	for(size_t i = 0; i < hookCount; i++)
	{
		void *hook = ((void **)entry->m_ReturnHooks._Myfirst)[i]; // Might have some problems with other std libraries.
		__asm
		{
			mov ebx, esp;
			mov ecx, argCount;
			jcxz endLoop2;
argLoop2:
			// Push all the arguments.
			mov eax, [ebp + 8 + ecx * 4];
			push eax;
			jcxz endLoop2;
			sub ecx, 1;
			jmp argLoop2;
endLoop2:
			mov ecx, this;
			mov eax, hook;
			call eax;
			mov esp, ebx;
		}
	}

	__asm
	{
		pop edx;
		pop eax;

		fld [esp];
		//fxch st(7);
		add esp, 8;
	}

	// Do not touch eax or edx or fp7-6. It can be our return value.

	__asm
	{
		// Since we are restoring registers, we will just use the SSE
		//		registers to store the amount of data to pop off the
		//		stack.
		movss xmm1, popCount;

		pop edi;
		pop esi;
		pop ebx;
		mov esp, ebp;
		pop ebp;

		movss xmm3, [esp] // Save return address.

		mov [esp], esp; // Move stack pointer into xmm2
		movss xmm2, [esp];
		paddd xmm2, xmm1; // Shift stack pointer by the popCount saved above.
		movss [esp], xmm2; // Move the adjusted stack pointer back into esp.
		mov esp, [esp];

		// The top of the stack needs to be the memory location we
		// 		return to.
		movss [esp], xmm3;
		
		//fxch st(6);
		ret;
	}
}

//************************************
// Method:    CreateTrampoline
// FullName:  CVTable::CreateTrampoline
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: CVEntry * entry
//************************************
void CVTable::CreateTrampoline(CVEntry *entry)
{
	// Get the size and location of void *CVTable::BaseVHook()
	void *BaseVHookFunc = NULL;
	size_t BaseVHookFuncSz = NULL;
	void *ReturnSignature = (void *)"\xC3\xCC\xCC";

	__asm
	{
		push eax;
		mov eax, CVTable::BaseVHook;
#ifdef _DEBUG
		add eax, 1;
		add eax, [eax];
		add eax, 4;
#endif
		mov BaseVHookFunc, eax;
		pop eax;
	}

	for(size_t i = 0; i < 1024; i++)
	{
		if(memcmp((void *)((size_t)BaseVHookFunc + i), ReturnSignature, 3) == 0) 
		{
			BaseVHookFuncSz = i + 1;
			break;
		}
	}

	// The size of a pointer, the new function and some padding.
	size_t BaseVHookFuncSzFull = BaseVHookFuncSz + sizeof(size_t);

	entry->m_pTrampolineMem = malloc(BaseVHookFuncSzFull);
	entry->m_pTrampolineMemSz = BaseVHookFuncSzFull;

	size_t trampoline = (size_t)entry->m_pTrampolineMem;

	// Copy the function.
	memcpy((void *)trampoline, BaseVHookFunc, BaseVHookFuncSz);
	
	for(size_t i = 0; i < BaseVHookFuncSz; i++)
	{
		size_t *value = (size_t *)(trampoline + i);

		if(*value == 0xAABBCCDD)
		{
			*value = (size_t)entry;
			break;
		}
	}

	trampoline += BaseVHookFuncSz;

	// Padding.
	memset((void *)trampoline, 0xCC, sizeof(size_t));

	entry->m_pTrampoline = entry->m_pTrampolineMem;

	// Set this memory to executable.
	DWORD oldProtect;
	VirtualProtect((void *)entry->m_pTrampolineMem, sizeof(entry->m_pTrampolineMemSz), PAGE_EXECUTE_READWRITE, &oldProtect);
}