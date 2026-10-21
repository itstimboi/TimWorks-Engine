#pragma once

#include <string>
#include <vector>

namespace TW
{
    std::string Base64Encode(const std::vector<unsigned char>& data);
    std::vector<unsigned char> Base64Decode(const std::string& data);
}