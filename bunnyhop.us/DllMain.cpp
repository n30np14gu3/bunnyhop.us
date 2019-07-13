#include "dependencies/common_includes.hpp"
#include "websockets/ws_api.h"
#include "bhop_api/bhop_api.h"
#include "SDK/crypto/XorStr.h"
#include "core/features/skinchanger/knifehook.hpp"

void ws_thread()
{
	ws_api::init_client(XorStr("185.22.232.35"), XorStr("3303"), XorStr("/api/software"), bhop_api::token.pToken);
	Sleep(INFINITE);
}

void loader_verification() 
{
	Sleep(1000);
	if (!bhop_api::loader_verify())
		TerminateProcess(GetCurrentProcess(), ~0);
}

void MainThread()
{
	try
	{
		loader_verification();
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ws_thread), nullptr, 0, nullptr);
		interfaces::initialize();
		hooks::initialize();
		knife_hook.knife_animation();
		Beep(600, 600);

		while (!GetAsyncKeyState(VK_END))
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

		hooks::shutdown();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	catch (const std::runtime_error & err)
	{

	}
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), nullptr, 0, nullptr);
		break;
	default:
		break;
	}
	return TRUE;
}