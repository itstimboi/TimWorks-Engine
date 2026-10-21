#pragma once

#include <string>
#include <filesystem>
#include <fstream>

std::string OpenMDLDialog();

std::string SaveMDLDialog();

std::string OpenTextureDialog();

std::filesystem::path GetProjectRoot();
std::string GetParentFolderName();