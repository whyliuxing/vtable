#include "vdetour.h"

class MainClass
{
public:
	virtual void FunctionA(int arg0, int arg1)
	{
		printf("\tMainClass::FunctionA arg0:%d arg1:%d\n", arg0, arg1);
	}

	virtual double FunctionB(int arg0, int arg1, int arg2)
	{
		printf("\tMainClass::FunctionB arg0:%d arg1:%d arg2:%d\n", arg0, arg1, arg2);

		return m_MemberValue;
	}

	float m_MemberValue;
};

class AttachedClass
{
public:
	void FunctionA(int arg0, int arg1)
	{
		printf("\tAttachedClass::FunctionA arg0:%d arg1:%d\n", arg0, arg1);
	}

	void FunctionB(int arg0, int arg1, int arg2)
	{
		printf("\tAttachedClass::FunctionB arg0:%d arg1:%d arg2:%d\n", arg0, arg1, arg2);
	}
};

class AttachedClass2
{
public:
	void FunctionA(int arg0, int arg1)
	{
		printf("\tAttachedClass2::FunctionA arg0:%d arg1:%d\n", arg0, arg1);
	}

	void FunctionB(int arg0, int arg1, int arg2)
	{
		printf("\tAttachedClass2::FunctionB arg0:%d arg1:%d arg2:%d\n", arg0, arg1, arg2);
	}

	double FunctionB_Replacement(int arg0, int arg1, int arg2)
	{
		printf("\tAttachedClass2::FunctionB_Replacement arg0:%d arg1:%d arg2:%d\n", arg0, arg1, arg2);
		return 9999999.0;
	}
};

int main()
{
	MainClass *testObject = new MainClass();
	testObject->m_MemberValue = 500;

	// Call FunctionA normally.
	printf("FunctionA, no hooks:\n");
	testObject->FunctionA(1, 2);

	printf("\n\n");

	// Create the vtable modifier.
	CVTable *vtable = new CVTable(*((void ***)testObject));

	// Give CVTable information about FunctionA.
	vtable->Hint(0, 2, "FunctionA");
	vtable->CallHook<void (AttachedClass:: *)(int, int)>(0, &AttachedClass::FunctionA);
	vtable->ReturnHook<void (AttachedClass2:: *)(int, int)>(0, &AttachedClass2::FunctionA);

	printf("FunctionA, 2 hooks:\n");
	testObject->FunctionA(1, 2);

	printf("\n\n");

	// Give CVTable information about FunctionB.
	vtable->Hint(1, 3, "FunctionB");
	vtable->CallHook<void (AttachedClass:: *)(int, int, int)>(1, &AttachedClass::FunctionB);
	vtable->ReturnHook<void (AttachedClass2:: *)(int, int, int)>(1, &AttachedClass2::FunctionB);
	vtable->Detour<double (AttachedClass2:: *)(int, int, int)>(1, &AttachedClass2::FunctionB_Replacement);

	printf("FunctionB, 2 hooks + detour:\n");
	printf("\tFunctionB returned: %f\n", testObject->FunctionB(1, 2, 3));

	printf("\n\n");

	// Delete the CVTable object, restoring the table.
	delete vtable;

	printf("FunctionB, restored:\n");
	printf("\tFunctionB returned: %f\n", testObject->FunctionB(1, 2, 3));

	printf("\n\n");

	delete testObject;

	system("pause");

	return 0;
}
