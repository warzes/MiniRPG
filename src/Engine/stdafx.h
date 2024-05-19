#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
#include <random>
#include <ratio>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <sstream>
#include <variant>
#include <queue>
#include <array>
#include <span>
#include <vector>
#include <unordered_map>

#include "WindowsInclude.h"
#include <mmsystem.h>

#include <glad/gl.h>
#include <glad/wgl.h>

#include <glmHeader.h>

#include <stb/stb_image.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Core.h"