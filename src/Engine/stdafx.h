#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <span>
#include <vector>
#include <unordered_map>

#include "WindowsInclude.h"

#include <glad/gl.h>
#include <glad/wgl.h>

#include <glmHeader.h>

#include <stb/stb_image.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Core.h"