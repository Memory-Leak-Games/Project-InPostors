#pragma once

// C++ standard library headers
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <chrono>
#include <cstdint>
#include <functional>

// String
#include <string>
#include <sstream>

// Containers
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/color_space.hpp>

// Third-party libraries
#include <eventpp/eventdispatcher.h>
#include <eventpp/eventqueue.h>
#include "effolkronium/random.hpp"
#include "nlohmann/json.hpp"
#include "magic_enum.hpp"

// Our headers
#include "Core/AssetManager/AssetManager.h"
#include "Macros.h"
#include "Core/Time.h"
#include "Core/Formatters.h"
#include "Core/Math.h"

// Debug
#ifdef DEBUG
#include <imgui.h>
#endif
