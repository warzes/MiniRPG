#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 3)
//#	pragma warning(disable : 5039)
#endif

#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <span>
#include <sstream>

#include "Engine/WindowsInclude.h"
#include <glad/gl.h>

#include <glmHeader.h>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#include "Engine/Core.h"
#include "Engine/LogSystem.h"
#include "Engine/Systems.h"

#include "Engine/WindowSystem.h"

#include "Engine/RenderSystem.h"
#include "Engine/RenderCamera.h"

#include "Engine/EngineApp.h"