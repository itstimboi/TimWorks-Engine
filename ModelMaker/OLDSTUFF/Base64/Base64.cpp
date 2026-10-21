#include "Base64.h"

namespace TW
{

static const char* BASE64_CHARS =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


std::string Base64Encode(
    const std::vector<unsigned char>& data
)
{
    std::string result;

    int val = 0;
    int valb = -6;

    for (unsigned char c : data)
    {
        val = (val << 8) | c;
        valb += 8;

        while (valb >= 0)
        {
            result.push_back(
                BASE64_CHARS[
                    (val >> valb) & 0x3F
                ]
            );

            valb -= 6;
        }
    }

    if (valb > -6)
    {
        result.push_back(
            BASE64_CHARS[
                ((val << 8) >>
                 (valb + 8)) & 0x3F
            ]
        );
    }

    while (result.size() % 4)
        result.push_back('=');

    return result;
}


// std::vector<unsigned char> Base64Decode(
//     const std::string& data
// )
// {
//     static const unsigned char lookup[256] =
//     {
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,
//         52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,
//         64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
//         15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
//         31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,
//         47,48,49,50,51,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
//         64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64
//     };
// 
//     std::vector<unsigned char> result;
// 
//     int val = 0;
//     int valb = -8;
// 
//     for (unsigned char c : data)
//     {
//         if (c == '=')
//             break;
// 
//         if (lookup[c] == 64)
//             continue;
// 
//         val = (val << 6) + lookup[c];
//         valb += 6;
// 
//         if (valb >= 0)
//         {
//             result.push_back(
//                 static_cast<unsigned char>(
//                     (val >> valb) & 0xFF
//                 )
//             );
// 
//             valb -= 8;
//         }
//     }
// 
//     return result;
// }

std::vector<unsigned char> Base64Decode(
    const std::string& data
)
{
    static const std::string chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::vector<unsigned char> result;

    int val = 0;
    int valBits = -8;

    for (unsigned char c : data)
    {
        if (c == '=')
            break;

        size_t pos = chars.find(c);

        if (pos == std::string::npos)
            continue;

        val = (val << 6) | static_cast<int>(pos);
        valBits += 6;

        if (valBits >= 0)
        {
            result.push_back(
                static_cast<unsigned char>(
                    (val >> valBits) & 0xFF
                )
            );

            valBits -= 8;
        }
    }

    return result;
}

}