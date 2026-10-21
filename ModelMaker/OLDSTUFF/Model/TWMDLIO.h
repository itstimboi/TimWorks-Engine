#pragma once

#include "TWModel.h"

#include <string>

namespace TW
{

bool SaveTWModel(
    const TWModel& model,
    const std::string& filename
);

bool LoadTWModel(
    TWModel& model,
    const std::string& filename
);

}