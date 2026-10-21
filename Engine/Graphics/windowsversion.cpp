#include <windows.h>
#include <stdio.h>

#include "windowsversion.h"

typedef LONG (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

BOOL GetRealWindowsVersion(OSVERSIONINFOEXW* osVersionInfo)
{
    if (!osVersionInfo)
        return FALSE;

    ZeroMemory(osVersionInfo, sizeof(OSVERSIONINFOEXW));
    osVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    // ------------------------------------------------------------
    // 1. Try RtlGetVersion
    // ------------------------------------------------------------

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");

    if (hNtdll)
    {
        RtlGetVersionPtr pRtlGetVersion =
            (RtlGetVersionPtr)GetProcAddress(
                hNtdll,
                "RtlGetVersion"
            );

        if (pRtlGetVersion)
        {
            OSVERSIONINFOEXW osInfoEx = {};

            osInfoEx.dwOSVersionInfoSize =
                sizeof(OSVERSIONINFOEXW);

            PRTL_OSVERSIONINFOW pRtlInfo =
                reinterpret_cast<PRTL_OSVERSIONINFOW>(&osInfoEx);

            if (pRtlGetVersion(pRtlInfo) == 0)
            {
                *osVersionInfo = osInfoEx;
                return TRUE;
            }
        }
    }

    // ------------------------------------------------------------
    // 2. Fallback: kernel32.dll file version
    // ------------------------------------------------------------

    wchar_t path[MAX_PATH];

    DWORD pathLength =
        GetModuleFileNameW(
            GetModuleHandleW(L"kernel32.dll"),
            path,
            MAX_PATH
        );

    if (pathLength == 0)
        return FALSE;

    DWORD dwHandle = 0;

    DWORD dwSize =
        GetFileVersionInfoSizeW(
            path,
            &dwHandle
        );

    if (dwSize == 0)
        return FALSE;

    BYTE* buffer = new BYTE[dwSize];

    BOOL result =
        GetFileVersionInfoW(
            path,
            0,
            dwSize,
            buffer
        );

    if (!result)
    {
        delete[] buffer;
        return FALSE;
    }

    VS_FIXEDFILEINFO* vi = nullptr;
    UINT len = 0;

    result =
        VerQueryValueW(
            buffer,
            L"\\",
            reinterpret_cast<void**>(&vi),
            &len
        );

    if (!result || !vi || len == 0)
    {
        delete[] buffer;
        return FALSE;
    }

    osVersionInfo->dwMajorVersion =
        HIWORD(vi->dwProductVersionMS);

    osVersionInfo->dwMinorVersion =
        LOWORD(vi->dwProductVersionMS);

    osVersionInfo->dwBuildNumber =
        HIWORD(vi->dwProductVersionLS);

    delete[] buffer;

    return TRUE;
}

namespace TWE
{
    WindowsVersion GetWindowsVersion()
    {
        OSVERSIONINFOEXW osvi = {};

        if (!GetRealWindowsVersion(&osvi))
            return WindowsVersion::Unknown;

        if (osvi.dwMajorVersion == 10)
        {
            if (osvi.dwBuildNumber >= 22000)
                return WindowsVersion::Windows11;

            return WindowsVersion::Windows10;
        }

        if (osvi.dwMajorVersion == 6)
        {
            if (osvi.dwMinorVersion == 3)
                return WindowsVersion::Windows81;

            if (osvi.dwMinorVersion == 2)
                return WindowsVersion::Windows8;

            if (osvi.dwMinorVersion == 1)
                return WindowsVersion::Windows7;

            if (osvi.dwMinorVersion == 0)
                return WindowsVersion::WindowsVista;
        }

        if (osvi.dwMajorVersion == 5)
        {
            if (osvi.dwMinorVersion == 1)
                return WindowsVersion::WindowsXP;

            if (osvi.dwMinorVersion == 0)
                return WindowsVersion::Windows2000;
        }

        if (osvi.dwMajorVersion == 4)
        {
            if (osvi.dwMinorVersion == 90)
                return WindowsVersion::WindowsME;

            if (osvi.dwMinorVersion == 10)
                return WindowsVersion::Windows98;

            if (osvi.dwMinorVersion == 0)
                return WindowsVersion::Windows95;
        }

        return WindowsVersion::Unknown;
    }
};

void PrintWindowsVersion()
{
    OSVERSIONINFOEXW osvi = {};

    if (!GetRealWindowsVersion(&osvi))
    {
        wprintf(L"Failed to detect Windows version\n");
        return;
    }

    wprintf(
        L"Windows version: %lu.%lu.%lu\n",
        osvi.dwMajorVersion,
        osvi.dwMinorVersion,
        osvi.dwBuildNumber
    );

    // ------------------------------------------------------------
    // Windows 10 / 11
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 10)
    {
        if (osvi.dwBuildNumber >= 22000)
            wprintf(L"Windows 11\n");
        else
            wprintf(L"Windows 10\n");

        return;
    }

    // ------------------------------------------------------------
    // Windows 8.1
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 6 &&
        osvi.dwMinorVersion == 3)
    {
        wprintf(L"Windows 8.1\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows 8
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 6 &&
        osvi.dwMinorVersion == 2)
    {
        wprintf(L"Windows 8\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows 7
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 6 &&
        osvi.dwMinorVersion == 1)
    {
        wprintf(L"Windows 7\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows Vista
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 6 &&
        osvi.dwMinorVersion == 0)
    {
        wprintf(L"Windows Vista\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows XP
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 5 &&
        osvi.dwMinorVersion == 1)
    {
        wprintf(L"Windows XP\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows 2000
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 5 &&
        osvi.dwMinorVersion == 0)
    {
        wprintf(L"Windows 2000\n");
        return;
    }

    // ------------------------------------------------------------
    // Windows 9x
    // ------------------------------------------------------------

    if (osvi.dwMajorVersion == 4 &&
        osvi.dwMinorVersion == 90)
    {
        wprintf(L"Windows ME\n");
        return;
    }

    if (osvi.dwMajorVersion == 4 &&
        osvi.dwMinorVersion == 10)
    {
        wprintf(L"Windows 98\n");
        return;
    }

    if (osvi.dwMajorVersion == 4 &&
        osvi.dwMinorVersion == 0)
    {
        wprintf(L"Windows 95\n");
        return;
    }

    wprintf(L"Unknown Windows Version\n");
}