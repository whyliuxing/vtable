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

#include <map>
#include <vector>

#pragma once

#ifndef vdetour_h__
#define vdetour_h__

//************************************
// Class: CVEntry
// Description: Internal class for
//		storing virtual table index
//		information.
//************************************
class CVEntry
{
public:
	void *m_pDetour;
	std::vector<void *> m_Hooks;
	void **m_pVEntry;
	void *m_pOriginalEntry;
	size_t m_iArgCount;
	const char *m_pName;
	void *m_pTrampoline;
	void *m_pTrampolineMem;
	size_t m_pTrampolineMemSz;
};

//************************************
// Class: CVTable
// Description: Manages virtual table
//		hooks and detours.
//************************************
class CVTable
{
public:
	CVTable(void **vtable);
	virtual ~CVTable(void);

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
	virtual void Hint(size_t vindex, size_t args, const char *name);

	//************************************
	// Method:    Detour
	// FullName:  CVTable<T>::Detour
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void Detour(size_t vindex, T func)
	{
		void *funcTypeless = NULL;
		__asm
		{
			mov eax, func;
			mov funcTypeless, eax;
		}
		this->Detour(vindex, funcTypeless);
	};

	//************************************
	// Method:    Detour
	// FullName:  CVTable::Detour
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void Detour(size_t vindex, void *func);

	//************************************
	// Method:    RemoveDetour
	// FullName:  CVTable::RemoveDetour
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	//************************************
	virtual void RemoveDetour(size_t vindex);

	//************************************
	// Method:    Hook
	// FullName:  CVTable<T>::Hook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void Hook(size_t vindex, T func)
	{
		void *funcTypeless = NULL;
		__asm
		{
			mov eax, func;
			mov funcTypeless, eax;
		}
		this->Hook(vindex, funcTypeless);
	};

	//************************************
	// Method:    Hook
	// FullName:  CVTable::Hook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void Hook(size_t vindex, void *func);

	//************************************
	// Method:    RemoveHook
	// FullName:  CVTable<T>::RemoveHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void RemoveHook(size_t vindex, T func)
	{
		void *funcTypeless = NULL;
		__asm
		{
			mov eax, func;
			mov funcTypeless, eax;
		}
		this->RemoveHook(vindex, funcTypeless);
	};

	//************************************
	// Method:    RemoveHook
	// FullName:  CVTable::RemoveHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void RemoveHook(size_t vindex, void *func);

	//************************************
	// Method:    Revert
	// FullName:  CVTable::Revert
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	//************************************
	virtual void Revert(size_t vindex);

	//************************************
	// Method:    RevertAll
	// FullName:  CVTable::RevertAll
	// Access:    public 
	// Returns:   void
	// Qualifier:
	//************************************
	virtual void RevertAll(void);

private:
	void **m_pVTable;

	std::map<size_t, CVEntry *>::iterator GetEntry(size_t vindex);
	virtual void *BaseVHook(void);
	virtual void CreateTrampoline(CVEntry *entry);

	std::map<size_t, CVEntry *> m_Entries;
};

#endif // vdetour_h__