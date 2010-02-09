#include "vdetour.h"

class VTest
{
public:
	virtual void Test(int lol, int lol2)
	{
		printf("Main %d %d\n", lol, lol2);
	}

	virtual double Test2(int lol, int lol2, int lol3)
	{
		printf("SECOND %d %d %d\n", lol, lol2, lol3);
		return bob;
	}

	float bob;
};

class VTest2
{
public:
	void Test(int lol, int lol2)
	{
		printf("2nd Main %d %d\n", lol, lol2);
	}

	void Test2(int lol, int lol2, int lol3)
	{
		printf("SECOND2 %d %d %d\n", lol, lol2, lol3);
	}
};

class VTest3
{
public:
	void Test(int lol, int lol2)
	{
		printf("3rd Main %d %d\n", lol, lol2);
		return;
	}

	void Test2(int lol, int lol2, int lol3)
	{
		printf("SECOND3 %d %d %d\n", lol, lol2, lol3);
	}

	double Test3(int lol, int lol2, int lol3)
	{
		printf("SECOND3Return %d %d %d\n", lol, lol2, lol3);
		return 9999999.0;
	}
};


int main()
{
	VTest *test = new VTest();

	test->bob = 500;

	test->Test(1, 2);

	CVTable *vtable = new CVTable(*((void ***)test));

	vtable->Hint(0, 2, "Test");
	vtable->Hook<void (VTest2:: *)(int, int)>(0, &VTest2::Test);
	vtable->Hook<void (VTest3:: *)(int, int)>(0, &VTest3::Test);

	test->Test(1, 2);

	vtable->Hint(1, 3, "Test2");
	vtable->Hook<void (VTest2:: *)(int, int, int)>(1, &VTest2::Test2);
	vtable->Hook<void (VTest3:: *)(int, int, int)>(1, &VTest3::Test2);
	vtable->Detour<double (VTest3:: *)(int, int, int)>(1, &VTest3::Test3);

	printf("LOLOLOL %f\n", test->Test2(1, 2, 3));

	delete vtable;

	printf("LOLOLOL %f\n", test->Test2(1, 2, 3));

	delete test;

	return 0;
}
