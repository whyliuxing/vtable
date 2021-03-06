//////////////////////////////////////////////////////////////////////////
//  Virtual Table Detour and Hooks Class for MSVC++
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
struct CVEntry
{
	// MASM Detour:
	void *m_pDetour;
	void *m_pOriginalEntry;
	size_t m_iArgCount;

	void *m_pCallHooks;
	size_t m_iCallHookCount;

	void *m_pReturnHooks;
	size_t m_iReturnHookCount;

	// Extra:
	void **m_pVEntry;
	const char *m_pName;
	void *m_pTrampoline;
	void *m_pTrampolineMem;
	size_t m_pTrampolineMemSz;

	std::vector<void *> *m_CallHooks;
	std::vector<void *> *m_ReturnHooks;

	void UpdateHooks();
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
		union
		{
			void *funcTypeless;
			T funcTyped;
		} u;

		u.funcTyped = func;
		this->Detour(vindex, u.funcTypeless);
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
	// Method:    CallHook
	// FullName:  CVTable<T>::CallHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void CallHook(size_t vindex, T func)
	{
		union
		{
			void *funcTypeless;
			T funcTyped;
		} u;

		u.funcTyped = func;
		this->CallHook(vindex, u.funcTypeless);
	};

	//************************************
	// Method:    CallHook
	// FullName:  CVTable::CallHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void CallHook(size_t vindex, void *func);

	//************************************
	// Method:    RemoveCallHook
	// FullName:  CVTable<T>::RemoveCallHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void RemoveCallHook(size_t vindex, T func)
	{
		union
		{
			void *funcTypeless;
			T funcTyped;
		} u;

		u.funcTyped = func;
		this->RemoveCallHook(vindex, u.funcTypeless);
	};

	//************************************
	// Method:    RemoveCallHook
	// FullName:  CVTable::RemoveCallHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void RemoveCallHook(size_t vindex, void *func);

	//************************************
	// Method:    ReturnHook
	// FullName:  CVTable<T>::ReturnHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void ReturnHook(size_t vindex, T func)
	{
		union
		{
			void *funcTypeless;
			T funcTyped;
		} u;

		u.funcTyped = func;
		this->ReturnHook(vindex, u.funcTypeless);
	};

	//************************************
	// Method:    ReturnHook
	// FullName:  CVTable::ReturnHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void ReturnHook(size_t vindex, void *func);

	//************************************
	// Method:    RemoveReturnHook
	// FullName:  CVTable<T>::RemoveReturnHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: T func
	//************************************
	template<class T>
	void RemoveReturnHook(size_t vindex, T func)
	{
		void *funcTypeless = (void *)func;
		this->RemoveReturnHook(vindex, funcTypeless);
	};

	//************************************
	// Method:    RemoveReturnHook
	// FullName:  CVTable::RemoveReturnHook
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: size_t vindex
	// Parameter: void * func
	//************************************
	virtual void RemoveReturnHook(size_t vindex, void *func);

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
	//virtual void *BaseVHook(void);
	void CreateTrampoline(CVEntry *entry);
	void *FollowJmp(void *ptr) const;

	std::map<size_t, CVEntry *> m_Entries;
};

#endif // vdetour_h__