#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commdlg.h>

#include <string>

#include "FileDialog.h"


std::string OpenMapDialog()
{
    char filename[MAX_PATH] = {};

    OPENFILENAMEA dialog = {};

    dialog.lStructSize = sizeof(dialog);

    dialog.hwndOwner = nullptr;

    dialog.lpstrFile = filename;

    dialog.nMaxFile = MAX_PATH;

    dialog.lpstrFilter =
        "TimWorks Map (*.tw_map)\0*.tw_map\0"
        "All Files (*.*)\0*.*\0";

    dialog.nFilterIndex = 1;

    dialog.Flags =
        OFN_PATHMUSTEXIST |
        OFN_FILEMUSTEXIST |
        OFN_NOCHANGEDIR;


    if (GetOpenFileNameA(&dialog))
    {
        return std::string(filename);
    }

    return "";
}


std::string SaveMapDialog()
{
    char filename[MAX_PATH] = {};

    OPENFILENAMEA dialog = {};

    dialog.lStructSize = sizeof(dialog);

    dialog.hwndOwner = nullptr;

    dialog.lpstrFile = filename;

    dialog.nMaxFile = MAX_PATH;

    dialog.lpstrFilter =
        "TimWorks Map (*.tw_map)\0*.tw_map\0"
        "All Files (*.*)\0*.*\0";

    dialog.nFilterIndex = 1;

    dialog.lpstrDefExt = "tmap";

    dialog.Flags =
        OFN_PATHMUSTEXIST |
        OFN_OVERWRITEPROMPT |
        OFN_NOCHANGEDIR;


    if (GetSaveFileNameA(&dialog))
    {
        return std::string(filename);
    }

    return "";
}

std::filesystem::path GetProjectRoot()
{
    std::filesystem::path exePath =
        std::filesystem::absolute(
            std::filesystem::path(__argv[0])
        );

    return exePath.parent_path().parent_path();
}

std::string GetParentFolderName() 
{
    wchar_t path[MAX_PATH];
    // Get the full path of the current executable
    DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
    if (length == 0) return "";

    std::filesystem::path exePath(path);
    // .parent_path() returns the directory containing the exe
    // .filename() returns the name of that directory
    return exePath.parent_path().string();
}