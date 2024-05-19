#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
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

#include "Engine/WindowsInclude.h"
#include <glad/gl.h>

#include <glmHeader.h>

#include <stb/stb_image.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Engine/Core.h"
#include "Engine/Clock.h"
#include "Engine/EngineString.h"
#include "Engine/EnumArray.h"
#include "Engine/FileInputStream.h"
#include "Engine/GenerateUUID.h"
#include "Engine/Image.h"
#include "Engine/LogSystem.h"
#include "Engine/MemoryInputStream.h"
#include "Engine/Systems.h"
#include "Engine/Utils.h"

#include "Engine/Keyboard.h"
#include "Engine/Mouse.h"
#include "Engine/WindowSystem.h"

#include "Engine/RenderSystem.h"
#include "Engine/RenderCamera.h"

#include "Engine/EngineApp.h"