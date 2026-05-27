// version.dll proxy: forwards every export to the real system DLL via PE
// forwarders, and spawns the bridge on DLL_PROCESS_ATTACH so the loader is
// never blocked on FMOD discovery or HTTP startup.

// Disable exceptions for DllMain to avoid C++ runtime initialization issues under Wine/Proton.
// The mod spawns a separate thread (bridge_thread) which will have exceptions enabled.
#if defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC optimize("-fno-exceptions")
#endif

#include <windows.h>

// Forwarding to "version.dll" without a path resolves back to us; the
// absolute system32 path is what breaks the basename collision.
//
// MSVC: __pragma(comment(linker, "/EXPORT:..."))
// MinGW: Use .def file or asm directives. We use .def file approach via
// CMakeLists.txt which generates version.def from this list.
#ifdef _MSC_VER
#define FWD(name) \
    __pragma(comment(linker, "/EXPORT:" #name "=C:\\Windows\\System32\\version." #name))
FWD(GetFileVersionInfoA)
FWD(GetFileVersionInfoByHandle)
FWD(GetFileVersionInfoExA)
FWD(GetFileVersionInfoExW)
FWD(GetFileVersionInfoSizeA)
FWD(GetFileVersionInfoSizeExA)
FWD(GetFileVersionInfoSizeExW)
FWD(GetFileVersionInfoSizeW)
FWD(GetFileVersionInfoW)
FWD(VerFindFileA)
FWD(VerFindFileW)
FWD(VerInstallFileA)
FWD(VerInstallFileW)
FWD(VerLanguageNameA)
FWD(VerLanguageNameW)
FWD(VerQueryValueA)
FWD(VerQueryValueW)
#undef FWD
#elif defined(__MINGW32__) || defined(__MINGW64__)
namespace {
template <typename Fn>
Fn resolve_version_proc(const char* name) noexcept {
    static HMODULE module = []() noexcept -> HMODULE {
        return LoadLibraryW(L"C:\\Windows\\System32\\version.dll");
    }();
    return module ? reinterpret_cast<Fn>(GetProcAddress(module, name)) : nullptr;
}
} // namespace

extern "C" {
__declspec(dllexport) WINBOOL WINAPI GetFileVersionInfoA(LPCSTR filename, DWORD handle, DWORD data_size, LPVOID data) {
    using Fn = WINBOOL (WINAPI *)(LPCSTR, DWORD, DWORD, LPVOID);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoA");
    return fn ? fn(filename, handle, data_size, data) : FALSE;
}

__declspec(dllexport) WINBOOL WINAPI GetFileVersionInfoByHandle(void* handle, DWORD data_size, LPVOID data) {
    using Fn = WINBOOL (WINAPI *)(void*, DWORD, LPVOID);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoByHandle");
    return fn ? fn(handle, data_size, data) : FALSE;
}

__declspec(dllexport) WINBOOL WINAPI GetFileVersionInfoExA(DWORD flags, LPCSTR filename, DWORD handle, DWORD data_size, LPVOID data) {
    using Fn = WINBOOL (WINAPI *)(DWORD, LPCSTR, DWORD, DWORD, LPVOID);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoExA");
    return fn ? fn(flags, filename, handle, data_size, data) : FALSE;
}

__declspec(dllexport) WINBOOL WINAPI GetFileVersionInfoExW(DWORD flags, LPCWSTR filename, DWORD handle, DWORD data_size, LPVOID data) {
    using Fn = WINBOOL (WINAPI *)(DWORD, LPCWSTR, DWORD, DWORD, LPVOID);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoExW");
    return fn ? fn(flags, filename, handle, data_size, data) : FALSE;
}

__declspec(dllexport) DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR filename, LPDWORD handle) {
    using Fn = DWORD (WINAPI *)(LPCSTR, LPDWORD);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoSizeA");
    return fn ? fn(filename, handle) : 0;
}

__declspec(dllexport) DWORD WINAPI GetFileVersionInfoSizeExA(DWORD flags, LPCSTR filename, LPDWORD handle) {
    using Fn = DWORD (WINAPI *)(DWORD, LPCSTR, LPDWORD);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoSizeExA");
    return fn ? fn(flags, filename, handle) : 0;
}

__declspec(dllexport) DWORD WINAPI GetFileVersionInfoSizeExW(DWORD flags, LPCWSTR filename, LPDWORD handle) {
    using Fn = DWORD (WINAPI *)(DWORD, LPCWSTR, LPDWORD);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoSizeExW");
    return fn ? fn(flags, filename, handle) : 0;
}

__declspec(dllexport) DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR filename, LPDWORD handle) {
    using Fn = DWORD (WINAPI *)(LPCWSTR, LPDWORD);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoSizeW");
    return fn ? fn(filename, handle) : 0;
}

__declspec(dllexport) WINBOOL WINAPI GetFileVersionInfoW(LPCWSTR filename, DWORD handle, DWORD data_size, LPVOID data) {
    using Fn = WINBOOL (WINAPI *)(LPCWSTR, DWORD, DWORD, LPVOID);
    auto fn = resolve_version_proc<Fn>("GetFileVersionInfoW");
    return fn ? fn(filename, handle, data_size, data) : FALSE;
}

__declspec(dllexport) DWORD WINAPI VerFindFileA(DWORD flags, LPSTR file_name, LPSTR win_dir, LPSTR app_dir, LPSTR cur_dir, PUINT cur_dir_len, LPSTR dest_dir, PUINT dest_dir_len) {
    using Fn = DWORD (WINAPI *)(DWORD, LPSTR, LPSTR, LPSTR, LPSTR, PUINT, LPSTR, PUINT);
    auto fn = resolve_version_proc<Fn>("VerFindFileA");
    return fn ? fn(flags, file_name, win_dir, app_dir, cur_dir, cur_dir_len, dest_dir, dest_dir_len) : 0;
}

__declspec(dllexport) DWORD WINAPI VerFindFileW(DWORD flags, LPWSTR file_name, LPWSTR win_dir, LPWSTR app_dir, LPWSTR cur_dir, PUINT cur_dir_len, LPWSTR dest_dir, PUINT dest_dir_len) {
    using Fn = DWORD (WINAPI *)(DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT, LPWSTR, PUINT);
    auto fn = resolve_version_proc<Fn>("VerFindFileW");
    return fn ? fn(flags, file_name, win_dir, app_dir, cur_dir, cur_dir_len, dest_dir, dest_dir_len) : 0;
}

__declspec(dllexport) DWORD WINAPI VerInstallFileA(DWORD flags, LPSTR src_file, LPSTR dest_file, LPSTR src_dir, LPSTR dest_dir, LPSTR cur_dir, LPSTR tmp_file, PUINT tmp_file_len) {
    using Fn = DWORD (WINAPI *)(DWORD, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR, PUINT);
    auto fn = resolve_version_proc<Fn>("VerInstallFileA");
    return fn ? fn(flags, src_file, dest_file, src_dir, dest_dir, cur_dir, tmp_file, tmp_file_len) : 0;
}

__declspec(dllexport) DWORD WINAPI VerInstallFileW(DWORD flags, LPWSTR src_file, LPWSTR dest_file, LPWSTR src_dir, LPWSTR dest_dir, LPWSTR cur_dir, LPWSTR tmp_file, PUINT tmp_file_len) {
    using Fn = DWORD (WINAPI *)(DWORD, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, LPWSTR, PUINT);
    auto fn = resolve_version_proc<Fn>("VerInstallFileW");
    return fn ? fn(flags, src_file, dest_file, src_dir, dest_dir, cur_dir, tmp_file, tmp_file_len) : 0;
}

__declspec(dllexport) DWORD WINAPI VerLanguageNameA(DWORD lang, LPSTR buffer, DWORD size) {
    using Fn = DWORD (WINAPI *)(DWORD, LPSTR, DWORD);
    auto fn = resolve_version_proc<Fn>("VerLanguageNameA");
    return fn ? fn(lang, buffer, size) : 0;
}

__declspec(dllexport) DWORD WINAPI VerLanguageNameW(DWORD lang, LPWSTR buffer, DWORD size) {
    using Fn = DWORD (WINAPI *)(DWORD, LPWSTR, DWORD);
    auto fn = resolve_version_proc<Fn>("VerLanguageNameW");
    return fn ? fn(lang, buffer, size) : 0;
}

__declspec(dllexport) WINBOOL WINAPI VerQueryValueA(LPCVOID block, LPCSTR sub_block, LPVOID* value, PUINT len) {
    using Fn = WINBOOL (WINAPI *)(LPCVOID, LPCSTR, LPVOID*, PUINT);
    auto fn = resolve_version_proc<Fn>("VerQueryValueA");
    return fn ? fn(block, sub_block, value, len) : FALSE;
}

__declspec(dllexport) WINBOOL WINAPI VerQueryValueW(LPCVOID block, LPCWSTR sub_block, LPVOID* value, PUINT len) {
    using Fn = WINBOOL (WINAPI *)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
    auto fn = resolve_version_proc<Fn>("VerQueryValueW");
    return fn ? fn(block, sub_block, value, len) : FALSE;
}
}
#else
extern "C" {
    __declspec(dllexport) void* GetFileVersionInfoA(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoByHandle(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoExA(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoExW(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoSizeA(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoSizeExA(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoSizeExW(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoSizeW(void) { return nullptr; }
    __declspec(dllexport) void* GetFileVersionInfoW(void) { return nullptr; }
    __declspec(dllexport) void* VerFindFileA(void) { return nullptr; }
    __declspec(dllexport) void* VerFindFileW(void) { return nullptr; }
    __declspec(dllexport) void* VerInstallFileA(void) { return nullptr; }
    __declspec(dllexport) void* VerInstallFileW(void) { return nullptr; }
    __declspec(dllexport) void* VerLanguageNameA(void) { return nullptr; }
    __declspec(dllexport) void* VerLanguageNameW(void) { return nullptr; }
    __declspec(dllexport) void* VerQueryValueA(void) { return nullptr; }
    __declspec(dllexport) void* VerQueryValueW(void) { return nullptr; }
}
#endif

namespace fh6 {
void run_bridge(HMODULE self) noexcept;
} // namespace fh6

namespace {
DWORD WINAPI bridge_thread(LPVOID self) {
    fh6::run_bridge(static_cast<HMODULE>(self));
    return 0;
}
} // namespace

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) noexcept {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        if (HANDLE t = CreateThread(nullptr, 0, bridge_thread, hModule, 0, nullptr)) CloseHandle(t);
    }
    return TRUE;
}

#if defined(__GNUC__) && !defined(_MSC_VER)
#pragma GCC diagnostic pop
#endif

