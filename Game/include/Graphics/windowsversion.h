#pragma once

namespace TWE
{
    enum class WindowsVersion
    {
        Unknown,

        Windows95,
        Windows98,
        WindowsME,

        Windows2000,
        WindowsXP,

        WindowsVista,
        Windows7,
        Windows8,
        Windows81,

        Windows10,
        Windows11
    };

    WindowsVersion GetWindowsVersion();
};