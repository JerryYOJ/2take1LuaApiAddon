#include "Include.h"
#include "format.h"
#include "libs/MinHook/MinHook.h"

extern "C" {
#include "libs/Lua/lua.h"
#include "libs/Lua/lualib.h"
#include "libs/Lua/lauxlib.h"
}

using _cmdrun = void(*)(std::wstring cmd);
_cmdrun rcrashid = nullptr;
_cmdrun rkickid = nullptr;

std::unique_ptr<std::vector<lua_State*>> g_RegisteredLua;

int rcrashid_wrapper(lua_State* L) {
    rcrashid(L"rcrash " + std::to_wstring(88) + std::to_wstring((long long)lua_tonumber(L, 1)));
    return 0;
}

int rkickid_wrapper(lua_State* L) {
    rkickid(L"rkick " + std::to_wstring(88) + std::to_wstring((long long)lua_tonumber(L, 1)));
}

HOOK_DEF(void, luaL_openlibs, lua_State* L) {
    orig_luaL_openlibs(L);
    //MessageBox(NULL, std::format("%p", _ReturnAddress()).c_str(), "Addon", MB_ICONINFORMATION);
    if (!rcrashid) rcrashid = (_cmdrun)((DWORD64)_ReturnAddress() - 0x113202);
    if (!rkickid) rkickid = (_cmdrun)((DWORD64)_ReturnAddress() - 0x113812);
    lua_register(L, "rcrashid", rcrashid_wrapper);
    lua_register(L, "rkickid", rkickid_wrapper);
    g_RegisteredLua->push_back(L);
    return;
}

void ScriptMain(HMODULE hDll) {
    g_RegisteredLua.reset(new std::vector<lua_State*>);

    MH_Initialize();
    MH_CreateHook(luaL_openlibs, (void*)new_luaL_openlibs, reinterpret_cast<void**>(&orig_luaL_openlibs));
    MH_EnableHook(MH_ALL_HOOKS);

    while (!(GetAsyncKeyState(VK_END) & 1));

    for (auto L = g_RegisteredLua->begin(); L != g_RegisteredLua->end(); L++) {
        lua_pushnil(*L);
        lua_setglobal(*L, "rcrashid");
    }

    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    g_RegisteredLua.reset();

    MessageBox(NULL, "Unloaded", "Addon", MB_ICONINFORMATION);
    FreeLibraryAndExitThread(hDll, 0);
    return;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ScriptMain, hModule, NULL, NULL);
    }
    return TRUE;
}

