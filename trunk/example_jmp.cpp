#include <stdio.h>

#include "jmpswapdetour.h"

CJmpSwapDetour *detour = NULL;

__declspec(noinline) __declspec(dllexport) void __stdcall DetourMe(void)
{
	printf("DetourMe\n\n");
}

__declspec(noinline) __declspec(dllexport) void __stdcall DetourDest(void)
{
	printf("DetourDest\n");

	detour->Swap();
	detour->GetTarget<void (__stdcall *)(void)>()();
	detour->Swap();
}

void main()
{
	detour = new CJmpSwapDetour(DetourMe, DetourDest);

	DetourMe();

	delete detour;

	DetourMe();
}